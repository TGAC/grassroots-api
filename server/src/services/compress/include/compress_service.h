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
#ifndef COMPRESS_SERVICE_H
#define COMPRESS_SERVICE_H

#include "service.h"
#include "library.h"
#include "compress_service_library.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the ServicesArray containing the Compress Service.
 *
 * @param config_p The service configuration data.
 * @return The ServicesArray containing the Compress Service or
 * <code>NULL</code> upon error.
 */
COMPRESS_SERVICE_API ServicesArray *GetServices (UserDetails *user_p);

/**
 * Free the ServicesArray and its associated Compress Service.
 *
 * @param services_p The ServicesArray to free.
 */
COMPRESS_SERVICE_API void ReleaseServices (ServicesArray *services_p);



#ifdef __cplusplus
}
#endif



#endif		/* #ifndef COMPRESS_SERVICE_H */
