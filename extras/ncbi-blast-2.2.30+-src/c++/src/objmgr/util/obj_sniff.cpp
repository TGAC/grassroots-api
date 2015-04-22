/*  $Id: obj_sniff.cpp 439868 2014-07-07 15:40:40Z vasilche $
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
* Author: Anatoliy Kuznetsov
*
* File Description: Objects sniffer implementation.
*   
*/

#include <ncbi_pch.hpp>
#include <objmgr/util/obj_sniff.hpp>
#include <objmgr/error_codes.hpp>

#include <serial/iterator.hpp>
#include <serial/serial.hpp>
#include <serial/objhook.hpp>
#include <serial/iterator.hpp>


#define NCBI_USE_ERRCODE_X   ObjMgr_Sniffer


BEGIN_NCBI_SCOPE

NCBI_DEFINE_ERR_SUBCODE_X(5);

BEGIN_SCOPE(objects)



//////////////////////////////////////////////////////////////////
//
// Internal class capturing stream offsets of objects while 
// ASN parsing.
//
class COffsetReadHook : public CReadObjectHook
{
public:
    COffsetReadHook(CObjectsSniffer* sniffer, 
                    CObjectsSniffer::EEventCallBackMode event_mode)
    : m_Sniffer(sniffer),
      m_EventMode(event_mode)
    {
        _ASSERT(sniffer);
    }

    virtual void  ReadObject (CObjectIStream &in, const CObjectInfo &object);
private:
    CObjectsSniffer*                     m_Sniffer;
    CObjectsSniffer::EEventCallBackMode  m_EventMode;
};

namespace {

class CCallStackGuard {
public:
    CCallStackGuard(CObjectsSniffer::TObjectStack& stack,
                    const CObjectInfo& object)
        : m_Stack(stack)
        {
            m_Stack.push_back(&object);
        }
    ~CCallStackGuard(void)
        {
            m_Stack.pop_back();
        }
private:
    CObjectsSniffer::TObjectStack& m_Stack;
};

}

void COffsetReadHook::ReadObject(CObjectIStream &in, 
                                 const CObjectInfo &object)
{
    CCallStackGuard guard(m_Sniffer->m_CallStack, object);
    
    if (m_EventMode == CObjectsSniffer::eCallAlways) {

        // Clear the discard flag before calling sniffer's event reactors
        m_Sniffer->SetDiscardCurrObject(false);

        m_Sniffer->OnObjectFoundPre(object, in.GetStreamPos());
     
        DefaultRead(in, object);

        m_Sniffer->OnObjectFoundPost(object);

        // Relay discard flag to the stream
        bool discard = m_Sniffer->GetDiscardCurrObject();
        in.SetDiscardCurrObject(discard);
    }
    else {
        if (m_EventMode == CObjectsSniffer::eSkipObject) {
            DefaultSkip(in, object);
        }
        else {
            DefaultRead(in, object);
        }
    }
}



void CObjectsSniffer::OnObjectFoundPre(const CObjectInfo& /*object*/, 
                                       CNcbiStreampos /*stream_pos*/)
{
}

void CObjectsSniffer::OnObjectFoundPost(const CObjectInfo& /* object */)
{
}


void CObjectsSniffer::AddCandidate(CObjectTypeInfo ti, 
                                   EEventCallBackMode emode) 
{
    m_Candidates.push_back(SCandidateInfo(ti, emode));
}

void CObjectsSniffer::Probe(CObjectIStream& input)
{
    _ASSERT(m_Candidates.size());
    vector<CRef<COffsetReadHook> > hooks;  // list of all hooks we set

    //
    // create hooks for all candidates
    //

    TCandidates::const_iterator it;

    for (it = m_Candidates.begin(); it < m_Candidates.end(); ++it) {
        CRef<COffsetReadHook> h(new COffsetReadHook(this, it->event_mode));
        
        it->type_info.SetLocalReadHook(input, &(*h));
        hooks.push_back(h);

    } // for

    m_TopLevelMap.clear();


    if (input.GetDataFormat() == eSerial_AsnText
        || input.GetDataFormat() == eSerial_Xml) {
        ProbeText(input);
    } else {
        ProbeASN1_Bin(input);
    }

    //
    // Reset(clean) the hooks
    //

    _ASSERT(hooks.size() == m_Candidates.size());
    for (it = m_Candidates.begin(); it < m_Candidates.end(); ++it) {
        it->type_info.ResetLocalReadHook(input);
    } // for
}


void CObjectsSniffer::ProbeText(CObjectIStream& input)
{
    TCandidates::const_iterator last_cand = m_Candidates.end();

    string format_name;  // for LOG_POST messages
    if (input.GetDataFormat() == eSerial_AsnText) {
        format_name = "ASN.1 text";
    } else {
        format_name = "XML";
    }

    try {
        while (true) {
            m_StreamPos = input.GetStreamPos();
            string header = input.ReadFileHeader();

            if ( last_cand != m_Candidates.end() ) {
                // Check the previously found candidate first
                // (performance optimization)
                if (header == last_cand->type_info.GetTypeInfo()->GetName()) {
                    TCandidates::const_iterator it = last_cand;
                    CObjectInfo object_info(it->type_info.GetTypeInfo());
                    input.Read(object_info, CObjectIStream::eNoFileHeader);
                    m_TopLevelMap.push_back(
                        SObjectDescription(it->type_info, m_StreamPos));

                    _TRACE("Same type "
                           << format_name << " top level object found:" 
                           << it->type_info.GetTypeInfo()->GetName());
                    continue;
                }
            }

            bool found = false;
            // Scan through all candidates
            ITERATE ( TCandidates, it, m_Candidates ) {
                if ( header == it->type_info.GetTypeInfo()->GetName() ) {
                    CObjectInfo object_info(it->type_info.GetTypeInfo());
                    input.Read(object_info, CObjectIStream::eNoFileHeader);
                    found = true;
                    last_cand = it;
                    m_TopLevelMap.push_back(
                        SObjectDescription(it->type_info, m_StreamPos));

                    LOG_POST_X(2, Info 
                                << format_name << " top level object found:" 
                                << it->type_info.GetTypeInfo()->GetName());
                    break;
                }
            } // for
            if ( !found ) {
                input.SetStreamPos(m_StreamPos);
                return;
            }
        } // while
    }
    catch (CEofException& /*ignored*/) {
    }
    catch (exception& e) {
        LOG_POST_X(3, Info << "Exception reading "
                   << format_name << " " << e.what());
        input.SetStreamPos(m_StreamPos);
    }
}


void CObjectsSniffer::ProbeASN1_Bin(CObjectIStream& input)
{
    TCandidates::const_iterator last_cand = m_Candidates.end();
    for ( ;; ) {
        m_StreamPos = input.GetStreamPos();

        if ( last_cand != m_Candidates.end() ) {
            // Check the previously found candidate first
            // (performance optimization)
            try {
                TCandidates::const_iterator it = last_cand;
                CObjectInfo object_info(it->type_info.GetTypeInfo());
                input.Read(object_info);
                m_TopLevelMap.push_back(
                    SObjectDescription(it->type_info, m_StreamPos));
                _TRACE("Same type ASN.1 binary top level object found:" 
                       << it->type_info.GetTypeInfo()->GetName());
                continue;
            }
            catch ( CEofException& ) {
                // no more objects
                return;
            }
            catch ( exception& ) {
                Reset();
                input.SetStreamPos(m_StreamPos);
            }
        }

        bool found = false;
        // Scan through all candidates
        ITERATE ( TCandidates, it, m_Candidates ) {
            if ( it == last_cand ) {
                // already tried
                continue;
            }
            try {
                CObjectInfo object_info(it->type_info.GetTypeInfo());
                input.Read(object_info);
                found = true;
                last_cand = it;
                m_TopLevelMap.push_back(
                    SObjectDescription(it->type_info, m_StreamPos));
                LOG_POST_X(2, Info 
                           << "ASN.1 binary top level object found:" 
                           << it->type_info.GetTypeInfo()->GetName());
                break;
            }
            catch ( CEofException& ) {
                // no more objects
                return;
            }
            catch ( exception& ) {
                Reset();
                input.SetStreamPos(m_StreamPos);
            }
        }
        if ( !found ) {
            // no matching candidate
            break;
        }
    } // while
}


END_SCOPE(objects)
END_NCBI_SCOPE
