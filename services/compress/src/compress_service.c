#include <string.h>

#include <arpa/inet.h>

#include "compress_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handle.h"
#include "handle_utils.h"
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
	CA_ZIP,
	CA_GZIP,
	CA_NUM_ALGORITHMS
} CompressionAlgorithm;


static const char *s_algorithm_names_pp [CA_NUM_ALGORITHMS] =
{
	"zip",
	"gzip"
};


typedef int (*CompressFunction) (Handle *in_p, Handle *out_p, int level);

static CompressFunction s_compress_fns [CA_NUM_ALGORITHMS] = 
{
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

static ParameterSet *GetCompressServiceParameters (ServiceData *service_data_p, TagItem *tags_p);


static int RunCompressService (ServiceData *service_data_p, ParameterSet *param_set_p);

static bool IsFileForCompressService (ServiceData *service_data_p, TagItem *tags_p, Handle *handle_p);





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


static ParameterSet *GetCompressServiceParameters (ServiceData *service_data_p, TagItem *tags_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Compress service parameters", "The parameters used for the Compress service");

	if (param_set_p)
		{
			SharedType def;
			TagItem *tag_p = FindMatchingTag (tags_p, TAG_INPUT_FILE);
				
			if (tag_p)
				{
					def.st_string_value_s = tag_p -> ti_value.st_string_value_s;					
				}
			else
				{
					def.st_string_value_s = NULL;										
				}

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, S_INPUT_PARAM_NAME_S, "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, PL_BASIC, NULL))
				{
					ParameterMultiOptionArray *options_p = NULL;
					const char *descriptions_pp [CA_NUM_ALGORITHMS] = { "Use Zip", "Use GZip" };
					SharedType values [CA_NUM_ALGORITHMS];

					values [CA_ZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_ZIP];
					values [CA_GZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_GZIP];

					options_p = AllocateParameterMultiOptionArray (CA_NUM_ALGORITHMS, descriptions_pp, values, PT_STRING);

					if (options_p)
						{
							def.st_string_value_s = values [0].st_string_value_s;

							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Compression algorithm", "The algorithm to use to compress the data with", TAG_COMPRESS_ALGORITHM, options_p, def, NULL, PL_BASIC, NULL))
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


static int Compress (Resource *input_resource_p, const char * const algorithm_s);

static int RunCompressService (ServiceData *service_data_p, ParameterSet *param_set_p)
{
	int result = -1;
	SharedType input_resource;
	
	if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &input_resource, true))
		{
			Resource *input_resource_p = & (input_resource.st_resource_value);
			SharedType value;
			
			if (GetParameterValueFromParameterSet (param_set_p, TAG_COMPRESS_ALGORITHM, &value, true))
				{
					const char * const algorithm_s = value.st_string_value_s;
					
					if (algorithm_s)
						{
							result = Compress (input_resource_p, algorithm_s);
								
						}		/* if (algorithm_s) */
										
				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */			
								
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	

	return result;
}


static int Compress (Resource *input_resource_p, const char * const algorithm_s)
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
					Handle *input_handle_p = GetResourceHandle (input_resource_p);

					if (input_handle_p)
						{
							Resource output_resource;
							Handle *output_handle_p = NULL;
							
							output_resource.re_protocol = input_resource_p -> re_protocol;
							output_resource.re_value_s = output_name_s;
							
							output_handle_p = GetResourceHandle (&output_resource);

							if (output_handle_p)
								{
									res = s_compress_fns [algo_index] (input_handle_p, output_handle_p, 0);
									
									CloseHandle (output_handle_p);
 								}		/* if (output_handle_p) */
							
							CloseHandle (input_handle_p);
						}		/* if (input_handle_p) */
								
					FreeCopiedString (output_name_s);
				}		/* if (output_name_s) */
						
		}		/* if (algo_index) */
	
	
	
	return success_flag;
}


static bool IsFileForCompressService (ServiceData *service_data_p, TagItem *tags_p, Handle *handle_p)
{
	bool interested_flag = true;
	const char *filename_s = NULL;
	TagItem *input_filename_tag_p = FindMatchingTag (tags_p, TAG_INPUT_FILE);
	
	if (input_filename_tag_p)
		{
			filename_s = input_filename_tag_p -> ti_value.st_string_value_s;
		}
	
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
			
			if (OpenHandle (handle_p, filename_s, "rb"))
				{
					size_t l = sizeof (i);
					size_t r = ReadFromHandle (handle_p, &i, l);
					
					if (r == l) 
						{
							
						}
						
					CloseHandle (handle_p);
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

