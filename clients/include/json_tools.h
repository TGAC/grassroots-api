#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H

#include "jansson.h"


#ifdef __cplusplus
extern "C" 
{
#endif

int SendJsonRequest (int socket_fd, json_t *json_p);

json_t *GetLoginJson (const char * const username_s, const char *password_s);

json_t *GetModifiedFiles (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);

json_t *GetAvailableServices (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
