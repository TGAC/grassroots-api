#ifndef COUNTRY_CODES_H
#define COUNTRY_CODES_H

#include "mongodb_service_library.h"


typedef struct CountryCode
{
	const char *cc_name_s;
	const char *cc_code_s;
} CountryCode;


#ifdef __cplusplus
extern "C"
{
#endif


MONGODB_SERVICE_API const char *GetCountryCodeFromName (const char * const country_name_s);


#ifdef __cplusplus
}
#endif

#endif 		/* ifnder COUNTRY_CODES_H */
 
