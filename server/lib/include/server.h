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
#ifndef GRASSROOTS_SERVICE_MANAGER_H
#define GRASSROOTS_SERVICE_MANAGER_H

#include "typedefs.h"
#include "grassroots_service_manager_library.h"
#include "jansson.h"
#include "operation.h"
#include "service.h"

#define DEFAULT_SERVER_PORT	("9991")

#define KEY_IRODS				("irods")
#define	KEY_FILE_DATA		("file_data")

#define KEY_FILENAME	("file")
#define KEY_DIRNAME		("dir")
#define KEY_PROTOCOL	("protocol")
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
 * @param request_s The message from the client.
 * @return The response from the server.
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *ProcessServerRawMessage (const char * const request_s, const int socket_fd);


GRASSROOTS_SERVICE_MANAGER_API json_t *ProcessServerJSONMessage (json_t *req_p, const int socket_fd, const char **error_s);


/**
 * Get a Service by its name.
 *
 * This will match a Service with its reponse from getting GetServiceName.
 *
 * @param service_name_s The name of the Service to find.
 * @return The matching Service or <code>NULL</code> if it could not be found.
 * @memberof Service
 */
GRASSROOTS_SERVICE_MANAGER_API Service *GetServiceByName (const char * const service_name_s);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_SERVICE_MANAGER_H */
