/*
 * nucleotide_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#include "nucleotide_blast_service.h"
#include "base_blast_service.h"
#include "blast_service_params.h"

/*******************************/
/***** STATIC DECLARATIONS *****/
/*******************************/

static const size_t S_NUM_TASKS = 6;

static const BlastTask s_tasks_p [S_NUM_TASKS] =
{
  { "megablast", "Traditional megablast used to find very similar (e.g., intraspecies or closely related species) sequences" },
  { "blastn", "Traditional BLASTN requiring an exact match of 11" },
  { "blastn-short", "BLASTN program optimized for sequences shorter than 50 bases" },
  { "rmblastn", "BLASTN with complexity adjusted scoring and masklevel" },
  { "dc-megablast", "Discontiguous megablast used to find more distant (e.g., interspecies) sequences" },
  { "megablast", "Traditional megablast used to find very similar (e.g., intraspecies or closely related species) sequences" }
};


static const char *GetNucleotideBlastServiceName (Service *service_p);

static const char *GetNucleotideBlastServiceDescription (Service *service_p);

static ParameterSet *GetNucleotideBlastServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static ServiceJobSet *RunNucleotideBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


/*******************************/
/******* API DEFINITIONS *******/
/*******************************/


Service *GetNucleotideBlastService ()
{
	Service *nucleotide_blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (nucleotide_blast_service_p)
		{
			BlastServiceData *data_p = AllocateBlastServiceData (nucleotide_blast_service_p);

			if (data_p)
				{
					InitialiseService (nucleotide_blast_service_p,
														 GetNucleotideBlastServiceName,
														 GetNucleotideBlastServiceDescription,
														 NULL,
														 RunNucleotideBlastService,
														 IsResourceForBlastService,
														 GetNucleotideBlastServiceParameters,
														 ReleaseBlastServiceParameters,
														 CloseBlastService,
														 CustomiseBlastServiceJob,
														 true,
														 true,
														 (ServiceData *) data_p);


					if (GetBlastServiceConfig (data_p))
						{
							return nucleotide_blast_service_p;
						}
				}

			FreeService (nucleotide_blast_service_p);
		}

	return NULL;
}



static const char *GetNucleotideBlastServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Nucleotide Blast service";
}


static const char *GetNucleotideBlastServiceDescription (Service * UNUSED_PARAM (service_p))
{
	return "A service to run Blast nucleotide searches";
}


static ParameterSet *GetNucleotideBlastServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Nucleotide Blast service parameters", "A service to run nucleotide Blast searches");

	if (param_set_p)
		{
			if (AddBaseBlastServiceParameters (service_p, param_set_p, DT_NUCLEOTIDE))
				{
				  BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

					if (AddGeneralAlgorithmParams (blast_data_p, param_set_p))
						{
							if (AddScoringParams (blast_data_p, param_set_p))
								{
								  if (AddProgramSelectionParameters (param_set_p, s_tasks_p, S_NUM_TASKS))
                    {
                      return param_set_p;
                    }
								}
						}
				}		/* if (AddBaseBlastServiceParameters (service_p, param_set_p, DT_NUCLEOTIDE)) */

			/*
			 * task: 'blastn' 'blastn-short' 'dc-megablast' 'megablast' 'rmblastn
			 */

		}		/* if (param_set_p) */


	return param_set_p;
}



static ServiceJobSet *RunNucleotideBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	ServiceJobSet *jobs_p = NULL;

	return jobs_p;
}


