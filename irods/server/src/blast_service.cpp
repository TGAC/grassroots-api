#include "blast_service.h"


typedef struct BlastService 
{
	struct Service;
	BlastTool *tool_p;
	
} BlastService;


/**
 * static function prototypes
 */
static int RunBlast (const char * const filsename_s, ParameterSet *param_set_p);

static bool IsBlastableFile (const char * const filename_s);

static ParameterSet *GetBlastParameters (void);



Service *AllocateService (void)
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (service_p)
		{
			service_p -> se_run_fn = RunBlast;
			service_p -> se_match_fn = IsBlastableFile;
			service_p -> se_get_params_fn = RunBlast;
		}
	
	return service_p;	
}


void FreeService (Service *service_p)
{
	delete service_p;
}


/**
 * Static functions
 */
 
static int RunBlast (const char * const filsename_s, ParameterSet *param_set_p)
{
	int result = 0;
	
	
	return result;
}


static bool IsBlastableFile (const char * const filename_s)
{
	
}


static ParameterSet *GetBlastParameters (void)
{
	
}
