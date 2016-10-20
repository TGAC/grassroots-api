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

#include "nucleotide_blast_service.h"
#include "protein_blast_service.h"

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

static bool InitBlastService (Service *blast_service_p, ServicesArray *services_p, size_t *index_p);



/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *  UNUSED_PARAM (config_p))
{
	ServicesArray *services_p = NULL;
	Service *nucleotide_service_p = GetNucleotideBlastService ();
	Service *protein_service_p = GetProteinBlastService ();
	size_t num_services = 0;

	if (nucleotide_service_p)
		{
			++ num_services;
		}

	if (protein_service_p)
		{
			++ num_services;
		}


	services_p = AllocateServicesArray (num_services);

	if (services_p)
		{
			bool success_flag = true;
			size_t i = 0;

			if (nucleotide_service_p && success_flag)
				{
					success_flag = InitBlastService (nucleotide_service_p, services_p, &i);
				}

			if (protein_service_p && success_flag)
				{
					success_flag = InitBlastService (protein_service_p, services_p, &i);
				}

			if (!success_flag)
				{

				}

		}		/* if (services_p) */
	else
		{

			if (nucleotide_service_p)
				{
					FreeService (nucleotide_service_p);
				}

			if (protein_service_p)
				{
					FreeService (protein_service_p);
				}
		}

	return services_p;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */


static bool InitBlastService (Service *blast_service_p, ServicesArray *services_p, size_t *index_p)
{
	bool success_flag = true;

	blast_service_p -> se_synchronous_flag = IsBlastToolFactorySynchronous (((BlastServiceData *) (blast_service_p -> se_data_p)) -> bsd_tool_factory_p);

	blast_service_p -> se_deserialise_job_json_fn = BuildBlastServiceJob;
	blast_service_p -> se_serialise_job_json_fn = BuildBlastServiceJobJSON;

	blast_service_p -> se_get_value_from_job_fn = GetValueFromBlastServiceJobOutput;

	if (*index_p < services_p -> sa_num_services)
		{
			* ((services_p -> sa_services_pp) + (*index_p)) = blast_service_p;
			++ (*index_p);
		}
	else
		{
			success_flag = false;
		}

	return success_flag;
}



void ReleaseBlastServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}



