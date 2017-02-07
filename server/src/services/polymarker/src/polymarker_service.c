/*
** Copyright 2014-2016 The Earlham Institute
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
#include <time.h>


#define ALLOCATE_POLYMARKER_TAGS (1)
#include "polymarker_service.h"

#include "memory_allocations.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "byte_buffer.h"



#ifdef _DEBUG
	#define POLYMARKER_SERVICE_DEBUG	(STM_LEVEL_FINEST)
#else
	#define POLYMARKER_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


/*
 * bin/polymarker.rb --contigs ~/Applications/grassroots-0/grassroots/extras/blast/databases/Triticum_aestivum_CS42_TGACv1_all.fa --marker_list test/data/short_primer_design_test.csv --output ~/Desktop/polymarker_out
 */






/*
 * STATIC PROTOTYPES
 */

static PolymarkerServiceData *AllocatePolymarkerServiceData (Service *service_p);

static void FreePolymarkerServiceData (PolymarkerServiceData *data_p);

static const char *GetPolymarkerServiceName (Service *service_p);

static const char *GetPolymarkerServiceDesciption (Service *service_p);

static ParameterSet *GetPolymarkerServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleasePolymarkerServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunPolymarkerService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static  ParameterSet *IsFileForPolymarkerService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool ClosePolymarkerService (Service *service_p);


static bool GetPolymarkerServiceConfig (PolymarkerServiceData *data_p);


static void CustomisePolymarkerServiceJob (Service * UNUSED_PARAM (service_p), ServiceJob *job_p);


static bool UpdatePolymarkerServiceJob (ServiceJob *job_p);


static void FreePolymarkerServiceJob (ServiceJob *job_p);



/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t * UNUSED_PARAM (config_p))
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					PolymarkerServiceData *data_p = AllocatePolymarkerServiceData (service_p);
					
					if (data_p)
						{

							InitialiseService (service_p,
								GetPolymarkerServiceName,
								GetPolymarkerServiceDesciption,
								NULL,
								RunPolymarkerService,
								IsFileForPolymarkerService,
								GetPolymarkerServiceParameters,
								ReleasePolymarkerServiceParameters,
								ClosePolymarkerService,
								CustomisePolymarkerServiceJob,
								true,
								false,
								(ServiceData *) data_p);
							
							if (GetPolymarkerServiceConfig (data_p))
								{
									* (services_p -> sa_services_pp) = service_p;

									return services_p;
								}

						}

					FreeServicesArray (services_p);
				}

			FreeService (service_p);
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
 

static bool GetPolymarkerServiceConfig (PolymarkerServiceData *data_p)
{
	bool success_flag = true;
	const json_t *polymarker_config_p = data_p -> psd_base_data.sd_config_p;

	if (polymarker_config_p)
		{
			const char *tool_s = GetJSONString (polymarker_config_p, "tool");

			if (tool_s)
				{
					if (strcmp (tool_s, "web") == 0)
						{
							data_p -> psd_tool_type = PTT_WEB;
						}
				}

		}		/* if (polymarker_config_p) */

	return success_flag;
}



static PolymarkerServiceData *AllocatePolymarkerServiceData (Service * UNUSED_PARAM (service_p))
{
	PolymarkerServiceData *data_p = (PolymarkerServiceData *) AllocMemory (sizeof (PolymarkerServiceData));

	data_p -> psd_tool_type = PTT_SYSTEM;

	return data_p;
}


static void FreePolymarkerServiceData (PolymarkerServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool ClosePolymarkerService (Service *service_p)
{
	FreePolymarkerServiceData ((PolymarkerServiceData *) (service_p -> se_data_p));

	return true;
}
 
 
static const char *GetPolymarkerServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Polymarker service";
}


static const char *GetPolymarkerServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service using Polymarker";
}


static ParameterSet *GetPolymarkerServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Polymarker service parameters", "The parameters used for the Polymarker service");
	
	if (param_set_p)
		{
			PolymarkerServiceData *data_p = (PolymarkerServiceData *) (service_p -> se_data_p);
			Parameter *param_p = NULL;
			SharedType def;

			def.st_string_value_s = NULL;

			if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_CONTIG_FILENAME.npt_type, PS_CONTIG_FILENAME.npt_name_s, "Reference", "The reference database to use", def, PL_ALL)) != NULL)
				{
					if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_GENE_ID.npt_type, PS_GENE_ID.npt_name_s, "Gene ID", "An unique identifier for the assay", def, PL_ALL)) != NULL)
						{
							if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_TARGET_CHROMOSOME.npt_type, PS_TARGET_CHROMOSOME.npt_name_s, "Target Chromosome", "The chromosome to use", def, PL_ALL)) != NULL)
								{
									if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_SEQUENCE.npt_type, PS_SEQUENCE.npt_name_s, "Sequence surrounding the polymorphisms", "The SNP must be marked in the format [A/T] for a varietal SNP with alternative bases, A or T",  def, PL_ALL)) != NULL)
										{
											return param_set_p;
										}
								}
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleasePolymarkerServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunPolymarkerService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	PolymarkerServiceData *data_p = (PolymarkerServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Polymarker service job");


	return service_p -> se_jobs_p;
}


static  ParameterSet *IsFileForPolymarkerService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}


static void CustomisePolymarkerServiceJob (Service * UNUSED_PARAM (service_p), ServiceJob *job_p)
{
	job_p -> sj_update_fn = UpdatePolymarkerServiceJob;
	job_p -> sj_free_fn = FreePolymarkerServiceJob;
}





static bool UpdatePolymarkerServiceJob (ServiceJob *job_p)
{
	bool success_flag = false;

	return success_flag;
}


static void FreePolymarkerServiceJob (ServiceJob *job_p)
{

	FreeServiceJob (job_p);
}

