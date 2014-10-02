#include <string.h>

#include <arpa/inet.h>

#include "compress_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "stream.h"

#include "zip.h"

/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData bsd_base_data;
} CompressServiceData;


static enum
{
	CA_ZIP,
	CA_GZIP,
	CA_NUM_ALGORITHMS
} CompressionAlgorithms;


static const char *s_algorithm_names_pp [CA_NUM_ALGORITHMS] =
{
	"zip",
	"gzip"
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

static bool IsFileForCompressService (ServiceData *service_data_p, TagItem *tags_p, Stream *stream_p);





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

					values [CA_ZIP].st_string_value_s = s_algorithm_names_pp [CA_ZIP];
					values [CA_GZIP].st_string_value_s = s_algorithm_names_pp [CA_GZIP];

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



static int RunCompressService (ServiceData *service_data_p, ParameterSet *param_set_p)
{
	int result = -1;
	SharedType value;
	
	if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value, true))
		{
			const char *input_name_s = value.st_string_value_s;
			
			if (GetParameterValueFromParameterSet (param_set_p, TAG_COMPRESS_ALGORITHM, &value, true))
				{
					/*
					 * need to have a way to check whether the filename points to a local, remote, irods,
					 * url, etc.
					 */
					if ()
					
					
					
				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */			
			
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	

	return result;
}


static bool IsFileForCompressService (ServiceData *service_data_p, TagItem *tags_p, Stream *stream_p)
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

