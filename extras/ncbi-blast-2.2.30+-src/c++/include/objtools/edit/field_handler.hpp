/*  $Id: field_handler.hpp 447186 2014-09-23 14:45:51Z ivanov $
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
 *  and reliability of the software and data,  the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties,  express or implied,  including
 *  warranties of performance,  merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Authors:  Colleen Bollin
 */


#ifndef _FIELD_HANDLER_H_
#define _FIELD_HANDLER_H_

#include <corelib/ncbistd.hpp>

#include <objmgr/scope.hpp>

#include <objmgr/scope.hpp>
#include <objmgr/bioseq_handle.hpp>
#include <objects/seq/Seqdesc.hpp>
#include <objects/seqfeat/Seq_feat.hpp>

#include <objtools/edit/string_constraint.hpp>
#include <objtools/edit/apply_object.hpp>

BEGIN_NCBI_SCOPE

BEGIN_SCOPE(objects)
BEGIN_SCOPE(edit)


class NCBI_XOBJEDIT_EXPORT CFieldHandler : public CObject
{
public:
    virtual vector<CConstRef<CObject> > GetObjects(CBioseq_Handle bsh) = 0;
    virtual vector<CConstRef<CObject> > GetObjects(CSeq_entry_Handle seh, const string& constraint_field, CRef<CStringConstraint> string_constraint) = 0;
    virtual vector<CRef<CApplyObject> > GetApplyObjects(CBioseq_Handle bsh) = 0;
    virtual string GetVal(const CObject& object) = 0;
    virtual vector<string> GetVals(const CObject& object) = 0;
    virtual bool IsEmpty(const CObject& object) const = 0;
    virtual void ClearVal(CObject& object) = 0;
    virtual bool SetVal(CObject& object, const string& val, EExistingText existing_text) = 0;
    virtual string IsValid(const string& val) { return ""; };
    virtual vector<string> IsValid(const vector<string>& values) { vector<string> x; return x; };;
    virtual CSeqFeatData::ESubtype GetFeatureSubtype() = 0;
    virtual CSeqdesc::E_Choice GetDescriptorSubtype() = 0;
    virtual void SetConstraint(const string& field, CConstRef<CStringConstraint> string_constraint) = 0;
    virtual bool AllowMultipleValues() = 0;
    virtual vector<CConstRef<CObject> > GetRelatedObjects(const CObject& object, CRef<CScope> scope) = 0;
    virtual vector<CConstRef<CObject> > GetRelatedObjects(const CApplyObject& object) = 0;
    virtual vector<CRef<CApplyObject> > GetRelatedApplyObjects(const CObject& object, CRef<CScope> scope);
    static bool QualifierNamesAreEquivalent (string name1, string name2);
    static vector<CRef<CApplyObject> > GetApplyObjectsFromRelatedObjects(vector<CConstRef<CObject> >, CRef<CScope> scope);
};


class NCBI_XOBJEDIT_EXPORT CFieldHandlerFactory
{
public:
    static CRef<CFieldHandler> Create(const string &field_name);
    static bool s_IsSequenceIDField(const string& field);
};


NCBI_XOBJEDIT_EXPORT bool DoesObjectMatchFieldConstraint (const CObject& object, const string& field_name, CRef<CStringConstraint> string_constraint, CRef<CScope> scope);
NCBI_XOBJEDIT_EXPORT bool DoesApplyObjectMatchFieldConstraint (const CApplyObject& object, const string& field_name, CRef<CStringConstraint> string_constraint);


static const string kFieldTypeSeqId = "SeqId";
const string kDefinitionLineLabel = "Definition Line";
const string kCommentDescriptorLabel = "Comment Descriptor";
const string kGenbankBlockKeyword = "Keyword";

END_SCOPE(edit)
END_SCOPE(objects)
END_NCBI_SCOPE

#endif

