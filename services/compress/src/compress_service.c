#include <string.h>

#include "compress_service.h"
#include "connect.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "stream.h"



/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData bsd_base_data;
} CompressServiceData;


/*
 * STATIC PROTOTYPES
 */

static const char *GetCompressServiceName (void);

static const char *GetCompressServiceDesciption (void);

static ParameterSet *GetCompressServiceParameters (void);


static int RunCompressService (const char * const filename_s, ParameterSet *param_set_p);

static bool IsFileForCompressService (const char * const filename_s, Stream *stream_p);





/*
 * API FUNCTIONS
 */

Service *AllocateService (void)
{
	Service *compress_service_p = (Service *) AllocMemory (sizeof (Service));
	ServiceData *data_p = NULL;

	InitialiseService (compress_service_p,
		GetCompressServiceName,
		GetCompressServiceDesciption,
		RunCompressService,
		IsFileForCompressService,
		GetCompressServiceParameters,
		data_p);

	return compress_service_p;
}


void FreeService (Service *service_p)
{
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */


static const char *GetCompressServiceName (void)
{
	return "Compress service";
}


static const char *GetCompressServiceDesciption (void)
{
	return "A service to compress data";
}


static ParameterSet *GetCompressServiceParameters (void)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Compress service parameters", "The parameters used for the Compress service");

	if (param_set_p)
		{
			SharedType def;

			def.st_string_value_s = NULL;

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, "Input", "The input file to read", NULL, def, NULL, PL_BASIC, NULL))
				{
					ParameterMultiOptionArray *options_p = NULL;
					const char *descriptions_pp [] = { "Use Zip", "Use GZip" };
					SharedType values [2];

					values [0].st_string_value_s = "zip";
					values [1].st_string_value_s = "gzip";

					options_p = AllocateParameterMultiOptionArray (2, descriptions_pp, values, PT_STRING);

					if (options_p)
						{
							def.st_string_value_s = values [0].st_string_value_s;

							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Compression algorithm", "The algorithm to use to compress the data with", options_p, def, NULL, PL_BASIC, NULL))
								{
									return param_set_p;
								}

							FreeParameterMultiOptionArray (options_p);
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}



static int RunCompressService (const char * const filename_s, ParameterSet *param_set_p)
{
	int result = -1;

	return result;
}


static bool IsFileForCompressService (const char * const filename_s, Stream *stream_p)
{
	bool interested_flag = true;

	/*
	 * @TODO
	 * We could check if the file is on a remote filesystem and if so
	 * make a full or partial local copy for analysis.
	 */

	if (filename_s)
		{
			/*
				Rather than use the extension, let's check the file header
			*/
			uint32 header = 0;
			uint32 i = 0;
			
			if (OpenStream (stream_p, filename_s, "rb"))
				{
					size_t l = sizeof (i);
					size_t r = ReadFromStream (stream_p, &i, l);
					
					if (r == l) 
						{
							
						}
						
					CloseStream (stream_p);
				}
				
			header = htonl (i);
			
			if (header == 0x504B0304)
				{
					/* it's already a zip file */
					interested_flag = false;
				}
			else if ((header | 0x1F8BFFFF) == 0x1F8BFFFF)
				{
					/* it's a gzip file */
					interested_flag = false;
				}


		}		/* if (filename_s) */


	return interested_flag;
}

