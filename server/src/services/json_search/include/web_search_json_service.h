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
#ifndef WEB_SEARH_JSON_SERVICE_H
#define WEB_SEARH_JSON_SERVICE_H

#include "service.h"
#include "library.h"
#include "web_search_json_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


WEB_SEARCH_JSON_SERVICE_API ServicesArray *GetServices (UserDetails *user_p, json_t *config_p);


WEB_SEARCH_JSON_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WEB_SEARH_JSON_SERVICE_H */
