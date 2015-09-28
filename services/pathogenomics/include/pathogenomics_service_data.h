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


typedef struct PathogenomicsServiceData PathogenomicsServiceData;


struct PathogenomicsServiceData
{
	ServiceData psd_base_data;

	MongoTool *psd_tool_p;

	const char *psd_database_s;

	const char *psd_samples_collection_s;

	const char *psd_locations_collection_s;

	const char *psd_phenotype_collection_s;

	json_t *(*psd_geocoder_fn) (struct PathogenomicsServiceData *data_p, const json_t *row_p, const char * const id_s);

	json_t *(*psd_refine_location_fn) (struct PathogenomicsServiceData *data_p, json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s);

	const char *psd_geocoder_uri_s;

};


#endif /* PATHOGENOMICS_SERVICE_DATA_H_ */
