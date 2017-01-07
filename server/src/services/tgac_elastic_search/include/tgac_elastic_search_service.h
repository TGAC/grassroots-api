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
#ifndef TGAC_ELASTIC_SEARCH_REST_SERVICE_H
#define TGAC_ELASTIC_SEARCH_REST_SERVICE_H

#include "service.h"
#include "library.h"
#include "tgac_elastic_search_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the Services available for accessing the ElasticSearch Service(s).
 *
 * @param config_p The service configuration data.
 * @return The ServicesArray containing the ElasticSearch Service(s) or
 * <code>NULL</code> upon error.
 */
TGAC_ELASTIC_SEARCH_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


/**
 * Free the ServicesArray and its associated ElasticSearch Service(s).
 *
 * @param services_p The ServicesArray to free.
 */
TGAC_ELASTIC_SEARCH_SERVICE_API void ReleaseServices (ServicesArray *services_p);



#ifdef __cplusplus
}
#endif



#endif		/* #ifndef TGAC_ELASTIC_SEARCH_REST_SERVICE_H */
