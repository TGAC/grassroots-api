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
#ifndef PATHOGENOMICS_SERVICE_H
#define PATHOGENOMICS_SERVICE_H


#include "pathogenomics_service_library.h"
#include "service.h"
#include "mongodb_tool.h"

#define PATHOGENOMICS_DB_S ("pathogenomics")


#ifdef ALLOCATE_PATHOGENOMICS_TAGS
	#define PATHOGENOMICS_PREFIX PATHOGENOMICS_SERVICE_LOCAL
	#define PATHOGENOMICS_VAL(x)	= x
#else
	#define PATHOGENOMICS_PREFIX extern
	#define PATHOGENOMICS_VAL(x)
#endif


PATHOGENOMICS_PREFIX const char *PG_ID_S PATHOGENOMICS_VAL ("ID");
PATHOGENOMICS_PREFIX const char *PG_GPS_S PATHOGENOMICS_VAL ("GPS");
PATHOGENOMICS_PREFIX const char *PG_POSTCODE_S PATHOGENOMICS_VAL ("Postal code");
PATHOGENOMICS_PREFIX const char *PG_TOWN_S PATHOGENOMICS_VAL ("Town");
PATHOGENOMICS_PREFIX const char *PG_COUNTY_S PATHOGENOMICS_VAL ("County");
PATHOGENOMICS_PREFIX const char *PG_COUNTRY_S PATHOGENOMICS_VAL ("Country");
PATHOGENOMICS_PREFIX const char *PG_DATE_S PATHOGENOMICS_VAL ("Date collected");

PATHOGENOMICS_PREFIX const char *PG_LOCATION_S PATHOGENOMICS_VAL ("location");
PATHOGENOMICS_PREFIX const char *PG_LATITUDE_S PATHOGENOMICS_VAL ("latitude");
PATHOGENOMICS_PREFIX const char *PG_LONGITUDE_S PATHOGENOMICS_VAL ("longitude");
PATHOGENOMICS_PREFIX const char *PG_NORTH_BOUND_S PATHOGENOMICS_VAL ("north_bound");
PATHOGENOMICS_PREFIX const char *PG_SOUTH_BOUND_S PATHOGENOMICS_VAL ("south_bound");
PATHOGENOMICS_PREFIX const char *PG_EAST_BOUND_S PATHOGENOMICS_VAL ("east_bound");
PATHOGENOMICS_PREFIX const char *PG_WEST_BOUND_S PATHOGENOMICS_VAL ("west_bound");





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
