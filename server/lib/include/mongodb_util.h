/*
 * mongo_db_util.h
 *
 *  Created on: 20 Jul 2015
 *      Author: billy
 */

#ifndef MONGO_DB_UTIL_H_
#define MONGO_DB_UTIL_H_

#include "mongoc.h"

#include "wheatis_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

WHEATIS_SERVICE_MANAGER_API bool InitMongoDB (void);

WHEATIS_SERVICE_MANAGER_API void ExitMongoDB (void);

WHEATIS_SERVICE_MANAGER_API mongoc_client_t *GetMongoClient (void);

WHEATIS_SERVICE_MANAGER_API void ReleaseMongoClient (mongoc_client_t *client_p);


#ifdef __cplusplus
}
#endif


#endif /* MONGO_DB_UTIL_H_ */
