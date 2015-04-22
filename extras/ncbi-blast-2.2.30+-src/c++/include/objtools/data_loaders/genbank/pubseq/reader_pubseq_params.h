#ifndef GBLOADER_PUBSEQ_PARAMS__HPP_INCLUDED
#define GBLOADER_PUBSEQ_PARAMS__HPP_INCLUDED

/*  $Id: reader_pubseq_params.h 194540 2010-06-15 16:21:12Z vasilche $
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
*  ===========================================================================
*
*  Author: Eugene Vasilchenko
*
*  File Description:
*    GenBank reader PubSeq configuration parameters
*
* ===========================================================================
*/

/* Name of PUBSEQ reader driver */
#define NCBI_GBLOADER_READER_PUBSEQ_DRIVER_NAME "pubseqos"

/* PUBSEQ_OS server name */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_SERVER   "server"
/* PUBSEQ_OS login name */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_USER     "user"
/* PUBSEQ_OS login password */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_PASSWORD "password"
/* DBAPI driver name */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_DRIVER   "driver"
/* allow gzip compression */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_GZIP     "gzip"
/* exclude WGS master descriptors */
#define NCBI_GBLOADER_READER_PUBSEQ_PARAM_EXCL_WGS_MASTER "exclude_wgs_master"

#endif
