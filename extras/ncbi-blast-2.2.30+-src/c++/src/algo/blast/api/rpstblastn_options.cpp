/*  $Id: rpstblastn_options.cpp 440360 2014-07-11 17:28:33Z madden $
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
 * Authors:  Jason Papadopoulos
 *
 */

/// @file rpstblastn_options.cpp
/// Implements the CRPSTBlastnOptionsHandle class.

#include <ncbi_pch.hpp>
#include <algo/blast/api/rpstblastn_options.hpp>
#include "blast_setup.hpp"

/** @addtogroup AlgoBlast
 *
 * @{
 */


BEGIN_NCBI_SCOPE
BEGIN_SCOPE(blast)

CRPSTBlastnOptionsHandle::CRPSTBlastnOptionsHandle(EAPILocality locality)
    : CBlastRPSOptionsHandle(locality)
{
    SetDefaults();
    m_Opts->SetProgram(eRPSTblastn);
}

void
CRPSTBlastnOptionsHandle::SetGappedExtensionDefaults()
{
    CBlastRPSOptionsHandle::SetGappedExtensionDefaults();
    SetCompositionBasedStats(false);
}


void
CRPSTBlastnOptionsHandle::SetQueryOptionDefaults()
{
    SetSegFiltering(true);
    m_Opts->SetQueryGeneticCode(BLAST_GENETIC_CODE);
}

END_SCOPE(blast)
END_NCBI_SCOPE


/* @} */
