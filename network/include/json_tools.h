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
 *
 */
WHEATIS_NETWORK_API int SendJsonRawRequest (const json_t *json_p, RawConnection *connection_p);

/**
 *
 */
WHEATIS_NETWORK_API json_t *GetLoginJsonRequest (const char * const username_s, const char *password_s);

WHEATIS_NETWORK_API json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);

WHEATIS_NETWORK_API json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s);


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
WHEATIS_NETWORK_API bool GetUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss);

WHEATIS_NETWORK_API json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, const char * const protocol_s, const char * const filename_s);

WHEATIS_NETWORK_API json_t *GetKeywordServicesRequest (const char * const username_s, const char * const password_s, const char * const keyword_s);

WHEATIS_NETWORK_API json_t *GetNamedServicesRequest (const char * const username_s, const char * const password_s, const char * const service_name_s);


/**
 * Generate the JSON fragment to send to the Server for checking the status of an Operation.
 *
 * @param username_s An optional username.
 * @param password_s An optional encrypted password.
 * @param service_uuid_s The uuid for the requested Operation.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
WHEATIS_NETWORK_API json_t *GetCheckServicesRequest (const char * const username_s, const char * const password_s, const char * const service_uuid_s);


/**
 * Add the given credentials in the correct position in a JSON fragment.
 *
 * @param root_p The JSON fragment to update.
 * @param username_s The username to add.
 * @param password_s The encrypted password to add.
 * @return <code>true</code> if the JSON fragment was updated successfully,
 * <code>false</code> otherwise.
 */
WHEATIS_NETWORK_API bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s);


/**
 * Get the JSON fragment for a given Operation.
 *
 * @param op The Operation to generate the JSON for.
 * @return The JSON fragment or <code>NULL</code> upon error.
 */
WHEATIS_NETWORK_API json_t *GetOperationAsJSON (Operation op);

WHEATIS_NETWORK_API json_t *GetServicesRequest (const char * const username_s, const char * const password_s, const Operation op, const char * const op_key_s, json_t * const op_data_p);

/**
 * Send a JSON-based request to the server and get the JSON-based response.
 *
 * @param req_p The JSON request to send to the Server.
 * @param connection_p The Connection to the Server.
 * @return The JSON response or <code>NULL</code> upon error.
 */
WHEATIS_NETWORK_API json_t *MakeRemoteJsonCall (json_t *req_p, Connection *connection_p);


/**
 * Delete a json object and all of its references to its children
 *
 * @param json_ The json object to remove.
 */
WHEATIS_NETWORK_API void WipeJSON (json_t *json_p);

WHEATIS_NETWORK_API const char *GetUserUUIDStringFromJSON (const json_t *credentials_p);


/**
 * Generate the JSON fragment to send to the Server for checking the status of one or more Operations.
 *
 * @param ids_pp An array of uuid_ts, one for each operation .
 * @param num_ids The number of ids in ids_pp.
 * @param connection_p The Connection to the Server.
 * @return The JSON fragment to send to the Server or <code>
 * NULL</code> upon error.
 */
WHEATIS_NETWORK_API json_t *GetServicesStatusRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p);

WHEATIS_NETWORK_API json_t *GetServicesResultsRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p);


/**
 * Get the current OperationStatus for a Service from its JSON fragment.
 *
 * @param service_json_p The JSON fragment of the response from the Server.
 * @param status_p Pointer to the OperationStatus value that will be updated.
 * @return <code>true</code> if the OperationStatus value was updated successfully, <code>
 * false</code> otherwise.
 */
WHEATIS_NETWORK_API bool GetStatusFromJSON (const json_t *service_json_p, OperationStatus *status_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
