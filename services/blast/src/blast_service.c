#include <string.h>

#include "blast_service.h"
#include "memory_allocations.h"

#include "blast_tool.hpp"


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
 
static const char *GetBlastServiceName (void);

static const char *GetBlastServiceDesciption (void);

static ParameterSet *GetBlastServiceParameters (ServiceData *service_data_p, TagItem *tags_p);

static int RunBlastService (ServiceData *service_data_p, ParameterSet *param_set_p);

static bool IsFileForBlastService (ServiceData *service_data_p, TagItem *tags_p, Handle *handle_p);





/*
 * API FUNCTIONS
 */

Service *GetService (void)
{
	Service *blast_service_p = (Service *) AllocMemory (sizeof (Service));
	ServiceData *data_p = NULL;
	
	InitialiseService (blast_service_p, 
		GetBlastServiceName, 
		GetBlastServiceDesciption, 
		RunBlastService,
		IsFileForBlastService,
		GetBlastServiceParameters, 
		data_p);
	
	return blast_service_p;
}


void ReleaseService (Service *service_p)
{
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS 
 */
 
 
static const char *GetBlastServiceName (void)
{
	return "Blast service";
}


static const char *GetBlastServiceDesciption (void)
{
	return "A service to run the Blast program";
}


static ParameterSet *GetBlastServiceParameters (ServiceData *service_data_p, TagItem *tags_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Blast service parameters", "The parameters used for the Blast service");
	
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

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, "Input", "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, PL_BASIC, NULL))
				{
					def.st_string_value_s = NULL;

					if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, "Output", "The output file to write", TAG_OUTPUT_FILE, NULL, def, NULL, PL_BASIC, NULL))
						{
							return param_set_p;
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}



static int RunBlastService (ServiceData *service_data_p, ParameterSet *param_set_p)
{
	int result = -1;
	BlastTool *tool_p = CreateBlastTool ();
	
	if (tool_p)
		{
			if (RunBlast (tool_p))
				{
					result = 0;
				}
				
			FreeBlastTool (tool_p); 
		}
		
	return result;
}


static bool IsFileForBlastService (ServiceData *service_data_p, TagItem *tags_p, Handle *handle_p)
{
	bool interested_flag = false;
	const char *filename_s = NULL;
	TagItem *tag_item_p = FindMatchingTag (tags_p, TAG_INPUT_FILE);
	
	if (tag_item_p)
		{
			filename_s = tag_item_p -> ti_value.st_string_value_s;
		}
	
	
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

