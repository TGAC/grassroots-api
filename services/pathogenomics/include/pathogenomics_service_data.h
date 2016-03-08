/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * pathogenomics_service_data.h
 *
 *  Created on: 28 Sep 2015
 *      Author: tyrrells
 */

#ifndef PATHOGENOMICS_SERVICE_DATA_H_
#define PATHOGENOMICS_SERVICE_DATA_H_

#include "jansson.h"

#include "service.h"
#include "mongodb_tool.h"


typedef enum
{
	PD_SAMPLE,
	PD_PHENOTYPE,
	PD_GENOTYPE,
	PD_FILES,
	PD_NUM_TYPES
} PathogenomicsData;

typedef struct PathogenomicsServiceData PathogenomicsServiceData;

/**
 *
 * @extends ServiceData
 */
struct PathogenomicsServiceData
{
	ServiceData psd_base_data;

	MongoTool *psd_tool_p;

	const char *psd_database_s;

	const char *psd_collection_ss [PD_NUM_TYPES];

//	const char *psd_locations_collection_s;

	bool (*psd_geocoder_fn) (struct PathogenomicsServiceData *data_p, json_t *row_p, const char * const id_s);

	const char *psd_geocoder_uri_s;


	const char *psd_files_download_root_uri_s;
};


#endif /* PATHOGENOMICS_SERVICE_DATA_H_ */
