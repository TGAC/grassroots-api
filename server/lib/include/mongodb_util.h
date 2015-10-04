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
