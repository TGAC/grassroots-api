/*  $Id: ncbicgir.cpp 446388 2014-09-15 17:56:27Z ivanov $
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
* Authors:  Eugene Vasilchenko, Denis Vakatov
*
* File Description:
*   CCgiResponse  -- CGI response generator class
*
*/


#include <ncbi_pch.hpp>
#include <corelib/request_ctx.hpp>
#include <cgi/ncbicgir.hpp>
#include <cgi/cgi_exception.hpp>
#include <cgi/cgi_session.hpp>
#include <cgi/cgictx.hpp>
#include <cgi/cgiapp.hpp>
#include <corelib/ncbi_safe_static.hpp>
#include <cgi/error_codes.hpp>
#include <time.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  define STDOUT_FILENO 1
#endif


#define NCBI_USE_ERRCODE_X   Cgi_Response


BEGIN_NCBI_SCOPE


const char* CCgiResponse::sm_ContentTypeName    = "Content-Type";
const char* CCgiResponse::sm_LocationName       = "Location";
const char* CCgiResponse::sm_ContentTypeDefault = "text/html";
const char* CCgiResponse::sm_ContentTypeMixed   = "multipart/mixed";
const char* CCgiResponse::sm_ContentTypeRelated = "multipart/related";
const char* CCgiResponse::sm_ContentTypeXMR     = "multipart/x-mixed-replace";
const char* CCgiResponse::sm_ContentDispoName   = "Content-Disposition";
const char* CCgiResponse::sm_FilenamePrefix     = "attachment; filename=\"";
const char* CCgiResponse::sm_HTTPStatusName     = "Status";
const char* CCgiResponse::sm_HTTPStatusDefault  = "200 OK";
const char* CCgiResponse::sm_BoundaryPrefix     = "NCBI_CGI_Boundary_";
const char* CCgiResponse::sm_CacheControl       = "Cache-Control";
const char* CCgiResponse::sm_AcceptRanges       = "Accept-Ranges";
const char* CCgiResponse::sm_AcceptRangesBytes  = "bytes";
const char* CCgiResponse::sm_ContentRange       = "Content-Range";

NCBI_PARAM_DEF_IN_SCOPE(bool, CGI, ThrowOnBadOutput, true, CCgiResponse);
NCBI_PARAM_DEF_IN_SCOPE(bool, CGI, ExceptionAfterHEAD, false, CCgiResponse);


// Helper writer and stream used to disable real output stream during
// HEAD requests after sending headers so that the application can not
// output more data or clear 'bad' bit.
class CCgiStreamWrapperWriter : public IWriter
{
public:
    CCgiStreamWrapperWriter(void)
        : m_ErrorReported(false) {}

    virtual ERW_Result Write(const void* buf,
                             size_t      count,
                             size_t*     bytes_written = 0)
    {
        if ( !m_ErrorReported ) {
            ERR_POST_X(4, "CCgiStreamWrapperWriter::Write() -- attempt to "
                "write data after sending headers on HEAD request.");
            m_ErrorReported = true;
        }
        // Pretend the operation was successfull so that applications
        // which check I/O result do not fail.
        if ( bytes_written ) {
            *bytes_written = count;
        }
        return eRW_Success;
    }

    virtual ERW_Result Flush(void)
    {
        return eRW_Success;
    }

private:
    bool m_ErrorReported;
};


class CCgiStreamWrapper : public CWStream
{
public:
    CCgiStreamWrapper(CNcbiOstream* out)
        : CWStream(new CCgiStreamWrapperWriter, 0, 0, CRWStreambuf::fOwnWriter),
          m_Output(out),
          m_OldBuf(0),
          m_NewBuf(rdbuf())
    {
        if ( m_Output ) {
            // Prevent output from writing anything, disable exceptions -
            // an attemp to write will be reported by the wrapper.
            m_Output->exceptions(ios_base::goodbit);
            m_Output->setstate(ios_base::badbit);
            // Replace rdbuf with the customized one, which does not
            // write anything, but reports an error (once).
            m_Output->flush();
            m_OldBuf = m_Output->rdbuf();
            m_Output->rdbuf(m_NewBuf);
        }
    }

    virtual ~CCgiStreamWrapper(void)
    {
        // Restore the original rdbuf so that it can be safely destroyed.
        if (m_Output  &&  m_Output->rdbuf() == m_NewBuf) {
            m_Output->rdbuf(m_OldBuf);
        }
    }

private:
    CNcbiOstream*           m_Output;
    CNcbiStreambuf*         m_OldBuf;
    CNcbiStreambuf*         m_NewBuf;
};


inline bool s_ZeroTime(const tm& date)
{
    static const tm kZeroTime = { 0 };
    return ::memcmp(&date, &kZeroTime, sizeof(tm)) == 0;
}


CCgiResponse::CCgiResponse(CNcbiOstream* os, int ofd)
    : m_IsRawCgi(false),
      m_IsMultipart(eMultipart_none),
      m_BetweenParts(false),
      m_Output(NULL),
      m_OutputFD(0),
      m_HeaderWritten(false),
      m_RequestMethod(CCgiRequest::eMethod_Other),
      m_Session(NULL),
      m_DisableTrackingCookie(false)
{
    SetOutput(os ? os  : &NcbiCout,
              os ? ofd : STDOUT_FILENO  // "os" on this line is NOT a typo
              );
}


CCgiResponse::~CCgiResponse(void)
{
    x_RestoreOutputExceptions();
}


bool CCgiResponse::HaveHeaderValue(const string& name) const
{
    return m_HeaderValues.find(name) != m_HeaderValues.end();
}


string CCgiResponse::GetHeaderValue(const string &name) const
{
    TMap::const_iterator ptr = m_HeaderValues.find(name);

    return (ptr == m_HeaderValues.end()) ? kEmptyStr : ptr->second;
}


void CCgiResponse::RemoveHeaderValue(const string& name)
{
    m_HeaderValues.erase(name);
}


bool CCgiResponse::x_ValidateHeader(const string& name, const string& value) const
{
    // Very basic validation of names - prohibit CR/LF.
    if (name.find("\n", 0) != NPOS) {
        return false;
    }
    // Values may contain [CR/]LF, but only if followed by space or tab.
    size_t pos = value.find("\n", 0);
    while (pos != NPOS) {
        ++pos;
        if (pos >= value.size()) break;
        if (value[pos] != ' '  &&  value[pos] != '\t') {
            return false;
        }
        pos = value.find("\n", pos);
    }
    return true;
}


void CCgiResponse::SetHeaderValue(const string& name, const string& value)
{
    if ( value.empty() ) {
        RemoveHeaderValue(name);
    } else {
        if ( x_ValidateHeader(name, value) ) {
            m_HeaderValues[name] = value;
        }
        else {
            NCBI_THROW(CCgiResponseException, eDoubleHeader,
                       "CCgiResponse::SetHeaderValue() -- "
                       "invalid header name or value: " +
                       name + "=" + value);
        }
    }
}


void CCgiResponse::SetHeaderValue(const string& name, const struct tm& date)
{
    if ( s_ZeroTime(date) ) {
        RemoveHeaderValue(name);
        return;
    }

    char buff[64];
    if ( !::strftime(buff, sizeof(buff),
                     "%a, %d %b %Y %H:%M:%S GMT", &date) ) {
        NCBI_THROW(CCgiErrnoException, eErrno,
                   "CCgiResponse::SetHeaderValue() -- strftime() failed");
    }
    SetHeaderValue(name, buff);
}


void CCgiResponse::SetHeaderValue(const string& name, const CTime& date)
{
    if ( date.IsEmpty()) {
        RemoveHeaderValue(name);
        return;
    }
    SetHeaderValue(name,
                   date.GetGmtTime().AsString("w, D b Y h:m:s") + " GMT");
}


void CCgiResponse::SetStatus(unsigned int code, const string& reason)
{
    if (code < 100) {
        THROW1_TRACE(runtime_error,
                     "CCgiResponse::SetStatus() -- code too small, below 100");
    }
    if (code > 999) {
        THROW1_TRACE(runtime_error,
                     "CCgiResponse::SetStatus() -- code too big, exceeds 999");
    }
    SetHeaderValue(sm_HTTPStatusName, NStr::UIntToString(code) + ' ' +
        (reason.empty() ?
        CCgiException::GetStdStatusMessage(CCgiException::EStatusCode(code))
        : reason));
    CDiagContext::GetRequestContext().SetRequestStatus(code);
}


void CCgiResponse::x_RestoreOutputExceptions(void)
{
    if (m_Output  &&  m_ThrowOnBadOutput.Get()) {
        m_Output->exceptions(m_OutputExpt);
    }
}


void CCgiResponse::SetOutput(CNcbiOstream* out, int fd)
{
    x_RestoreOutputExceptions();

    m_HeaderWritten = false;
    m_Output        = out;
    m_OutputFD      = fd;

    // Make the output stream to throw on write if it's in a bad state
    if (m_Output  &&  m_ThrowOnBadOutput.Get()) {
        m_OutputExpt = m_Output->exceptions();
        m_Output->exceptions(IOS_BASE::badbit | IOS_BASE::failbit);
    }
}


CNcbiOstream* CCgiResponse::GetOutput(void) const
{
    bool client_int_ok = TClientConnIntOk::GetDefault()  ||
        (AcceptRangesBytes()  && !HaveContentRange());

    if (m_Output  &&
        !client_int_ok  &&
        !(m_RequestMethod == CCgiRequest::eMethod_HEAD  &&  m_HeaderWritten)  &&
        (m_Output->rdstate()  &  (IOS_BASE::badbit | IOS_BASE::failbit)) != 0  &&
        m_ThrowOnBadOutput.Get()) {
        ERR_POST_X(1, Severity(TClientConnIntSeverity::GetDefault()) <<
                   "CCgiResponse::GetOutput() -- output stream is in bad state");
        const_cast<CCgiResponse*>(this)->SetThrowOnBadOutput(false);
    }
    return m_Output;
}


CNcbiOstream& CCgiResponse::out(void) const
{
    if ( !m_Output ) {
        THROW1_TRACE(runtime_error, "CCgiResponse::out() on NULL out.stream");
    }
    return *GetOutput();
}


CNcbiOstream& CCgiResponse::WriteHeader(CNcbiOstream& os) const
{
    if (&os == m_Output) {
        if (m_HeaderWritten) {
            NCBI_THROW(CCgiResponseException, eDoubleHeader,
                       "CCgiResponse::WriteHeader() -- called more than once");
        } else
            m_HeaderWritten = true;
    }

    // HTTP status line (if "raw CGI" response)
    bool skip_status = false;
    if ( IsRawCgi() ) {
        string status = GetHeaderValue(sm_HTTPStatusName);
        if ( status.empty() ) {
            status = sm_HTTPStatusDefault;
        } else {
            skip_status = true;  // filter out the status from the HTTP header
        }
        os << "HTTP/1.1 " << status << HTTP_EOL;
    }

    if (m_IsMultipart != eMultipart_none
        &&  CCgiUserAgent().GetEngine() == CCgiUserAgent::eEngine_IE) {
        // MSIE requires multipart responses to start with these extra
        // headers, which confuse other browsers. :-/
        os << sm_ContentTypeName << ": message/rfc822" << HTTP_EOL << HTTP_EOL
           << "Mime-Version: 1.0" << HTTP_EOL;
    }

    // Dirty hack (JQuery JSONP for browsers that don't support CORS)
    if ( !m_JQuery_Callback.empty() ) {
        CCgiResponse* self = const_cast<CCgiResponse*>(this);
        if (m_IsMultipart == eMultipart_none)
            self->SetHeaderValue(sm_ContentTypeName, "text/javascript");
        else
            self->m_JQuery_Callback.erase();
    }

    // Default content type (if it's not specified by user already)
    switch (m_IsMultipart) {
    case eMultipart_none:
        if ( !HaveHeaderValue(sm_ContentTypeName) ) {
            os << sm_ContentTypeName << ": " << sm_ContentTypeDefault
               << HTTP_EOL;
        }
        break;
    case eMultipart_mixed:
        os << sm_ContentTypeName << ": " << sm_ContentTypeMixed
           << "; boundary=" << m_Boundary << HTTP_EOL;
        break;
    case eMultipart_related:
        os << sm_ContentTypeName << ": " << sm_ContentTypeRelated
           << "; type=" << (HaveHeaderValue(sm_ContentTypeName)
                            ? sm_ContentTypeName : sm_ContentTypeDefault)
           << "; boundary=" << m_Boundary << HTTP_EOL;
        break;
    case eMultipart_replace:
        os << sm_ContentTypeName << ": " << sm_ContentTypeXMR
           << "; boundary=" << m_Boundary << HTTP_EOL;
        break;
    }

    if (m_Session) {
        const CCgiCookie* const scookie = m_Session->GetSessionCookie();
        if (scookie) {
            const_cast<CCgiResponse*>(this)->m_Cookies.Add(*scookie);
        }
    }
    if (!m_DisableTrackingCookie  &&  m_TrackingCookie.get()) {
        CCgiResponse* self = const_cast<CCgiResponse*>(this);
        self->m_Cookies.Add(*m_TrackingCookie);
        self->SetHeaderValue(TCGI_TrackingTagName::GetDefault(),
            m_TrackingCookie->GetValue());
        self->SetHeaderValue("NCBI-PHID",
            GetDiagContext().GetRequestContext().GetHitID());
        // Prevent storing the page in public caches.
        string cc = GetHeaderValue(sm_CacheControl);
        if ( cc.empty() ) {
            cc = "private";
        }
        else {
            // 'private' already present?
            if (NStr::FindNoCase(cc, "private") == NPOS) {
                // no - check for 'public'
                size_t pos = NStr::FindNoCase(cc, "public");
                if (pos != NPOS) {
                    ERR_POST_X_ONCE(3, Warning <<
                        "Cache-Control already set to 'public', "
                        "switching to 'private'");
                    NStr::ReplaceInPlace(cc, "public", "private", pos, 1);
                }
                else if (NStr::FindNoCase(cc, "no-cache") == NPOS) {
                    cc.append(", private");
                }
            }
        }
        self->SetHeaderValue(sm_CacheControl, cc);
    }

    // Cookies (if any)
    if ( !m_Cookies.Empty() ) {
        os << m_Cookies;
    }

    // All header lines (in alphabetical order)
    ITERATE (TMap, i, m_HeaderValues) {
        if (skip_status  &&  NStr::EqualNocase(i->first, sm_HTTPStatusName)) {
            continue;
        } else if (m_IsMultipart != eMultipart_none
                   &&  NStr::StartsWith(i->first, "Content-", NStr::eNocase)) {
            continue;
        }
        os << i->first << ": " << i->second << HTTP_EOL;
    }

    if (m_IsMultipart != eMultipart_none) { // proceed with first part
        os << HTTP_EOL << "--" << m_Boundary << HTTP_EOL;
        if ( !HaveHeaderValue(sm_ContentTypeName) ) {
            os << sm_ContentTypeName << ": " << sm_ContentTypeDefault
               << HTTP_EOL;
        }
        for (TMap::const_iterator it = m_HeaderValues.lower_bound("Content-");
             it != m_HeaderValues.end()
             &&  NStr::StartsWith(it->first, "Content-", NStr::eNocase);
             ++it) {
            os << it->first << ": " << it->second << HTTP_EOL;
        }
    }

    // End of header (empty line)
    os << HTTP_EOL;

    // Dirty hack (JQuery JSONP for browsers that don't support CORS)
    if ( !m_JQuery_Callback.empty() ) {
        os << m_JQuery_Callback << '(';
    }

    if (m_RequestMethod == CCgiRequest::eMethod_HEAD  &&  &os == m_Output
        &&  !m_OutputWrapper.get()) {
        try {
            m_OutputWrapper.reset(new CCgiStreamWrapper(m_Output));
        }
        catch (ios_base::failure&) {
        }
        if ( m_ExceptionAfterHEAD.Get() ) {
            // Optionally stop processing request immediately. The exception
            // should not be handles by ProcessRequest, but must go up to
            // the Run() to work correctly.
            NCBI_CGI_THROW_WITH_STATUS(CCgiHeadException, eHeaderSent,
                "HEAD response sent.", CCgiException::e200_Ok);
        }
    }

    return os;
}


void CCgiResponse::Finalize(void) const
{
    if (!m_JQuery_Callback.empty()  &&  m_Output  &&  m_HeaderWritten) {
        *m_Output <<  ')';
    }
}


void CCgiResponse::SetFilename(const string& name, size_t size)
{
    string disposition = sm_FilenamePrefix + NStr::PrintableString(name) + '"';
    if (size > 0) {
        disposition += "; size=";
        disposition += NStr::SizetToString(size);
    }
    SetHeaderValue(sm_ContentDispoName, disposition);
}


void CCgiResponse::BeginPart(const string& name, const string& type_in,
                             CNcbiOstream& os, size_t size)
{
    _ASSERT(m_IsMultipart != eMultipart_none);
    if ( !m_BetweenParts ) {
        os << HTTP_EOL << "--" << m_Boundary << HTTP_EOL;
    }

    string type = type_in;
    if (type.empty() /* &&  m_IsMultipart == eMultipart_replace */) {
        type = GetHeaderValue(sm_ContentTypeName);
    }
    os << sm_ContentTypeName << ": "
       << (type.empty() ? sm_ContentTypeDefault : type) << HTTP_EOL;

    if ( !name.empty() ) {
        os << sm_ContentDispoName << ": " << sm_FilenamePrefix
           << Printable(name) << '"';
        if (size > 0) {
            os << "; size=" << size;
        }
        os << HTTP_EOL;
    } else if (m_IsMultipart != eMultipart_replace) {
        ERR_POST_X(2, Warning << "multipart content contains anonymous part");
    }

    os << HTTP_EOL;
}


void CCgiResponse::EndPart(CNcbiOstream& os)
{
    _ASSERT(m_IsMultipart != eMultipart_none);
    if ( !m_BetweenParts ) {
        os << HTTP_EOL << "--" << m_Boundary << HTTP_EOL << NcbiFlush;
    }
    m_BetweenParts = true;
}


void CCgiResponse::EndLastPart(CNcbiOstream& os)
{
    _ASSERT(m_IsMultipart != eMultipart_none);
    os << HTTP_EOL << "--" << m_Boundary << "--" << HTTP_EOL << NcbiFlush;
    m_IsMultipart = eMultipart_none; // forbid adding more parts
}


void CCgiResponse::Flush(void) const
{
    CNcbiOstream* os = GetOutput();
    if (!os  ||  !os->good()) {
        return; // Don't try to flush NULL or broken output
    }
    *os << NcbiFlush;
}


void CCgiResponse::SetTrackingCookie(const string& name, const string& value,
                                     const string& domain, const string& path,
                                     const CTime& exp_time)
{
    m_TrackingCookie.reset(new CCgiCookie(name, value, domain, path));
    if ( !exp_time.IsEmpty() ) {
        m_TrackingCookie->SetExpTime(exp_time);
    }
    else {
        // Set the cookie for one year by default.
        CTime def_exp(CTime::eCurrent, CTime::eGmt);
        def_exp.AddYear(1);
        m_TrackingCookie->SetExpTime(def_exp);
    }
}


void CCgiResponse::DisableTrackingCookie(void)
{
    m_DisableTrackingCookie = true;
}


void CCgiResponse::SetThrowOnBadOutput(bool throw_on_bad_output)
{
    m_ThrowOnBadOutput.Set(throw_on_bad_output);
    if (m_Output  &&  throw_on_bad_output) {
        m_OutputExpt = m_Output->exceptions();
        m_Output->exceptions(IOS_BASE::badbit | IOS_BASE::failbit);
    }
}


void CCgiResponse::SetExceptionAfterHEAD(bool expt_after_head)
{
    m_ExceptionAfterHEAD.Set(expt_after_head);
}


bool CCgiResponse::AcceptRangesBytes(void) const
{
    string accept_ranges = NStr::TruncateSpaces(GetHeaderValue(sm_AcceptRanges));
    return NStr::EqualNocase(accept_ranges, sm_AcceptRangesBytes);
}


bool CCgiResponse::HaveContentRange(void) const
{
    return HaveHeaderValue(sm_ContentRange);
}


void CCgiResponse::InitCORSHeaders(const string& /*origin*/,
                                   const string& /*jquery_callback*/)
{
    // This method is deprecated and does nothing.
    // Use CCgiContext::ProcessCORSRequest().
}


END_NCBI_SCOPE
