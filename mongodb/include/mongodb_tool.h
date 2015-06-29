/*
 * mongodb_tool.h
 *
 *  Created on: 26 Jun 2015
 *      Author: billy
 */

#ifndef MONGODB_TOOL_H_
#define MONGODB_TOOL_H_

#include "mongoc.h"
#include "typedefs.h"
#include "jansson.h"
#include "mongodb_library.h"

typedef struct MongoTool
{
	/*
	 * mongoc_client_t is NOT thread-safe and should
	 * only be used from one thread at a time. When used in
	 * multi-threaded scenarios, it is recommended that you
	 * use the thread-safe mongoc_client_pool_t to retrieve
	 * a mongoc_client_t for your thread.
	 */
	mongoc_client_t *mt_client_p;

	mongoc_collection_t *mt_collection_p;

	mongoc_cursor_t *mt_cursor_p;
} MongoTool;


/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_RESOURCE_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * resource.c.
 */
#ifdef ALLOCATE_MONGODB_TAGS
	#define MONGODB_PREFIX WHEATIS_MONGODB_API
	#define MONGODB_VAL(x)	= x
#else
	#define MONGODB_PREFIX extern
	#define MONGODB_VAL(x)
#endif


MONGODB_PREFIX const char *MONGO_ID_S MONGODB_VAL("id");



#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_MONGODB_API bool InitMongo (const char *connection_s);

WHEATIS_MONGODB_API void ExitMongo (void);


WHEATIS_MONGODB_API bool GetMongoCollection (MongoTool *tool_p, const char *db_s, const char *collection_s);


WHEATIS_MONGODB_API MongoTool *AllocateMongoTool (void);

WHEATIS_MONGODB_API void FreeMongoTool (MongoTool *tool_p);

WHEATIS_MONGODB_API bson_oid_t *InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p);



#ifdef __cplusplus
}
#endif



#endif /* MONGODB_TOOL_H_ */
