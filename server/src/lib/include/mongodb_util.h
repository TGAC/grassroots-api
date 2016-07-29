/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * mongo_db_util.h
 *
 *  Created on: 20 Jul 2015
 *      Author: billy
 */

#ifndef MONGO_DB_UTIL_H_
#define MONGO_DB_UTIL_H_

#include "mongoc.h"

#include "grassroots_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

GRASSROOTS_SERVICE_MANAGER_API bool InitMongoDB (void);

GRASSROOTS_SERVICE_MANAGER_API void ExitMongoDB (void);

GRASSROOTS_SERVICE_MANAGER_API mongoc_client_t *GetMongoClient (void);

GRASSROOTS_SERVICE_MANAGER_API void ReleaseMongoClient (mongoc_client_t *client_p);


#ifdef __cplusplus
}
#endif


#endif /* MONGO_DB_UTIL_H_ */