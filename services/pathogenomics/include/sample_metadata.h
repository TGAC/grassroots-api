/*
 * sample_metadata.h
 *
 *  Created on: 1 Jul 2015
 *      Author: tyrrells
 */

#ifndef SAMPLE_METADATA_H_
#define SAMPLE_METADATA_H_

#include "bson.h"
#include "mongodb_service_library.h"
#include "mongodb_tool.h"

#ifdef __cplusplus
extern "C"
{
#endif


MONGODB_SERVICE_LOCAL bson_t *GetSamples (MongoTool *tool_p, json_t *query_p);




#ifdef __cplusplus
}
#endif


#endif /* SAMPLE_METADATA_H_ */
