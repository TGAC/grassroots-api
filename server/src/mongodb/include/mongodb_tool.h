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


#ifdef ALLOCATE_MONGODB_TAGS
	#define MONGODB_PREFIX GRASSROOTS_MONGODB_API
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


MONGODB_PREFIX const char *MONGO_CLAUSE_OPERATOR_S MONGODB_VAL("operator");
MONGODB_PREFIX const char *MONGO_CLAUSE_VALUE_S MONGODB_VAL("value");


/**
 * A MongoTool is a datatype that allows access to the data stored within
 * a MongoDB instance.
 */
typedef struct MongoTool
{
	/**
	 * @private
	 *
	 * This is the current mongo client.
	 */
	mongoc_client_t *mt_client_p;


	/**
	 * @private
	 *
	 * This is the current mongo collection.
	 */
	mongoc_collection_t *mt_collection_p;

	/**
	 * @private
	 *
	 * This is the current mongo cursor.
	 */
	mongoc_cursor_t *mt_cursor_p;
} MongoTool;


/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_MONGODB_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_MONGODB_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * mongodb_tool.c.
 */

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Set the database an collection that a MongoTool will use.
 *
 * @param tool_p The MongoTool to update.
 * @param db_s The database to use.
 * @param collection_s The collection to use.
 * @return <code>true</code> if the MongoTool was updated successfully,
 * <code>false</code> otherwise.
 * @memberof MongoTool
 */
GRASSROOTS_MONGODB_API bool SetMongoToolCollection (MongoTool *tool_p, const char *db_s, const char *collection_s);


/**
 * This allocates a MongoTool that connects to the MongoDB instance specified in the
 * grassroots.config file
 *
 * @return A MongoTool or <code>NULL</code> upon error.
 * @memberof MongoTool
 * @see InitMongoDB
 */
GRASSROOTS_MONGODB_API MongoTool *AllocateMongoTool (void);


/**
 * Delete a MongoTool and release the connection that it held
 *
 * @param tool_p The MongoTool to free.
 * @memberof MongoTool
 */
GRASSROOTS_MONGODB_API void FreeMongoTool (MongoTool *tool_p);


/**
 * Insert data from a given JSON fragment using a given MongoTool.
 *
 * @param tool_p The MongoTool to use.
 * @param json_p The JSON fragment to insert.
 * @return A pointer to a newly-created BSON id or <code>NULL</code>
 * upon error. This value will need to be freed using FreeMemory to
 * avoid a memory leak.
 * @memberof MongoTool
 */
GRASSROOTS_MONGODB_API bson_oid_t *InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p);


/**
 * Create a new BSON fragment from a given JSON one.
 *
 * @param json_p The JSON fragment to convert to BSON.
 * @return The BSON fragment or <code>NULL</code> upon error.
 */
GRASSROOTS_MONGODB_API bson_t *ConvertJSONToBSON (const json_t *json_p);


/**
 * Create a new JSON fragment from a given BSON one.
 *
 * @param bson_p The BSON fragment to convert to JSON.
 * @return The JSON fragment or <code>NULL</code> upon error.
 */
GRASSROOTS_MONGODB_API json_t *ConvertBSONToJSON (const bson_t *bson_p);


GRASSROOTS_MONGODB_API bool UpdateMongoDocument (MongoTool *tool_p, const bson_oid_t *id_p, const json_t *json_p);


GRASSROOTS_MONGODB_API bool UpdateMongoDocumentByBSON (MongoTool *tool_p, const bson_t *query_p, const json_t *update_p);


GRASSROOTS_MONGODB_API bool UpdateMongoDocumentByJSON (MongoTool *tool_p, const json_t *query_p, const json_t *update_p);


GRASSROOTS_MONGODB_API bool RemoveMongoDocuments (MongoTool *tool_p, const json_t *selector_json_p, const bool remove_first_match_only_flag);


GRASSROOTS_MONGODB_API bool FindMatchingMongoDocumentsByJSON (MongoTool *tool_p, const json_t *query_json_p, const char **fields_ss);


GRASSROOTS_MONGODB_API bool FindMatchingMongoDocumentsByBSON (MongoTool *tool_p, const bson_t *query_p, const char **fields_ss);


GRASSROOTS_MONGODB_API bool IterateOverMongoResults (MongoTool *tool_p, bool (*process_bson_fn) (const bson_t *document_p, void *data_p), void *data_p);


/**
 * Check whether a MongoTool has any results after running a query.
 *
 * @param tool_p The MongoTool to check.
 * @return <code>true</code> if the MongoTool has results,
 * <code>false</code> otherwise.
 * @memberof MongoTool

 */
GRASSROOTS_MONGODB_API bool HasMongoQueryResults (MongoTool *tool_p);


/**
 *
 * Get all results from a mongodb collection.
 *
 * @param tool_p The MongoTool to get the results with.
 * @param query_p The query to run.
 * @return A json_t array with all of the results from the search or <code>NULL</code> upon error.
 * @memberof MongoTool
 */
GRASSROOTS_MONGODB_API json_t *GetAllMongoResultsAsJSON (MongoTool *tool_p, bson_t *query_p);


GRASSROOTS_MONGODB_API int32 GetAllMongoResultsForKeyValuePair (MongoTool *tool_p, json_t **docs_pp, const char * const key_s, const char * const value_s, const char **fields_ss);


GRASSROOTS_MONGODB_API json_t *GetCurrentValuesAsJSON (MongoTool *tool_p, const char **fields_ss, const size_t num_fields);


GRASSROOTS_MONGODB_API json_t *GetAllExistingMongoResultsAsJSON (MongoTool *tool_p);


GRASSROOTS_MONGODB_API bool AddBSONDocumentToJSONArray (const bson_t *document_p, void *data_p);


/**
 * Print the JSON representation of a BSON fragment to the log Stream.
 *
 * @param level The Stream level to specify when printing this.
 * @param filename_s The name of the file which will be specified with this message.
 * @param line_number The line number which will be specified with this message.
 * @param bson_p The BSON fragment to print.
 * @param prefix_s An optional string to print in the stream prior to the BSON representation.
 */
GRASSROOTS_MONGODB_API void PrintBSONToLog (const int level, const char * const filename_s, const int line_number, const bson_t *bson_p, const char * const prefix_s);


/**
 * Print the JSON representation of a BSON fragment to the errors Stream.
 *
 * @param level The Stream level to specify when printing this.
 * @param filename_s The name of the file which will be specified with this message.
 * @param line_number The line number which will be specified with this message.
 * @param bson_p The BSON fragment to print.
 * @param prefix_s An optional string to print in the stream prior to the BSON representation.
 */
GRASSROOTS_MONGODB_API void PrintBSONToErrors (const int level, const char * const filename_s, const int line_number, const bson_t *bson_p, const char * const prefix_s);


GRASSROOTS_MONGODB_API void LogBSONOid (const bson_oid_t *bson_p, const int level, const char * const prefix_s);


GRASSROOTS_MONGODB_API void LogAllBSON (const bson_t *bson_p, const int level, const char * const file_s, const int line, const char * const prefix_s);


GRASSROOTS_MONGODB_API int32 IsKeyValuePairInCollection (MongoTool *tool_p, const char *database_s, const char *collection_s, const char *key_s, const char *value_s);


GRASSROOTS_MONGODB_API bson_t *GenerateQuery (const json_t *json_p);


GRASSROOTS_MONGODB_API json_t *ConvertBSONValueToJSON (const bson_value_t *value_p);


GRASSROOTS_MONGODB_API const char *InsertOrUpdateMongoData (MongoTool *tool_p, json_t *values_p, const char * const database_s, const char * const collection_s, const char * const primary_key_id_s, const char * const mapped_id_s, const char * const object_key_s);


#ifdef __cplusplus
}
#endif



#endif /* MONGODB_TOOL_H_ */
