/*
 * protein_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#include "blastp_service.h"

#include <string.h>

#include "blast_service.h"
#include "blast_service_params.h"
#include "blast_app_parameters.h"
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


static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *GetProteinBlastServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);


static bool AddScoringParameters (BlastServiceData *data_p, ParameterSet *param_set_p);

static bool AddMatrixParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);

static bool AddCompositionalAdjustmentsParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);

static bool AddProteinGeneralAlgorithmParameters (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);



/*******************************/
/******* API DEFINITIONS *******/
/*******************************/


Service *GetBlastPService ()
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


ParameterSet *CreateProteinBlastServiceParameters (Service *service_p, const char *param_set_name_s, const char *param_set_description_s, AddAdditionalParamsFn query_sequence_callback_fn, const BlastTask *tasks_p, const uint32 num_tasks)
{
	ParameterSet *param_set_p = AllocateParameterSet (param_set_name_s, param_set_description_s);

	if (param_set_p)
		{
			if (AddBaseBlastServiceParameters (service_p, param_set_p, DT_PROTEIN, query_sequence_callback_fn))
				{
          BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

					if (AddGeneralAlgorithmParams (blast_data_p, param_set_p, AddProteinGeneralAlgorithmParameters))
						{
							if (AddProteinBlastParameters (blast_data_p, param_set_p))
								{
								  if (AddProgramSelectionParameters (blast_data_p, param_set_p, tasks_p, num_tasks))
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




bool AddProteinBlastParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	if (AddScoringParameters (data_p, param_set_p))
		{
			success_flag = true;
		}

	return success_flag;
}


bool ParseBlastPParameters (const BlastServiceData *data_p, ParameterSet *params_p, ArgsProcessor *ap_p)
{
	bool success_flag = false;

	/* matrix */
	if (GetAndAddBlastArgs (params_p, S_MATRIX.npt_name_s, false, ap_p))
		{
			/* Word Size */
			if (GetAndAddBlastArgs (params_p, S_COMP_BASED_STATS.npt_name_s, false, ap_p))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_COMP_BASED_STATS.npt_name_s);
				}

		}		/* if (GetAndAddBlastArgs (params_p, S_MATRIX.npt_name_s, false, ap_p)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_MATRIX.npt_name_s);
		}

	return success_flag;
}


static const char *GetProteinBlastServiceName (Service * UNUSED_PARAM (service_p))
{
 	return "BlastP service";
}


static const char *GetProteinBlastServiceDescription (Service * UNUSED_PARAM (service_p))
{
	return "A service to search protein databases with protein queries";
}



static ParameterSet *GetProteinBlastServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	return CreateProteinBlastServiceParameters (service_p, "Protein Blast service parameters", "A service to run Protein Blast searches", NULL, s_tasks_p, S_NUM_TASKS);
}





static bool AddProteinGeneralAlgorithmParameters (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p)
{
	bool success_flag = false;
	SharedType def;
	Parameter *param_p;

	def.st_ulong_value = 3;

	if ((param_p = EasyCreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, group_p, S_WORD_SIZE.npt_type, S_WORD_SIZE.npt_name_s, "Word size", "Expected number of chance matches in a random model", def, PL_ALL)) != NULL)
		{
			success_flag = true;
		}

	return success_flag;
}


static ServiceJobSet *RunProteinBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	BlastAppParameters app_params;
	ServiceJobSet *jobs_p = NULL;

	app_params.bap_parse_params_fn = ParseBlastPParameters;

	jobs_p = RunBlastService (service_p, param_set_p, user_p, providers_p, &app_params);

	return jobs_p;
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
	const char *descriptions_ss [] =
		{
			"No composition-based statistics",
			"Composition-based statistics as in NAR 29:2994-3005, 2001",
			"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, conditioned on sequence properties",
			"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, unconditionally"
		};

	for (i = 0; i < S_NUM_COMP_BASED_STATS; ++ i)
		{
			(comp_values_p + i) -> st_ulong_value = i;
		}

	memset (&def, 0, sizeof (SharedType));

	options_p = AllocateParameterMultiOptionArray (S_NUM_COMP_BASED_STATS, descriptions_ss, comp_values_p, PT_UNSIGNED_INT, true);

	if (options_p)
		{
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


