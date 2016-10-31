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
#include <string.h>

#define ALLOCATE_BLAST_SERVICE_CONSTANTS (1)
#include "base_blast_service.h"

#include "blastn_service.h"
#include "blastp_service.h"
#include "blastx_service.h"

#include "memory_allocations.h"

#include "service_job_set_iterator.h"
#include "string_utils.h"
#include "grassroots_config.h"
#include "temp_file.hpp"
#include "json_tools.h"
#include "blast_formatter.h"
#include "blast_service_job.h"
#include "paired_blast_service.h"
#include "blast_service_params.h"
#include "blast_tool_factory.hpp"
#include "jobs_manager.h"
#include "blast_service_job.h"
#include "blast_service_params.h"

#include "servers_pool.h"
#include "remote_parameter_details.h"




/***************************************/

static void InitBlastService (Service *blast_service_p);



/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *  UNUSED_PARAM (config_p))
{
	ServicesArray *services_array_p = NULL;
	const uint32 NUM_SERVICES = 3;
	Service *services_pp [NUM_SERVICES];
	uint32 num_added_services = 0;
	uint32 i;

	memset (services_pp, 0, NUM_SERVICES * (sizeof (Service *)));

	*services_pp = GetBlastNService ();
	* (services_pp + 1) = GetBlastPService ();
	* (services_pp + 2) = GetBlastXService ();


	/*
	 * Loop through all of the Blast services
	 * and try and initialise them
	 */
	for (i = 0; i < NUM_SERVICES; ++ i)
		{
			Service *service_p = * (services_pp + i);

			if (service_p)
				{
					InitBlastService (service_p);
					 ++ num_added_services;
				}
		}

	/*
	 * If we have successfully got any Blast services,
	 * create and populate the ServicesArray that will
	 * contain them
	 */
	if (num_added_services > 0)
		{
			services_array_p = AllocateServicesArray (num_added_services);

			if (services_array_p)
				{
					Service **added_service_pp = services_array_p -> sa_services_pp;

					for (i = 0; i < NUM_SERVICES; ++ i)
						{
							if (* (services_pp + i))
								{
									*added_service_pp = * (services_pp + i);
									++ added_service_pp;
								}
						}

				}		/* if (services_array_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create ServicesArray for Blast services");

					for (i = 0; i < NUM_SERVICES; ++ i)
						{
							if (* (services_pp + i))
								{
									FreeService (* (services_pp + i));
								}
						}

				}

		}		/* if (num_added_services > 0) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create any Blast services");
		}

	return services_array_p;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */


static void InitBlastService (Service *blast_service_p)
{
	blast_service_p -> se_synchronous_flag = IsBlastToolFactorySynchronous (((BlastServiceData *) (blast_service_p -> se_data_p)) -> bsd_tool_factory_p);

	blast_service_p -> se_deserialise_job_json_fn = BuildBlastServiceJob;
	blast_service_p -> se_serialise_job_json_fn = BuildBlastServiceJobJSON;

	blast_service_p -> se_get_value_from_job_fn = GetValueFromBlastServiceJobOutput;
}



void ReleaseBlastServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}



