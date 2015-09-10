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


MONGODB_PREFIX const char *MONGO_ID_S MONGODB_VAL("_id");
MONGODB_PREFIX const char *MONGO_COLLECTION_S MONGODB_VAL("collection");
MONGODB_PREFIX const char *MONGO_OPERATION_S MONGODB_VAL("operation");
MONGODB_PREFIX const char *MONGO_OPERATION_INSERT_S MONGODB_VAL("insert");
MONGODB_PREFIX const char *MONGO_OPERATION_SEARCH_S MONGODB_VAL("search");
MONGODB_PREFIX const char *MONGO_OPERATION_REMOVE_S MONGODB_VAL("remove");
MONGODB_PREFIX const char *MONGO_OPERATION_DATA_S MONGODB_VAL("data");
MONGODB_PREFIX const char *MONGO_OPERATION_FIELDS_S MONGODB_VAL("fields");
MONGODB_PREFIX const char *MONGO_OPERATION_GET_ALL_S MONGODB_VAL("dump");

#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_MONGODB_API bool InitMongo (const char *connection_s);


WHEATIS_MONGODB_API void ExitMongo (void);


WHEATIS_MONGODB_API bool SetMongoToolCollection (MongoTool *tool_p, const char *db_s, const char *collection_s);


WHEATIS_MONGODB_API MongoTool *AllocateMongoTool (void);


WHEATIS_MONGODB_API void FreeMongoTool (MongoTool *tool_p);


WHEATIS_MONGODB_API bson_oid_t *InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p);


WHEATIS_MONGODB_API bson_t *ConvertJSONToBSON (const json_t *json_p);


WHEATIS_MONGODB_API json_t *ConvertBSONToJSON (const bson_t *bson_p);


WHEATIS_MONGODB_API bool UpdateMongoDocument (MongoTool *tool_p, const bson_oid_t *id_p, json_t *json_p);


WHEATIS_MONGODB_API bool RemoveMongoDocuments (MongoTool *tool_p, const json_t *selector_json_p, const bool remove_first_match_only_flag);


WHEATIS_MONGODB_API bool FindMatchingMongoDocumentsByJSON (MongoTool *tool_p, const json_t *query_json_p, const char **fields_ss);


WHEATIS_MONGODB_API bool FindMatchingMongoDocumentsByBSON (MongoTool *tool_p, const bson_t *query_p, const char **fields_ss);


WHEATIS_MONGODB_API bool IterateOverMongoResults (MongoTool *tool_p, bool (*process_bson_fn) (const bson_t *document_p, void *data_p), void *data_p);


WHEATIS_MONGODB_API bool HasMongoQueryResults (MongoTool *tool_p);


WHEATIS_MONGODB_API json_t *GetAllMongoResultsAsJSON (MongoTool *tool_p, bson_t *query_p);


WHEATIS_MONGODB_API json_t *GetAllExistingMongoResultsAsJSON (MongoTool *tool_p);


WHEATIS_MONGODB_API bool AddBSONDocumentToJSONArray (const bson_t *document_p, void *data_p);


WHEATIS_MONGODB_API void LogBSON (const bson_t *bson_p, const int level, const char * const prefix_s);


WHEATIS_MONGODB_API void LogBSONOid (const bson_oid_t *bson_p, const int level, const char * const prefix_s);


WHEATIS_MONGODB_API void LogAllBSON (const bson_t *bson_p, const int level, const char * const prefix_s);


WHEATIS_MONGODB_API int32 IsKeyValuePairInCollection (MongoTool *tool_p, const char *database_s, const char *collection_s, const char *key_s, const char *value_s);


#ifdef __cplusplus
}
#endif



#endif /* MONGODB_TOOL_H_ */
