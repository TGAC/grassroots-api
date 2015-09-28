/*
 * phenotype_metadata.h
 *
 *  Created on: 16 Sep 2015
 *      Author: tyrrells
 */

#ifndef PHENOTYPE_METADATA_H_
#define PHENOTYPE_METADATA_H_

#include "pathogenomics_service_library.h"
#include "mongodb_tool.h"
#include "pathogenomics_service_data.h"

#include "pathogenomics_service.h"


#ifdef __cplusplus
extern "C"
{
#endif


PATHOGENOMICS_SERVICE_LOCAL	bool InsertPhenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p, json_t *errors_p);


#ifdef __cplusplus
}
#endif


#endif /* PHENOTYPE_METADATA_H_ */
