/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
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
