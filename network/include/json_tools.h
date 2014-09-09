#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H

#include "network_library.h"
#include "jansson.h"
#include "typedefs.h"

#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_NETWORK_API int SendJsonRequest (int socket_fd, uint32 id, json_t *json_p);

WHEATIS_NETWORK_API json_t *GetLoginJson (const char * const username_s, const char *password_s);

WHEATIS_NETWORK_API json_t *GetModifiedFiles (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);

WHEATIS_NETWORK_API json_t *GetAvailableServices (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
