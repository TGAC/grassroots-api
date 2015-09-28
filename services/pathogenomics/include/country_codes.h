#ifndef COUNTRY_CODES_H
#define COUNTRY_CODES_H

#include "pathogenomics_service_library.h"
#include "pathogenomics_service_data.h"
#include "mongodb_tool.h"

typedef struct CountryCode
{
	const char *cc_name_s;
	const char *cc_code_s;
} CountryCode;


#ifdef __cplusplus
extern "C"
{
#endif


PATHOGENOMICS_SERVICE_LOCAL const char *GetCountryCodeFromName (const char * const country_name_s);


PATHOGENOMICS_SERVICE_LOCAL bool IsValidCountryCode (const char * const code_s);


PATHOGENOMICS_SERVICE_LOCAL bool InsertLocationData (MongoTool *tool_p, const json_t *row_p, PathogenomicsServiceData *data_p, const char *id_s);


#ifdef __cplusplus
}
#endif

#endif 		/* ifnder COUNTRY_CODES_H */
 
