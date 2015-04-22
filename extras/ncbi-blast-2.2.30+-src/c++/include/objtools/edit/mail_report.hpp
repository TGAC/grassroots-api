/*  $Id: mail_report.hpp 435187 2014-05-14 14:28:00Z bollin $
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


#ifndef _MAIL_REPORT_H_
#define _MAIL_REPORT_H_

#include <corelib/ncbistd.hpp>
#include <objects/seqtable/Seq_table.hpp>
#include <objmgr/scope.hpp>

BEGIN_NCBI_SCOPE

BEGIN_SCOPE(objects)
BEGIN_SCOPE(edit)

NCBI_XOBJEDIT_EXPORT CRef<CSeq_table> MakeMailReportPreReport(CSeq_entry_Handle seh);
NCBI_XOBJEDIT_EXPORT void MakeMailReportPostReport(CSeq_table& table, CScope& scope);
NCBI_XOBJEDIT_EXPORT string GetReportFromMailReportTable(const CSeq_table& table);


END_SCOPE(edit)
END_SCOPE(objects)
END_NCBI_SCOPE

#endif

