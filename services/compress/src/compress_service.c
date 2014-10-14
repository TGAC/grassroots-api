#include <string.h>

#include <arpa/inet.h>

#include "compress_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"

#include "zip.h"
#include "gzip.h"

/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData bsd_base_data;
} CompressServiceData;


typedef enum
{
	CA_Z,
	CA_ZIP,
	CA_GZIP,
	CA_NUM_ALGORITHMS
} CompressionAlgorithm;


static const char *s_algorithm_names_pp [CA_NUM_ALGORITHMS] =
{
	"z",
	"zip",
	"gzip"
};


typedef int (*CompressFunction) (Handler *in_p, Handler *out_p, int level);

static CompressFunction s_compress_fns [CA_NUM_ALGORITHMS] = 
{
	CompressAsZipNoHeader,
	CompressAsZip,
	CompressAsGZip
};



#define TAG_COMPRESS_ALGORITHM 	(TAG_USER |	0x00000001)

static const char *S_INPUT_PARAM_NAME_S = "Input";



/*
 * STATIC PROTOTYPES
 */

static const char *GetCompressServiceName (void);

static const char *GetCompressServiceDesciption (void);

static ParameterSet *GetCompressServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p);


static int RunCompressService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForCompressService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p);


static int Compress (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);




/*
 * API FUNCTIONS
 */

Service *GetService (void)
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


void ReleaseService (Service *service_p)
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


static ParameterSet *GetCompressServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Compress service parameters", "The parameters used for the Compress service");

	if (param_set_p)
		{
			SharedType def;
				
			def.st_resource_value_p = resource_p;		

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, S_INPUT_PARAM_NAME_S, "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
				{
					ParameterMultiOptionArray *options_p = NULL;
					const char *descriptions_pp [CA_NUM_ALGORITHMS] = { "Use Raw", "Use Zip", "Use GZip" };
					SharedType values [CA_NUM_ALGORITHMS];

					values [CA_Z].st_string_value_s = (char *) s_algorithm_names_pp [CA_Z];
					values [CA_ZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_ZIP];
					values [CA_GZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_GZIP];

					options_p = AllocateParameterMultiOptionArray (CA_NUM_ALGORITHMS, descriptions_pp, values, PT_STRING);

					if (options_p)
						{
							def.st_string_value_s = values [0].st_string_value_s;

							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Compression algorithm", "The algorithm to use to compress the data with", TAG_COMPRESS_ALGORITHM, options_p, def, NULL, NULL, PL_BASIC, NULL))
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




static int RunCompressService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	int result = -1;
	SharedType input_resource;
	
	if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &input_resource, true))
		{
			Resource *input_resource_p = input_resource.st_resource_value_p;
			SharedType value;
			
			if (GetParameterValueFromParameterSet (param_set_p, TAG_COMPRESS_ALGORITHM, &value, true))
				{
					const char * const algorithm_s = value.st_string_value_s;
					
					if (algorithm_s)
						{
							result = Compress (input_resource_p, algorithm_s, credentials_p);
								
						}		/* if (algorithm_s) */
										
				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */			
								
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	

	return result;
}


static int Compress (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p)
{
	bool success_flag = false;
	int i;
	int algo_index = -1;
	int res = 0;
	
	/* Find the algorithm to use */
	for (i = 0; i < CA_NUM_ALGORITHMS; ++ i)
		{
			if (strcmp (algorithm_s, s_algorithm_names_pp [i]) == 0)
				{
					algo_index = i;
					i = CA_NUM_ALGORITHMS;
				}
		}
		
	if (algo_index)
		{
			char *output_name_s = SetFileExtension (input_resource_p -> re_value_s, s_algorithm_names_pp [algo_index]);
			
			if (output_name_s)
				{
					Handler *input_handler_p = GetResourceHandler (input_resource_p, credentials_p);

					if (input_handler_p)
						{
							Resource output_resource;
							Handler *output_handler_p = NULL;
							
							output_resource.re_protocol_s = input_resource_p -> re_protocol_s;
							output_resource.re_value_s = output_name_s;
							
							output_handler_p = GetResourceHandler (&output_resource, credentials_p);

							if (output_handler_p)
								{
									res = s_compress_fns [algo_index] (input_handler_p, output_handler_p, 0);
									
									success_flag = (res == Z_OK);
									
									CloseHandler (output_handler_p);
 								}		/* if (output_handler_p) */
							
							CloseHandler (input_handler_p);
						}		/* if (input_handler_p) */
								
					FreeCopiedString (output_name_s);
				}		/* if (output_name_s) */
						
		}		/* if (algo_index) */
	
	
	return success_flag;
}


static bool IsFileForCompressService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;
	const char *filename_s = resource_p -> re_value_s;

	
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
			
			if (OpenHandler(handler_p, filename_s, "rb"))
				{
					size_t l = sizeof (i);
					size_t r = ReadFromHandler (handler_p, &i, l);
					
					if (r == l) 
						{
							
						}
						
					CloseHandler (handler_p);
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

