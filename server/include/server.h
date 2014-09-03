#ifndef WHEATIS_SERVICE_MANAGER_H
#ifndef WHEATIS_SERVICE_MANAGER_H


#include "wheatis_service_manager_api.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Construct a response message based upon a client's message.
 *
 * @param request_s The message from the client.
 * @return The response from the server.
 */
WHEATIS_SERVICE_MANAGER_API char *ProcessMessage (const char * const request_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_SERVICE_MANAGER_H */
