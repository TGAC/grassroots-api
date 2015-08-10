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

#include "mongodb_service.h"

#ifdef __cplusplus
extern "C"
{
#endif



MONGODB_SERVICE_LOCAL json_t *GetLocationData (const json_t *row_p, MongoDBServiceData *data_p);


MONGODB_SERVICE_LOCAL bool ConvertDate (json_t *row_p);


MONGODB_SERVICE_LOCAL json_t *RefineLocationData (json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s);

#ifdef __cplusplus
}
#endif


#endif /* SAMPLE_METADATA_H_ */
