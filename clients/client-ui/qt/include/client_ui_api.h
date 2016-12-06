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
#ifndef CLIENT_UI_API_H
#define CLIENT_UI_API_H

//#include "grassroots_ui.h"
#include "client.h"


#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_CLIENT_API Client *GetClient (Connection *connection_p);

GRASSROOTS_CLIENT_API bool ReleaseClient (Client *client_p);


//json_t *GetUserParameters (json_t *service_p, const char * const filename_s);

#ifdef __cplusplus
}
#endif

#endif // CLIENT_UI_API_H
