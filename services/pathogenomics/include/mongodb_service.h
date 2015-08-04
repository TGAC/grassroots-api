#ifndef MONGODB_SERVICE_H
#define MONGODB_SERVICE_H


#include "mongodb_service_library.h"
#include "service.h"

#define PATHOGENOMICS_DB_S ("pathogenomics")


#ifdef ALLOCATE_PATHOGENOMICS_TAGS
	#define PATHOGENOMICS_PREFIX MONGODB_SERVICE_LOCAL
	#define PATHOGENOMICS_VAL(x)	= x
#else
	#define PATHOGENOMICS_PREFIX extern
	#define PATHOGENOMICS_VAL(x)
#endif


PATHOGENOMICS_PREFIX const char *PG_ID_S = "ID";
PATHOGENOMICS_PREFIX const char *PG_GPS_S = "GPS";
PATHOGENOMICS_PREFIX const char *PG_POSTCODE_S = "Postal code";
PATHOGENOMICS_PREFIX const char *PG_TOWN_S = "Town";
PATHOGENOMICS_PREFIX const char *PG_COUNTY_S = "County";
PATHOGENOMICS_PREFIX const char *PG_COUNTRY_S = "Country";
PATHOGENOMICS_PREFIX const char *PG_DATE_S = "Date collected";
PATHOGENOMICS_PREFIX const char *PG_GEOJSON_S = "GeoJSON";



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
