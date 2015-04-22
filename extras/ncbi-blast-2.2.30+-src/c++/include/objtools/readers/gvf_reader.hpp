 /*  $Id: gvf_reader.hpp 411995 2013-09-04 12:36:33Z ludwigf $
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
 * Author: Frank Ludwig
 *
 * File Description:
 *   BED file reader
 *
 */

#ifndef OBJTOOLS_READERS___GVF_READER__HPP
#define OBJTOOLS_READERS___GVF_READER__HPP

#include <corelib/ncbistd.hpp>
#include <objects/seq/Annotdesc.hpp>
#include <objects/seqfeat/Seq_feat.hpp>
#include <objects/seqfeat/Cdregion.hpp>

#include <objtools/readers/reader_base.hpp>
#include <objtools/readers/gff2_reader.hpp>
#include <objtools/readers/gff3_reader.hpp>

BEGIN_NCBI_SCOPE

BEGIN_SCOPE(objects) // namespace ncbi::objects::

class CGFFReader;
class CGff3ReadRecord;
class SRecord;

//  ============================================================================
class CGvfReadRecord
//  ============================================================================
    : public CGff3ReadRecord
{
public:
    CGvfReadRecord(
        unsigned int lineNumber,
        IMessageListener* pEC = 0): 
        mLineNumber(lineNumber),
        mpMessageListener(pEC)
    {};
    ~CGvfReadRecord() {};

    virtual bool AssignFromGff(
        const string& );

    bool SanityCheck() const;

protected:
    virtual bool x_AssignAttributesFromGff(
        const string& );
    void xTraceError(
        EDiagSev,
        const string&);

    unsigned int mLineNumber;
    IMessageListener* mpMessageListener;
};

//  ----------------------------------------------------------------------------
class NCBI_XOBJREAD_EXPORT CGvfReader
//  ----------------------------------------------------------------------------
    : public CGff3Reader
{
public:
    CGvfReader(
        unsigned int uFlags,
        const string& name = "",
        const string& title = "" );

    virtual ~CGvfReader();

protected:
    virtual bool x_ParseStructuredCommentGff(
        const string&,
        CRef< CAnnotdesc >& );
                                
    virtual bool x_ParseFeatureGff(
        const string&,
        TAnnots&,
        IMessageListener*);

    CRef<CSeq_annot> x_GetAnnotById(
        TAnnots& annots,
        const string& strId );

    virtual bool x_MergeRecord(
        const CGvfReadRecord&,
        CRef<CSeq_annot>,
        IMessageListener*);

    bool x_FeatureSetLocation(
        const CGff2Record&,
        CRef< CSeq_feat > );
    
    bool xFeatureSetLocationInterval(
        const CGff2Record&,
        CRef< CSeq_feat > );
    
    bool xFeatureSetLocationPoint(
        const CGff2Record&,
        CRef< CSeq_feat > );
    
    bool x_FeatureSetVariation(
        const CGvfReadRecord&,
        CRef< CSeq_feat > );

    virtual bool x_FeatureSetExt(
        const CGvfReadRecord&,
        CRef<CSeq_feat>,
        IMessageListener*);

    bool xVariationMakeSNV(
        const CGvfReadRecord&,
         CRef<CVariation_ref> );

    bool xVariationMakeCNV(
        const CGvfReadRecord&,
        CRef<CVariation_ref> );

    bool xVariationMakeInsertions(
        const CGvfReadRecord&,
        CRef<CVariation_ref> );

    bool xVariationMakeDeletions(
        const CGvfReadRecord&,
        CRef<CVariation_ref> );

    bool xVariationSetInsertions(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    bool xVariationSetDeletions(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual bool xVariationSetId(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual bool xVariationSetParent(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual bool xVariationSetName(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual bool xVariationSetSnvs(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual bool xVariationSetProperties(
        const CGvfReadRecord&,
        CRef< CVariation_ref > );

    virtual CGff2Record* x_CreateRecord() { return new CGvfReadRecord(m_uLineNumber); };   

protected:
    CRef< CAnnotdesc > m_Pragmas;
 
};

END_SCOPE(objects)
END_NCBI_SCOPE

#endif // OBJTOOLS_READERS___GVF_READER__HPP
