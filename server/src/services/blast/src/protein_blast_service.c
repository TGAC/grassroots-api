/*
 * protein_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#include <string.h>

#include "protein_blast_service.h"
#include "base_blast_service.h"
#include "blast_service_params.h"
#include "blastp_app_parameters.hpp"
#include "blast_util.h"


/*******************************/
/***** STATIC DECLARATIONS *****/
/*******************************/


static NamedParameterType S_WORD_SIZE = { "word_size", PT_UNSIGNED_INT };
static NamedParameterType S_MATRIX = { "matrix", PT_STRING };
static NamedParameterType S_COMP_BASED_STATS = { "comp_based_stats", PT_UNSIGNED_INT };


static const size_t S_NUM_TASKS = 3;
static const BlastTask s_tasks_p [S_NUM_TASKS] =
{
  { "blastp", "blastp: Traditional BLASTP to compare a protein query to a protein database" },
  { "blastp-short", "blastp-short: BLASTP optimized for queries shorter than 30 residues" },
  { "blastp-fast", "blastp-fast: BLASTP optimized for faster runtime" }
};



static const uint32 S_NUM_MATRICES = 8;

static const char *S_MATRICES_SS [S_NUM_MATRICES] =
{
	"PAM30",
	"PAM70",
	"PAM250",
	"BLOSUM80",
	"BLOSUM62",
	"BLOSUM45",
	"BLOSUM50",
	"BLOSUM90"
};



static const uint32 S_NUM_COMP_BASED_STATS = 4;


static const char *GetProteinBlastServiceName (Service *service_p);

static const char *GetProteinBlastServiceDescription (Service *service_p);

static ParameterSet *GetProteinBlastServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static bool AddProteinBlastParameters (BlastServiceData *data_p, ParameterSet *param_set_p);

static bool AddScoringParameters (BlastServiceData *data_p, ParameterSet *param_set_p);

static bool AddMatrixParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);

static bool AddCompositionalAdjustmentsParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);

/*******************************/
/******* API DEFINITIONS *******/
/*******************************/


Service *GetProteinBlastService ()
{
	Service *protein_blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (protein_blast_service_p)
		{
			BlastServiceData *data_p = AllocateBlastServiceData (protein_blast_service_p, DT_PROTEIN);

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
							if (AddProteinBlastParameters (blast_data_p, param_set_p))
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
	BlastPAppParameters app_params;

	ServiceJobSet *jobs_p = RunBlastService (service_p, param_set_p, user_p, providers_p, &app_params);

	return jobs_p;
}




bool ParseProteinBlastParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p)
{
	bool success_flag = false;


	/* matrix */
	if (GetAndAddBlastArgsToByteBuffer (params_p, S_MATRIX.npt_name_s, false, buffer_p))
		{
			/* Word Size */
			if (GetAndAddBlastArgsToByteBuffer (params_p, S_COMP_BASED_STATS.npt_name_s, false, buffer_p))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_COMP_BASED_STATS.npt_name_s);
				}


		}		/* if (BPAP_COMP_BASED_STATS (params_p, S_MATRIX.npt_name_s, "-reward", buffer_p, false)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_MATRIX.npt_name_s);
		}

	return success_flag;
}



static bool AddProteinBlastParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	if (AddScoringParameters (data_p, param_set_p))
		{
			success_flag = true;
		}

	return success_flag;
}



static bool AddScoringParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	ParameterGroup *group_p = CreateAndAddParameterGroupToParameterSet ("Scoring Parameters", NULL, & (data_p -> bsd_base_data), param_set_p);

	if (AddMatrixParameter (data_p, param_set_p, group_p))
		{
			if (AddCompositionalAdjustmentsParameter (data_p, param_set_p, group_p))
				{
					success_flag = true;
				}		/* if (AddCompositionalAdjustmentsParameter (data_p, param_set_p, group_p)) */
			else
				{

				}
		}		/* if (AddMatrixParameter (data_p, param_set_p, group_p)) */
	else
		{

		}

	return success_flag;
}



static bool AddMatrixParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	SharedType matrix_values_p [S_NUM_MATRICES];
	uint32 i;
	ParameterMultiOptionArray *options_p = NULL;

	for (i = 0; i < S_NUM_MATRICES; ++ i)
		{
			(matrix_values_p + i) -> st_string_value_s = (char *) * (S_MATRICES_SS + i);
		}

	memset (&def, 0, sizeof (SharedType));

	options_p = AllocateParameterMultiOptionArray (S_NUM_MATRICES, NULL, matrix_values_p, PT_STRING, true);

	if (options_p)
		{
			/* set BLOSUM62 as default */
			def.st_string_value_s = (matrix_values_p + 4) -> st_string_value_s;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, group_p, S_MATRIX.npt_type, false, S_MATRIX.npt_name_s, "Matrix", "The Scoring matrix to use", options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					success_flag = true;
				}
			else
				{
					FreeParameterMultiOptionArray (options_p);
				}
		}

	return success_flag;
}



static bool AddCompositionalAdjustmentsParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	SharedType comp_values_p [S_NUM_COMP_BASED_STATS];
	uint32 i;
	ParameterMultiOptionArray *options_p = NULL;

	for (i = 0; i < S_NUM_COMP_BASED_STATS; ++ i)
		{
			(comp_values_p + i) -> st_ulong_value = i;
		}

	memset (&def, 0, sizeof (SharedType));

	options_p = AllocateParameterMultiOptionArray (S_NUM_COMP_BASED_STATS, NULL, comp_values_p, PT_UNSIGNED_INT, true);

	if (options_p)
		{
			const char *descriptions_ss [] =
				{
					"No composition-based statistics",
					"Composition-based statistics as in NAR 29:2994-3005, 2001",
					"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, conditioned on sequence properties",
					"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, unconditionally"
				};

			def.st_ulong_value = 2;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, group_p, S_COMP_BASED_STATS.npt_type, false, S_COMP_BASED_STATS.npt_name_s, "Compositional adjustments", "Matrix adjustment method to compensate for amino acid composition of sequences.", options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					success_flag = true;
				}
			else
				{
					FreeParameterMultiOptionArray (options_p);
				}
		}

	return success_flag;
}


