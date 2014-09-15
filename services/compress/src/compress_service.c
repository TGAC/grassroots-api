#include <string.h>

#include "compress_service.h"
#include "memory_allocations.h"


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

static bool IsFileForCompressService (const char * const filename_s, FileLocation loc);





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

					LinkedListAddTail (params_p, (ListNode * const) node_p);

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

							FreeeParameterMultiOptionArray (options_p);
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


static bool IsFileForCompressService (const char * const filename_s, FileLocation loc)
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

			if (loc == FILE_LOCATION_IRODS)
				{
					rcComm_t *conn_p = CreateConnection ();

					if (conn_p)
						{
							openedDataObjInp_t *data_obj_p = OpenDataOj (conn_p);

							if (data_obj_p)
								{
									byteBuffer_t buffer;

									buffer.len = 4;
									buffer.buffer = &header;

									DataRead (conn_p, data_obj_p, &buffer, 4);

									CloseDataObj (data_obj_p, true);
								}

							CloseConnection (conn_p);
						}

				}
			else
				{
					FILE *in_f = fopen (filename_s, "rb");

					if (in_f)
						{
							if (fread (&header, sizeof (uint32), 1, in_f) == 1)
								{
								}

							fclose (in_f);
						}
				}


			if (data == 0x04034b50)
				{
					/* it's already a zip file */
					interested_flag = false;
				}
			else if (data && 0x1F8B0000 == 0x1F8B0000)
				{
					/* it's a gzip file */
					interested_flag = false;
				}


		}		/* if (filename_s) */


	return interested_flag;
}

