#include <string.h>

#include "blast_service.h"
#include "memory_allocations.h"

#include "blast_tool.hpp"

/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData bsd_base_data;
	BlastTool *bsd_blast_tool_p;
} BlastServiceData;



/*
 * STATIC PROTOTYPES
 */

static BlastServiceData *AllocateBlastServiceData (Service *blast_service_p);

static void FreeBlastServiceData (BlastServiceData *data_p);

static const char *GetBlastServiceName (Service *service_p);

static const char *GetBlastServiceDesciption (Service *service_p);

static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseBlastService (Service *service_p);

static bool AddQuerySequenceParams (ParameterSet *param_set_p);

static bool AddGeneralAlgorithmParams (ParameterSet *param_set_p);

static bool AddScoringParams (ParameterSet *param_set_p);

static bool AddDatabaseParams (ParameterSet *param_set_p);

/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *config_p)
{
	Service *blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (blast_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = (ServiceData *) AllocateBlastServiceData (blast_service_p);
					
					if (data_p)
						{
							InitialiseService (blast_service_p,
								GetBlastServiceName,
								GetBlastServiceDesciption,
								NULL,
								RunBlastService,
								IsFileForBlastService,
								GetBlastServiceParameters,
								ReleaseBlastServiceParameters,
								CloseBlastService,
								NULL,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = blast_service_p;

							return services_p;
						}

					FreeServicesArray (services_p);
				}

			FreeService (blast_service_p);
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
 

static BlastServiceData *AllocateBlastServiceData (Service *blast_service_p)
{
	BlastServiceData *data_p = (BlastServiceData *) AllocMemory (sizeof (BlastServiceData));

	if (data_p)
		{
			data_p -> bsd_blast_tool_p = CreateBlastTool (blast_service_p);

			if (data_p -> bsd_blast_tool_p)
				{
					return data_p;
				}

			FreeMemory (data_p);
		}

	return NULL;
}


static void FreeBlastServiceData (BlastServiceData *data_p)
{
	FreeBlastTool (data_p -> bsd_blast_tool_p);
	FreeMemory (data_p);
}

 
static bool CloseBlastService (Service *service_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

	FreeBlastServiceData (blast_data_p);

	return true;
}
 
 
static const char *GetBlastServiceName (Service *service_p)
{
	return "Blast service";
}


static const char *GetBlastServiceDesciption (Service *service_p)
{
	return "A service to run the Blast program";
}


/*
 * The list of databases that can be searched
 */
static bool AddDatabaseParams (ParameterSet *param_set_p)
{
	bool success_flag = false;

	return success_flag;
}

static bool AddQuerySequenceParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 5;
	const Parameter **grouped_params_pp = (const Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	const Parameter **grouped_param_pp = grouped_params_pp;

	def.st_string_value_s = NULL;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, "input", "Input", "The input file to read", TAG_BLAST_INPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
		{
			def.st_string_value_s = NULL;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, "output", "Output", "The output file to write", TAG_BLAST_OUTPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					def.st_string_value_s = NULL;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "query_sequence", "Query Sequence(s)", "Query sequence(s) to be used for a BLAST search should be pasted in the 'Search' text area. "
					  "It accepts a number of different types of input and automatically determines the format or the input."
					  " To allow this feature there are certain conventions required with regard to the input of identifiers (e.g., accessions or gi's)", TAG_BLAST_INPUT_QUERY, NULL, def, NULL, NULL, PL_ALL, NULL))  != NULL)
						{
							const char *subrange_s = "Coordinates for a subrange of the query sequence. The BLAST search will apply only to the residues in the range. Valid sequence coordinates are from 1 to the sequence length. Set either From or To to 0 to ignore the range. The range includes the residue at the To coordinate.";

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							def.st_ulong_value = 0;

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "from", "From", subrange_s, TAG_BLAST_SUBRANGE_FROM, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
								{
									def.st_ulong_value = 0;

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "to", "To", subrange_s, TAG_BLAST_SUBRANGE_TO, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
										{
											const char * const group_name_s = "Query Sequence Parameters";

											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
												{
													PrintErrors (STM_LEVEL_WARNING, "Failed to add %s grouping", group_name_s);
													FreeMemory (grouped_params_pp);
												}

											success_flag = true;
										}
								}
						}
				}
		}

	return success_flag;
}


static bool AddGeneralAlgorithmParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 5;
	const Parameter **grouped_params_pp = (const Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	const Parameter **grouped_param_pp = grouped_params_pp;
	uint8 level = PL_INTERMEDIATE | PL_ALL;

	def.st_ulong_value = 100;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "max_target_sequences", "Max target sequences", "Select the maximum number of aligned sequences to display (the actual number of alignments may be greater than this)." , TAG_BLAST_MAX_SEQUENCES, NULL, def, NULL, NULL, level, NULL)) != NULL)
		{
			def.st_boolean_value = true;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}


			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_BOOLEAN, "short_queries", "Short queries", "Automatically adjust parameters for short input sequences", TAG_BLAST_SHORT_QUERIES, NULL, def, NULL, NULL, level, NULL)) != NULL)
				{
					def.st_data_value = 10.0;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_REAL, "expect_threshold", "Expect threshold", "Expected number of chance matches in a random model" , TAG_BLAST_EXPECT_THRESHOLD, NULL, def, NULL, NULL, level, NULL)) != NULL)
						{
							def.st_ulong_value = 28;

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "word_size", "Word size", "Expected number of chance matches in a random model", TAG_BLAST_WORD_SIZE, NULL, def, NULL, NULL, level, NULL)) != NULL)
								{
									def.st_ulong_value = 0;

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "max_matches_in_a_query_range", "Max matches in a query range", "Limit the number of matches to a query range. This option is useful if many strong matches to one part of a query may prevent BLAST from presenting weaker matches to another part of the query", TAG_BLAST_WORD_SIZE, NULL, def, NULL, NULL, level, NULL)) != NULL)
										{
											const char * const group_name_s = "General Algorithm Parameters";

											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
												{
													PrintErrors (STM_LEVEL_WARNING, "Failed to add %s grouping", group_name_s);
													FreeMemory (grouped_params_pp);
												}

											success_flag = true;
										}
								}
						}
				}
		}

	return success_flag;
}


static bool AddScoringParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 2;
	const Parameter **grouped_params_pp = (const Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	const Parameter **grouped_param_pp = grouped_params_pp;
	const ParameterLevel level = PL_INTERMEDIATE | PL_ADVANCED;

	def.st_long_value = 2;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_SIGNED_INT, "match", "Match", "Reward for a nucleotide match.", TAG_BLAST_MATCH_SCORE, NULL, def, NULL, NULL, level, NULL)) != NULL)
		{
			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			def.st_long_value = -3;

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_SIGNED_INT, "mismatch", "Mismatch", "Penalty for a nucleotide mismatch.", TAG_BLAST_MISMATCH_SCORE, NULL, def, NULL, NULL, level, NULL)) != NULL)
				{
					const char * const group_name_s = "Scoring Parameters";

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to add %s grouping", group_name_s);
							FreeMemory (grouped_params_pp);
						}

					success_flag = true;
				}
		}


	return success_flag;
}


static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *config_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Blast service parameters", "The parameters used for the Blast service");
	
	if (param_set_p)
		{
			if (AddQuerySequenceParams (param_set_p))
				{
					if (AddGeneralAlgorithmParams (param_set_p))
						{
							if (AddScoringParams (param_set_p))
								{
									return param_set_p;
								}
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	BlastTool *tool_p = blast_data_p -> bsd_blast_tool_p;
	
	if (tool_p -> ParseParameters (param_set_p))
		{
			if (RunBlast (tool_p))
				{
					const char *result_s = tool_p -> GetResults ();

					if (result_s)
						{
							json_error_t error;

							res_json_p = json_pack_ex (&error, 0, "{s:s}", "results", result_s);

							if (res_json_p)
								{
									res = OS_SUCCEEDED;
								}
						}
				}
			else
				{
					res = OS_FAILED;
				}
		}
		
	res_json_p = CreateServiceResponseAsJSON (service_p, res, res_json_p);
		
	return res_json_p;
}


static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;
	const char *filename_s = resource_p -> re_value_s;
	
	
	/*
	 * @TODO
	 * We could check if the file is on a remote filesystem and if so
	 * make a full or partial local copy for analysis.
	 */
	
	/* 
	 * We can check on file extension and also the content of the file
	 * to determine if we want to blast this file.
	 */
	if (filename_s)
		{
			const char *extension_s = strstr (filename_s, ".");
			
			if (extension_s)
				{
					/* move past the . */
					++ extension_s;
					
					/* check that the file doesn't end with the . */
					if (*extension_s != '\0')
						{
							if (strcmp (extension_s, "fa") == 0)
								{
									interested_flag = true;
								}
								
						}		/* if (*extension_s != '\0') */
					
				}		/* if (extension_s) */
				
		}		/* if (filename_s) */
	
	
	return interested_flag;	
}

