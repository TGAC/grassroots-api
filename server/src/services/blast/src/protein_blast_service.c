/*
 * protein_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#include "protein_blast_service.h"
#include "base_blast_service.h"
#include "blast_service_params.h"


/*******************************/
/***** STATIC DECLARATIONS *****/
/*******************************/

static const size_t S_NUM_TASKS = 3;
static const BlastTask s_tasks_p [S_NUM_TASKS] =
{
  { "blastp", "blastp: Traditional BLASTP to compare a protein query to a protein database" },
  { "blastp-short", "blastp-short: BLASTP optimized for queries shorter than 30 residues" },
  { "blastp-fast", "blastp-fast: BLASTP optimized for faster runtime" }
};


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
	ParameterSet *param_set_p = AllocateParameterSet ("Protein Blast service parameters", "A service to run Protein Blast searches");

	if (param_set_p)
		{
			if (AddBaseBlastServiceParameters (service_p, param_set_p, DT_PROTEIN))
				{
          BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

					if (AddGeneralAlgorithmParams (blast_data_p, param_set_p))
						{
							if (AddScoringParams (blast_data_p, param_set_p))
								{
								  if (AddProgramSelectionParameters (blast_data_p, param_set_p, s_tasks_p, S_NUM_TASKS))
                    {
                      return param_set_p;
                    }
								}
						}
				}		/* if (AddBaseBlastServiceParameters (service_p, param_set_p, DT_PROTEIN)) */

			/*
			 * task: 'blastn' 'blastn-short' 'dc-megablast' 'megablast' 'rmblastn
			 */

		}		/* if (param_set_p) */


	return param_set_p;
}



static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	ServiceJobSet *jobs_p = NULL;

	return jobs_p;
}



