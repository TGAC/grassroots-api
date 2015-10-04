#ifndef GRASSROOTS_SERVICE_MANAGER_H
#define GRASSROOTS_SERVICE_MANAGER_H

#include "typedefs.h"
#include "grassroots_service_manager_library.h"
#include "jansson.h"
#include "operation.h"

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


GRASSROOTS_SERVICE_MANAGER_API json_t *ProcessServerJSONMessage (json_t *req_p, const int socket_fd);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_SERVICE_MANAGER_H */
