#ifndef CLIENT_UI_API_H
#define CLIENT_UI_API_H

//#include "grassroots_ui.h"
#include "client.h"


#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_CLIENT_API Client *GetClient (Connection *connection_p);

GRASSROOTS_CLIENT_API void ReleaseClient (Client *client_p);


//json_t *GetUserParameters (json_t *service_p, const char * const filename_s);

#ifdef __cplusplus
}
#endif

#endif // CLIENT_UI_API_H
