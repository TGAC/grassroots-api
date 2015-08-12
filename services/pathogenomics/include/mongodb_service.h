#ifndef MONGODB_SERVICE_H
#define MONGODB_SERVICE_H


#include "mongodb_service_library.h"
#include "service.h"
#include "mongodb_tool.h"

#define PATHOGENOMICS_DB_S ("pathogenomics")


#ifdef ALLOCATE_PATHOGENOMICS_TAGS
	#define PATHOGENOMICS_PREFIX MONGODB_SERVICE_LOCAL
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
PATHOGENOMICS_PREFIX const char *PG_LONGTITUDE_S PATHOGENOMICS_VAL ("longtitude");
PATHOGENOMICS_PREFIX const char *PG_NORTH_BOUND_S PATHOGENOMICS_VAL ("north_bound");
PATHOGENOMICS_PREFIX const char *PG_SOUTH_BOUND_S PATHOGENOMICS_VAL ("south_bound");
PATHOGENOMICS_PREFIX const char *PG_EAST_BOUND_S PATHOGENOMICS_VAL ("east_bound");
PATHOGENOMICS_PREFIX const char *PG_WEST_BOUND_S PATHOGENOMICS_VAL ("west_bound");


typedef struct MongoDBServiceData MongoDBServiceData;


struct MongoDBServiceData
{
	ServiceData bsd_base_data;

	MongoTool *msd_tool_p;

	const char *msd_samples_collection_s;

	const char *msd_locations_collection_s;

	json_t *(*msd_geocoder_fn) (struct MongoDBServiceData *data_p, const json_t *row_p);

	json_t *(*msd_refine_location_fn) (struct MongoDBServiceData *data_p, json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s);

	const char *msd_geocoder_uri_s;

};



#ifdef __cplusplus
extern "C"
{
#endif


MONGODB_SERVICE_API ServicesArray *GetServices (json_t *config_p);


MONGODB_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef MONGODB_SERVICE_H */
