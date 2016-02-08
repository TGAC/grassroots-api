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
#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H

#include "network_library.h"
#include "jansson.h"
#include "typedefs.h"
#include "operation.h"
#include "request_tools.h"
#include "uuid/uuid.h"

#define JSON_KEY_USERNAME ("username")
#define JSON_KEY_PASSWORD ("password")


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * @brief Generate a request to get a list of of iRODS objects modified within a given period.
 *
 * This will generate the required json_t request to send from a Client to a Server to get a list of
 * modified iRODS data objects.
 * @param username_s The encrypted iRODS username.
 * @param password_s The encrypted iRODS password.
 * @param from_s The start of the interval in the form YYYYMMDDhhmmss.
 * @param to_s The start of the interval in the form YYYYMMDDhhmmss.
 * @return The json_t object for this request or <code>NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);


/**
 * Generate the JSON fragment to send to the Server for getting all available Services.
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s);


/**
 * Extract the user credentials from a JSON fragment.
 *
 * @param root_p The JSON fragment to inspect.
 * @param username_ss Pointer to the username to update.
 * @param password_ss Pointer to the encrypted password to update.
 * @return <code>true</code> if the credentials were extracted from
 * the JSON fragment successfully,
 * <code>false</code> otherwise.
 */
GRASSROOTS_NETWORK_API bool GetUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss);


/**
 * Generate the JSON fragment to send to the Server for getting Services that are viable for a given Resource
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @param protocol_s The Resource protocol.
 * @param filename_s The filename of the Resource.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, const char * const protocol_s, const char * const filename_s);


/**
 * Generate the JSON fragment to send to the Server for running all keyword-aware Services for a given keyword.
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @param keyword_s The keyword to use.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetKeywordServicesRequest (const char * const username_s, const char * const password_s, const char * const keyword_s);


/**
 * Generate the JSON fragment to send to the Server for getting the details of a particular Service.
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @param service_name_s The name of the Service.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetNamedServicesRequest (const char * const username_s, const char * const password_s, const char * const service_name_s);


/**
 * Generate the JSON fragment to send to the Server for checking the status of an Operation.
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @param service_uuid_s The uuid for the requested Operation.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetCheckServicesRequest (const char * const username_s, const char * const password_s, const char * const service_uuid_s);


/**
 * Add the given credentials in the correct position in a JSON fragment.
 *
 * @param root_p The JSON fragment to update.
 * @param username_s The username to add.
 * @param password_s The encrypted password to add.
 * @return <code>true</code> if the JSON fragment was updated successfully,
 * <code>false</code> otherwise.
 */
GRASSROOTS_NETWORK_API bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s);


/**
 * Get the JSON fragment for a given Operation.
 *
 * @param op The Operation to generate the JSON for.
 * @return The JSON fragment or <code>NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetOperationAsJSON (Operation op);


/**
 * @brief Generate the request to run an Operation.
 *
 * @param username_s An optional username (encrypted).
 * @param password_s An optional password (encrypted).
 * @param op The Operation to perform.
 * @param op_key_s The key to be used to attach the associated Operation-specific data with.
 * @see KEY_IRODS
 * @see	KEY_FILE_DATA
 * @see	KEY_QUERY
 * @see KEY_FILENAME
 * @see KEY_DIRNAME
 * @see KEY_PROTOCOL
 * @see KEY_INTERVAL
 * @param op_data_p The Operation-specific data.
 * @return The json_t for the request or <code>NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetServicesRequest (const char * const username_s, const char * const password_s, const Operation op, const char * const op_key_s, json_t * const op_data_p);

/**
 * Send a JSON-based request to the server and get the JSON-based response.
 *
 * @param req_p The JSON request to send to the Server.
 * @param connection_p The Connection to the Server.
 * @return The JSON response or <code>NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *MakeRemoteJsonCall (json_t *req_p, Connection *connection_p);


/**
 * Delete a json object and all of its references to its children
 *
 * @param json_p The json object to remove.
 */
GRASSROOTS_NETWORK_API void WipeJSON (json_t *json_p);


/**
 * Get the User uuid from a JSON fragment.
 *
 * @param credentials_p The JSON fragment to parse.
 * @return The user's uuid or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_NETWORK_API const char *GetUserUUIDStringFromJSON (const json_t *credentials_p);


/**
 * Generate the JSON fragment to send to the Server for checking the status of one or more Operations.
 *
 * @param ids_pp An array of uuid_ts, one for each operation .
 * @param num_ids The number of ids in ids_pp.
 * @param connection_p The Connection to the Server.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetServicesStatusRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p);


/**
 * Generate the JSON fragment to send to the Server for getting the results of one or more Operations.
 *
 * @param ids_pp An array of uuid_ts, one for each operation .
 * @param num_ids The number of ids in ids_pp.
 * @param connection_p The Connection to the Server.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API json_t *GetServicesResultsRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p);


/**
 * Get the current OperationStatus for a Service from its JSON fragment.
 *
 * @param service_json_p The JSON fragment of the response from the Server.
 * @param status_p Pointer to the OperationStatus value that will be updated.
 * @return <code>true</code> if the OperationStatus value was updated successfully, <code>
 * false</code> otherwise.
 */
GRASSROOTS_NETWORK_API bool GetStatusFromJSON (const json_t *service_json_p, OperationStatus *status_p);




/**
 * Make a call from a Client to a Server.
 *
 * @param client_results_p The request data to send from the Client to the Server.
 * @param username_s An optional username, this can be <code>NULL</code>.
 * @param password_s An optional encrypted password, this can be <code>NULL</code>.
 * @param connection_p The connection to the Server.
 * @return The response of the Server to the Client or <code>NULL</code> upon error.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API json_t *CallServices (json_t *client_results_p, const char * const username_s, const char * const password_s, Connection *connection_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
