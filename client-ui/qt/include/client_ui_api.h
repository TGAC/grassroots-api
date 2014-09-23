#ifndef CLIENT_UI_API_H
#define CLIENT_UI_API_H

#include "wheatis_ui.h"

#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_CLIENT_API Client *AllocateClient (void);

WHEATIS_CLIENT_API void *FreeClient (Client *client_p);

//json_t *GetUserParameters (json_t *service_p, const char * const filename_s);

#ifdef __cplusplus
}
#endif

#endif // CLIENT_UI_API_H
