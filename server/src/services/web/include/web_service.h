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
 */
#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include "service.h"
#include "library.h"
#include "web_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the ServicesArray containing the Web Services.
 *
 * @param config_p The service configuration data.
 * @return The ServicesArray containing all of the Web Services or
 * <code>NULL</code> upon error.
 * @ingroup web_service
 */
WEB_SERVICE_API ServicesArray *GetServices (json_t *config_p);


/**
 * Free the ServicesArray containing the Web Services.
 *
 * @param services_p The ServicesArray to free.
 * @ingroup web_service
 */
WEB_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WEB_SERVICE_H */
