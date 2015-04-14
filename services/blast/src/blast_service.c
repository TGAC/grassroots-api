#include <string.h>

#include "blast_service.h"
#include "memory_allocations.h"

#include "blast_tool.hpp"



#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')

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

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, "Input", NULL, "The input file to read", TAG_BLAST_INPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL))
				{
					def.st_string_value_s = NULL;

					if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, "Output", NULL, "The output file to write", TAG_BLAST_OUTPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL))
						{
							def.st_string_value_s = NULL;

							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Query Sequence(s)", NULL, "Query sequence(s) to be used for a BLAST search should be pasted in the 'Search' text area. "
							  "It accepts a number of different types of input and automatically determines the format or the input."
							  " To allow this feature there are certain conventions required with regard to the input of identifiers (e.g., accessions or gi's)", TAG_BLAST_INPUT_QUERY, NULL, def, NULL, NULL, PL_ALL, NULL))
								{
									def.st_ulong_value = 100;

									if (CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "Max target sequences", NULL, "Select the maximum number of aligned sequences to display (the actual number of alignments may be greater than this)." , TAG_BLAST_OUTPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL))
										{
											def.st_boolean_value = true;

											if (CreateAndAddParameterToParameterSet (param_set_p, PT_BOOLEAN, "Short queries", NULL, "Automatically adjust parameters for short input sequences", TAG_BLAST_SHORT_QUERIES, NULL, def, NULL, NULL, PL_ALL, NULL))
												{
													def.st_ulong_value = 10;

													if (CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "Expect threshold", NULL, "Expected number of chance matches in a random model" , TAG_BLAST_EXPECT_THRESHOLD, NULL, def, NULL, NULL, PL_ALL, NULL))
														{
															def.st_ulong_value = 28;

															if (CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "Word size", NULL, "Expected number of chance matches in a random model", TAG_BLAST_WORD_SIZE, NULL, def, NULL, NULL, PL_ALL, NULL))
																{
																	def.st_ulong_value = 0;

																	if (CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "Max matches in a query range", NULL, "Limit the number of matches to a query range. This option is useful if many strong matches to one part of a query may prevent BLAST from presenting weaker matches to another part of the query", TAG_BLAST_WORD_SIZE, NULL, def, NULL, NULL, PL_ALL, NULL))
																		{

																			return param_set_p;
																		}
																}
														}
												}
										}
								}
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

