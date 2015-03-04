#include <string.h>

#include "blast_service.h"
#include "memory_allocations.h"

#include "blast_tool.hpp"



#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'L', 'I', 'N')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'L', 'O', 'U')

/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData bsd_base_data;
	BlastTool *blast_tool_p;
} BlastServiceData;


/*
 * STATIC PROTOTYPES
 */
 
static const char *GetBlastServiceName (Service *service_p);

static const char *GetBlastServiceDesciption (Service *service_p);

static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseBlastService (Service *service_p);

/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *config_p)
{
	Service *blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (blast_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = NULL;
					
					InitialiseService (blast_service_p, 
						GetBlastServiceName, 
						GetBlastServiceDesciption, 
						NULL,
						RunBlastService,
						IsFileForBlastService,
						GetBlastServiceParameters,
						ReleaseBlastServiceParameters,
						CloseBlastService,
						true,
						data_p);
					
					* (services_p -> sa_services_pp) = blast_service_p;
							
					return services_p;
				}
				
			FreeService (blast_service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */
 
 
static bool CloseBlastService (Service *service_p)
{
	return true;
}
 
 
static const char *GetBlastServiceName (Service *service_p)
{
	return "Blast service";
}


static const char *GetBlastServiceDesciption (Service *service_p)
{
	return "A service to run the Blast program";
}


static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *config_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Blast service parameters", "The parameters used for the Blast service");
	
	if (param_set_p)
		{
			SharedType def;
							
			def.st_resource_value_p = resource_p;

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, "Input", NULL, "The input file to read", TAG_BLAST_INPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
				{
					def.st_string_value_s = NULL;

					if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, "Output", NULL, "The output file to write", TAG_BLAST_OUTPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
						{
							return param_set_p;
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}



static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	BlastTool *tool_p = CreateBlastTool ();
	json_t *res_json_p = NULL;
	
	if (tool_p)
		{
			res = (RunBlast (tool_p)) ? OS_SUCCEEDED : OS_FAILED;

			FreeBlastTool (tool_p); 
		}
		
	res_json_p = CreateServiceResponseAsJSON (service_p, res, NULL);
		
	return res_json_p;
}


static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;
	const char *filename_s = resource_p -> re_value_s;
	
	
	/*
	 * @TODO
	 * We could check if the file is on a remote filesystem and if so
	 * make a full or partial local copy for analysis.
	 */
	
	/* 
	 * We can check on file extension and also the content of the file
	 * to determine if we want to blast this file.
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
							if (strcmp (extension_s, "fa") == 0)
								{
									interested_flag = true;
								}
								
						}		/* if (*extension_s != '\0') */
					
				}		/* if (extension_s) */
				
		}		/* if (filename_s) */
	
	
	return interested_flag;	
}

