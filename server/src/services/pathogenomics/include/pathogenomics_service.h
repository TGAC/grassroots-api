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
#ifndef PATHOGENOMICS_SERVICE_H
#define PATHOGENOMICS_SERVICE_H


#include "pathogenomics_service_library.h"
#include "service.h"
#include "mongodb_tool.h"

/**
 * The name of the database to use to store all of the pathogenomics details.
 */
#define PATHOGENOMICS_DB_S ("pathogenomics")


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_PATHOGENOMICS_TAGS
	#define PATHOGENOMICS_PREFIX PATHOGENOMICS_SERVICE_LOCAL
	#define PATHOGENOMICS_VAL(x)	= x
#else
	#define PATHOGENOMICS_PREFIX extern
	#define PATHOGENOMICS_VAL(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


PATHOGENOMICS_PREFIX const char *PG_SAMPLE_S PATHOGENOMICS_VAL ("sample");

PATHOGENOMICS_PREFIX const char *PG_ID_S PATHOGENOMICS_VAL ("ID");

PATHOGENOMICS_PREFIX const char *PG_UKCPVS_ID_S PATHOGENOMICS_VAL ("UKCPVS ID");

PATHOGENOMICS_PREFIX const char *PG_GPS_S PATHOGENOMICS_VAL ("GPS");

PATHOGENOMICS_PREFIX const char *PG_POSTCODE_S PATHOGENOMICS_VAL ("Postal code");

PATHOGENOMICS_PREFIX const char *PG_TOWN_S PATHOGENOMICS_VAL ("Town");

PATHOGENOMICS_PREFIX const char *PG_COUNTY_S PATHOGENOMICS_VAL ("County");

PATHOGENOMICS_PREFIX const char *PG_COUNTRY_S PATHOGENOMICS_VAL ("Country");

/**
 * The key used to define the data that sample was collected in the form yyyy-mm-dd.
 */
PATHOGENOMICS_PREFIX const char *PG_DATE_S PATHOGENOMICS_VAL ("Date collected");

/**
 * The key used to define the data that sample was collected in the form yyyymmdd.
 */
PATHOGENOMICS_PREFIX const char *PG_RAW_DATE_S PATHOGENOMICS_VAL ("Date collected (compact)");


/**
 *
 */
PATHOGENOMICS_PREFIX const char *PG_RUST_S PATHOGENOMICS_VAL ("Rust (YR/SR/LR)");

/**
 * The key used to hold the object that contains all of the genotyping
 * data for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_GENOTYPE_S PATHOGENOMICS_VAL ("genotype");

/**
 * The key used to hold the object that contains all of the phenotyping
 * data for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_PHENOTYPE_S PATHOGENOMICS_VAL ("phenotype");

PATHOGENOMICS_PREFIX const char *PG_FILES_S PATHOGENOMICS_VAL ("files");


PATHOGENOMICS_PREFIX const char *PG_DISEASE_S PATHOGENOMICS_VAL ("Disease");

PATHOGENOMICS_PREFIX const char *PG_VARIETY_S PATHOGENOMICS_VAL ("Variety");

PATHOGENOMICS_PREFIX const char *PG_COLLECTOR_S PATHOGENOMICS_VAL ("Name/Collector");

PATHOGENOMICS_PREFIX const char *PG_COMPANY_S PATHOGENOMICS_VAL ("Company");

PATHOGENOMICS_PREFIX const char *PG_ADDRESS_S PATHOGENOMICS_VAL ("Address");

/**
 * The key for the location object for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_LOCATION_S PATHOGENOMICS_VAL ("location");

/**
 * The key for the latitude of for a location object for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_LATITUDE_S PATHOGENOMICS_VAL ("latitude");

/**
 * The key for the longitude of for a location object for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_LONGITUDE_S PATHOGENOMICS_VAL ("longitude");

/**
 * The key for the north-eastern bounds for the location object for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_NORTH_EAST_LOCATION_S PATHOGENOMICS_VAL ("north_east_bound");

/**
 * The key for the south-western bounds for the location object for a given record.
 */
PATHOGENOMICS_PREFIX const char *PG_SOUTH_WEST_LOCATION_S PATHOGENOMICS_VAL ("south_west_bound");


PATHOGENOMICS_PREFIX const char *PG_PRIVATE_VIEW_S PATHOGENOMICS_VAL ("ignore_live_dates");

/**
 * The suffix attached to various keys used to create an associated key
 * that holds the value of when the first key's value should be made public.
 *
 * For example
 *
 * 	"genotype": { ... },
 * 	"genotype_live_date": "2016-01-01"
 */
#define PG_LIVE_DATE_SUFFIX_S "_live_date"


#ifdef __cplusplus
extern "C"
{
#endif


PATHOGENOMICS_SERVICE_API ServicesArray *GetServices (json_t *config_p);


PATHOGENOMICS_SERVICE_API void ReleaseServices (ServicesArray *services_p);


PATHOGENOMICS_SERVICE_LOCAL bool AddErrorMessage (json_t *errors_p, const json_t *values_p, const size_t row, const char * const error_s);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PATHOGENOMICS_SERVICE_H */
