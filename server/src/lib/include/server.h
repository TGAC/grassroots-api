/*
** Copyright 2014-2016 The Earlham Institute
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
 * @brief
 */
#ifndef GRASSROOTS_SERVICE_MANAGER_H
#define GRASSROOTS_SERVICE_MANAGER_H

#include "typedefs.h"
#include "grassroots_service_manager_library.h"
#include "jansson.h"
#include "operation.h"


/**
 * This is the default port to run the Grassroots server on when it
 * is running as a standalone server rather than as a module within
 * Apache which is the recommended mode of operation.
 */
#define DEFAULT_SERVER_PORT	("9991")

#define KEY_IRODS				("irods")

#define KEY_INTERVAL	("interval")

/*
typedef json_t (*server_callback_fn) (json_t *req_p, json_t *credentials_p);

typedef struct ServerOperations
{
	server_callback_fn [OP_NUM_OPERATIONS];
} ServerOperations;
*/
 
#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Construct a response message based upon a client's message.
 *
 * This will attempt to create a JSON object from the incoming message
 * and call ProcessServerJSONMessage.
 *
 * @param request_s The message from the client.
 * @param socket_fd The socket file descriptor used.
 * @return The response from the server or <code>NULL</code> upon error.
 * @see ProcessServerJSONMessage
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *ProcessServerRawMessage (const char * const request_s, const int socket_fd);


/**
 * Process a given JSON request and produce the server response.
 *
 * @param req_p The incoming JSON request.
 * @param socket_fd The socket file descriptor used.
 * @param error_s A pointer to a variable where any error messages can be stored.
 * @return
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *ProcessServerJSONMessage (json_t *req_p, const int socket_fd, const char **error_s);


/**
 * Create a response object with a valid header and a given key and value.
 *
 * @param req_p If this object is not <code>NULL</code> and contains a "verbose" key set to true,
 * then the request will be added to a "request" key within the "header" section of this response.
 * This is to allow the tracking of requests to responses if needed.
 * @param key_s The key to use to add the associated value to the generated response.
 * @param value_p The value to add to the generated response.
 * @return The response or <code>NULL</code> upon error.
 * @see GetInitialisedMessage
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *GetInitialisedResponseOnServer (const json_t *req_p, const char *key_s, json_t *value_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_SERVICE_MANAGER_H */
