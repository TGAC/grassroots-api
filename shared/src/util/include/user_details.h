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
 * @file
 * @addtogroup User User authentication and authorization
 * @{
 */
#ifndef USER_DETAILS_H
#define USER_DETAILS_H

#include "jansson.h"

#include "typedefs.h"
#include "grassroots_util_library.h"
#include "linked_list.h"


/**
 * @brief  A datatype to store user credentials for a single system.
 */
typedef struct UserAuthentication
{
	/**
	 * An identifier for the system that this
	 * UserAuthentication is for.
	 */
	char *ua_system_id_s;

	/** The username */
	char *ua_username_s;

	/** The encrypted password */
	char *ua_password_s;

	/** An OAth token for time-based authentication */
	char *ua_oath_totp_token_s;
} UserAuthentication;


/**
 * @brief  A datatype to store UserAuthentications on a LinkedList
 * @extends ListItem
 */
typedef struct UserAuthenticationNode
{
	/** The base ListItem to allow the storage of UserAuthenticationNodes on a LinkedList. */
	ListItem uan_base_node;

	/** A pointer to the UserAuthentication details. */
	UserAuthentication *uan_auth_p;

} UserAuthenticationNode;


/**
 * @brief  A datatype to store user credentials
 */
typedef struct UserDetails
{
	/** A LinkedList of UserAuthenticationNodes */
	LinkedList *ud_auth_list_p;
} UserDetails;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a UserDetails with the given username.
 *
 * @param credentials_p The credentials to get the details of a user's
 * authorisations for various systems. This can be NULL.
 * @return A newly-allocated UserDetails or <code>NULL</code> upon error.
 * @memberof UserDetails
 */
GRASSROOTS_UTIL_API UserDetails *AllocateUserDetails (const json_t *credentials_p);


/**
 * Free a UserDetails.
 *
 * @param user_details_p The UserDetails to free.
 * @memberof UserDetails
 */
GRASSROOTS_UTIL_API void FreeUserDetails (UserDetails *user_details_p);


/**
 * Get the UserAuthentication details for a given system.
 *
 * @param user_p The UserDetails to get the UserAuthentication from.
 * @param system_s The name of the system to add the credentials for e.g. "irods", "kerberos", etc.
 * @return A pointer to the relevant UserAuthentication or <code>NULL</code> upon error.
 * @memberof UserDetails
 */
GRASSROOTS_UTIL_API const UserAuthentication *GetUserAuthenticationForSystem (const UserDetails *user_p, const char *system_s);


/**
 * This will create and add the UserAuthentication details for a given system.
 *
 * @param user_details_p The UserDetails to amend.
 * @param system_s The name of the system to add the credentials for e.g. "irods", "kerberos", etc.
 * @param username_s The username for the given system.
 * @param password_s The password for the given system. This should only be used if the token option is unuable for a given system.
 * @param token_s The token for the given system.
 * @return <code>true</code> if the authentication details were added to the UserDetails successfully, <code>false</code> otherwise.
 * @memberof UserDetails
 */
GRASSROOTS_UTIL_API bool AddUserAuthentication (UserDetails *user_details_p, const char *system_s, const char *username_s, const char *password_s, const char *token_s);

/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef USER_DETAILS_H */
