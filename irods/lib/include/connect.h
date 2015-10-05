/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
/**
 * @file connect.h
 * @addtogroup iRODS iRODS Utility Library
 * @brief Routines to handle connections to
 * iRODS systems.
 * @{
 */
#ifndef IRODS_CONNECT_H
#define IRODS_CONNECT_H

#include "rcConnect.h"
#include "jansson.h"
#include "irods_util_library.h"

#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Create a connection to a local iRODS server.
 *
 * @param credentials_json_p The JSON fragment containing the iRODS user details.
 * @return The connection to the iRODS server or <code>NULL</code> upon error.
 */
IRODS_UTIL_API rcComm_t *CreateIRODSConnectionFromJSON (const json_t *credentials_json_p);

/**
 * Create a connection to a local iRODS server.
 *
 * @param username_s The iRODS username.
 * @param password_s The iRODS password. FIXME! This needs to change to being encrypted.
 * @return The connection to the iRODS server or <code>NULL</code> upon error.
 */
IRODS_UTIL_API rcComm_t *CreateIRODSConnection (const char *username_s, const char *password_s);

/**
 * Close a connection to an iRODS server.
 *
 * @param connection_p The connection to close.
 */
IRODS_UTIL_API void CloseIRODSConnection (rcComm_t *connection_p);

/** @} */

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_CONNECT_H */
