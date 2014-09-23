#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H

#include "network_library.h"
#include "jansson.h"
#include "typedefs.h"


#define JSON_KEY_USERNAME ("username")
#define JSON_KEY_PASSWORD ("password")


#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_NETWORK_API int SendJsonRequest (int socket_fd, uint32 id, const json_t *json_p);

WHEATIS_NETWORK_API json_t *GetLoginJsonRequest (const char * const username_s, const char *password_s);

WHEATIS_NETWORK_API json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);

WHEATIS_NETWORK_API json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s);

WHEATIS_NETWORK_API bool GetIrodsUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss);

WHEATIS_NETWORK_API json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, json_t * const file_data_p);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
