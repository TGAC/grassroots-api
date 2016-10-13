/*
 * protein_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#include "protein_blast_service.h"
#include "base_blast_service.h"


/*******************************/
/***** STATIC DECLARATIONS *****/
/*******************************/


static const char *GetProteinBlastServiceName (Service *service_p);

static const char *GetProteinBlastServiceDescription (Service *service_p);

static ParameterSet *GetProteinBlastServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


/*******************************/
/******* API DEFINITIONS *******/
/*******************************/


Service *GetProteinBlastService ()
{
	Service *protein_blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (protein_blast_service_p)
		{
			BlastServiceData *data_p = AllocateBlastServiceData (protein_blast_service_p);

			if (data_p)
				{
					InitialiseService (protein_blast_service_p,
														 GetProteinBlastServiceName,
														 GetProteinBlastServiceDescription,
														 NULL,
														 RunProteinBlastService,
														 IsResourceForBlastService,
														 GetProteinBlastServiceParameters,
														 ReleaseBlastServiceParameters,
														 CloseBlastService,
														 CustomiseBlastServiceJob,
														 true,
														 true,
														 (ServiceData *) data_p);

					if (GetBlastServiceConfig (data_p))
						{
							return protein_blast_service_p;
						}

					return protein_blast_service_p;
				}

			FreeService (protein_blast_service_p);
		}

	return NULL;
}



static const char *GetProteinBlastServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Protein Blast service";
}


static const char *GetProteinBlastServiceDescription (Service * UNUSED_PARAM (service_p))
{
	return "A service to run Blast Protein searches";
}


static ParameterSet *GetProteinBlastServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *params_p = AllocateParameterSet ("Protein Blast service parameters", "A service to run Protein Blast searches");

	if (params_p)
		{
			if (AddBaseBlastServiceParameters (service_p, params_p, DT_PROTEIN))
				{

				}		/* if (AddBaseBlastServiceParameters (service_p, params_p, DT_PROTEIN)) */

			/*
			 * task: 'blastn' 'blastn-short' 'dc-megablast' 'megablast' 'rmblastn
			 */

		}		/* if (params_p) */


	return params_p;
}




static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	ServiceJobSet *jobs_p = NULL;

	return jobs_p;
}



