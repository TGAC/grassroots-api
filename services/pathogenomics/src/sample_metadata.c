/*
 * sample_metadata.c
 *
 *  Created on: 1 Jul 2015
 *      Author: tyrrells
 */
#include "sample_metadata.h"
#include "mongo_tool.h"

bson_t *GetSamples (MongoTool *tool_p, json_t *query_p, const char **fields_ss)
{
	bson_t *docs_p = NULL;

	if (FindMatchingMongoDocuments (tool_p, query_p, fields_ss))
		{

		}

	return docs_p;
}

