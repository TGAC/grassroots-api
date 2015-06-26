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


} MongoTool;


bool InitMongo (const char *connection_s);

void ExitMongo (void);


bool GetMongoCollection (MongoTool *tool_p, const char *db_s, const char *collection_s);


MongoTool *AllocateMongoTool (void);

void FreeMongoTool (MongoTool *tool_p);

bool InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p);


#endif /* MONGODB_TOOL_H_ */
