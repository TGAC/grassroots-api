#ifndef CONNECT_SERVICES__NETSTORAGE_OBJECT_INFO__HPP
#define CONNECT_SERVICES__NETSTORAGE_OBJECT_INFO__HPP

/*  $Id: netstorageobjectinfo.hpp 444309 2014-08-22 18:51:53Z kazimird $
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
 * Author:  Dmitry Kazimirov
 *
 * File Description:
 *   Declarations for the CNetStorageObjectInfo implementation.
 *
 */

#include <connect/services/netstorage.hpp>


BEGIN_NCBI_SCOPE

struct SNetStorageObjectInfoImpl : public CObject
{
    SNetStorageObjectInfoImpl(const string& object_loc,
            ENetStorageObjectLocation location,
            CJsonNode::TInstance object_loc_info,
            Uint8 file_size,
            CJsonNode::TInstance storage_specific_info);

    string m_ObjectLoc;
    ENetStorageObjectLocation m_Location;
    CJsonNode m_ObjectLocInfo;
    Uint8 m_FileSize;
    CJsonNode m_StorageSpecificInfo;
};

END_NCBI_SCOPE

#endif  /* CONNECT_SERVICES__NETSTORAGE_OBJECT_INFO__HPP */
