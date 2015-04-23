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

WHEATIS_NETWORK_API int SendJsonRawRequest (const json_t *json_p, RawConnection *connection_p);

WHEATIS_NETWORK_API json_t *GetLoginJsonRequest (const char * const username_s, const char *password_s);

WHEATIS_NETWORK_API json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s);

WHEATIS_NETWORK_API json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s);

WHEATIS_NETWORK_API bool GetUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss);

WHEATIS_NETWORK_API json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, const char * const protocol_s, const char * const filename_s);

WHEATIS_NETWORK_API json_t *GetKeywordServicesRequest (const char * const username_s, const char * const password_s, const char * const keyword_s);

WHEATIS_NETWORK_API json_t *GetNamedServicesRequest (const char * const username_s, const char * const password_s, const char * const service_name_s);

WHEATIS_NETWORK_API json_t *GetCheckServicesRequest (const char * const username_s, const char * const password_s, const char * const service_uuid_s);

WHEATIS_NETWORK_API bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s);

WHEATIS_NETWORK_API json_t *GetOperationAsJSON (Operation op);

WHEATIS_NETWORK_API json_t *GetServicesRequest (const char * const username_s, const char * const password_s, const Operation op, const char * const op_key_s, json_t * const op_data_p);

WHEATIS_NETWORK_API json_t *MakeRemoteJsonCall (json_t *req_p, Connection *connection_p);

WHEATIS_NETWORK_API const char *GetUserUUIDStringFromJSON (const json_t *credentials_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_TOOLS_H */
