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
	Service *Compress_service_p = (Service *) AllocMemory (sizeof (Service));
	ServiceData *data_p = NULL;
	
	InitialiseService (Compress_service_p, 
		GetCompressServiceName, 
		GetCompressServiceDesciption, 
		RunCompressService,
		IsFileForCompressService,
		GetCompressServiceParameters, 
		data_p);
	
	return Compress_service_p;
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
	return "A service to run the Compress program";
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
					def.st_string_value_s = NULL;

					if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, "Output", "The output file to write", NULL, def, NULL, PL_BASIC, NULL))
						{
							return param_set_p;
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
	bool interested_flag = false;
	
	/*
	 * @TODO
	 * We could check if the file is on a remote filesystem and if so
	 * make a full or partial local copy for analysis.
	 */
	
	/* 
	 * We can check on file extension and also the content of the file
	 * to determine if we want to Compress this file.
	 */
	if (filename_s)
		{
			const char *extension_s = strstr (filename_s, ".");
			
			if (extension_s)
				{
					/* move past the . */
					++ extension_s;
					
					/* check that the file doesn't end with the . */
					if (*extension_s != '\0')
						{
							interested_flag = ((strcmp (extension_s, "gz") != 0) &&
								(strcmp (extension_s, "zip") != 0));
								
						}		/* if (*extension_s != '\0') */
					
				}		/* if (extension_s) */
				
		}		/* if (filename_s) */
	
	
	return interested_flag;	
}

