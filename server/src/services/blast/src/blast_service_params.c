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

/*
 * blast_service_params.c
 *
 *  Created on: 13 Feb 2016
 *      Author: billy
 */

#include <limits.h>
#include <string.h>

#include "blast_service_params.h"
#include "provider.h"
#include "grassroots_config.h"
#include "string_utils.h"
#include "streams.h"
#include "service.h"




static const char *s_output_formats_ss [BOF_NUM_TYPES] =
{
	"pairwise",
	"query-anchored showing identities",
	"query-anchored no identities",
	"flat query-anchored, show identities",
	"flat query-anchored, no identities",
	"XML Blast output",
	"tabular",
	"tabular with comment lines",
	"Text ASN.1",
	"Binary ASN.1",
	"Comma-separated values",
	"BLAST archive format (ASN.1)",
	"JSON Seqalign output",
	"Multiple file JSON Blast output",
	"Multiple file XML2 Blast output",
	"Single file JSON Blast output",
	"Single file XML2 Blast output",
	"Sequence Alignmnent/Map (SAM)"
};


/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/

uint32 GetNumberOfDatabases (const BlastServiceData *data_p, const DatabaseType dt)
{
	uint32 i = 0;
	const DatabaseInfo *db_p = data_p -> bsd_databases_p;

	if (db_p)
		{
			while (db_p -> di_name_s)
				{
					if (db_p -> di_type == dt)
						{
							++ i;
						}

					++ db_p;
				}
		}

	return i;
}


char *CreateGroupName (const char *server_s)
{
	char *group_name_s = ConcatenateVarargsStrings (BS_DATABASE_GROUP_NAME_S, " provided by ", server_s, NULL);

	if (group_name_s)
		{
			#if PAIRED_BLAST_SERVICE_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Created group name \"%s\" for \"%s\" and \"%s\"", group_name_s, server_s);
			#endif
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create group name for \"%s\"", group_name_s);
		}

	return group_name_s;
}


/*
 * The list of databases that can be searched
 */
uint16 AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p, const DatabaseType db_type)
{
	uint16 num_added_databases = 0;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = GetNumberOfDatabases (data_p, db_type);

	if (num_group_params)
		{
			Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
			Parameter **grouped_param_pp = grouped_params_pp;
			const DatabaseInfo *db_p = data_p -> bsd_databases_p;
			char *group_s = NULL;
			const json_t *provider_p = NULL;
			const char *group_to_use_s = NULL;

			if (db_p)
				{
					while (db_p -> di_name_s)
						{
							if (db_p -> di_type == db_type)
								{
									/* try and get the local name of the database */
									const char *local_name_s = strrchr (db_p -> di_name_s, GetFileSeparatorChar ());

									def.st_boolean_value = db_p -> di_active_flag;

									if (local_name_s)
										{
											++ local_name_s;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_BOOLEAN, false, db_p -> di_name_s, db_p -> di_description_s, db_p -> di_name_s, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ALL, NULL)) != NULL)
										{
											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											++ num_added_databases;
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add database \"%s\"", db_p -> di_name_s);
										}

								}		/* if (db_p -> di_type == db_type) */

							++ db_p;

						}		/* while (db_p && success_flag) */

				}		/* if (db_p) */

			provider_p = GetGlobalConfigValue (SERVER_PROVIDER_S);

			if (provider_p)
				{
					const char *provider_s = GetProviderName (provider_p);

					if (provider_s)
						{
							group_s = CreateGroupName (provider_s);
						}
				}

			group_to_use_s = group_s ? group_s : BS_DATABASE_GROUP_NAME_S;

			if (!AddParameterGroupToParameterSet (param_set_p, group_to_use_s, NULL, grouped_params_pp, num_group_params, & (data_p -> bsd_base_data)))
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", BS_DATABASE_GROUP_NAME_S);
					FreeMemory (grouped_params_pp);
				}

			if (group_s)
				{
					FreeCopiedString (group_s);
				}

		}		/* if (num_group_params) */

	return num_added_databases;
}


Parameter *SetUpPreviousJobUUIDParamater (const BlastServiceData *service_data_p, ParameterSet *param_set_p)
{
	Parameter *param_p = NULL;
	SharedType def;

	memset (&def, 0, sizeof (def));

	param_p = CreateAndAddParameterToParameterSet (& (service_data_p -> bsd_base_data), param_set_p, BS_JOB_ID.npt_type, false, BS_JOB_ID.npt_name_s, "Job IDs", "The UUIDs for Blast jobs that have previously been run", NULL, def, NULL, NULL, PL_ALL, NULL);

	return param_p;
}


Parameter *SetUpOutputFormatParamater (const BlastServiceData *service_data_p,ParameterSet *param_set_p)
{
	Parameter *param_p = NULL;
	ParameterMultiOptionArray *options_p = NULL;
	SharedType values [BOF_NUM_TYPES];
	uint32 i;
	SharedType def;

	memset (&def, 0, sizeof (def));


	for (i = 0; i < BOF_NUM_TYPES; ++ i)
		{
			values [i].st_ulong_value = i;
		}

	options_p = AllocateParameterMultiOptionArray (BOF_NUM_TYPES, s_output_formats_ss, values, PT_UNSIGNED_INT, true);

	if (options_p)
		{
			/* default to  blast asn */
			def.st_ulong_value = BOF_BLAST_ASN1;

			param_p = CreateAndAddParameterToParameterSet (& (service_data_p -> bsd_base_data), param_set_p, BS_OUTPUT_FORMAT.npt_type, false, BS_OUTPUT_FORMAT.npt_name_s, "Output format", "The output format for the results", options_p, def, NULL, NULL, PL_ALL, NULL);

			if (!param_p)
				{
					FreeParameterMultiOptionArray (options_p);
				}
		}		/* if (options_p) */

	return param_p;
}


bool AddQuerySequenceParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 5;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;

	def.st_string_value_s = NULL;

	if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, BS_INPUT_FILE.npt_type, false, BS_INPUT_FILE.npt_name_s, "Input", "The input file to read", NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
		{
			def.st_string_value_s = NULL;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			if ((param_p = SetUpPreviousJobUUIDParamater (data_p, param_set_p)) != NULL)
				{
					def.st_string_value_s = NULL;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, BS_INPUT_QUERY.npt_type, false, BS_INPUT_QUERY.npt_name_s, "Query Sequence(s)", "Query sequence(s) to be used for a BLAST search should be pasted in the 'Search' text area. "
																															"It accepts a number of different types of input and automatically determines the format or the input."
																															" To allow this feature there are certain conventions required with regard to the input of identifiers (e.g., accessions or gi's)", NULL, def, NULL, NULL, PL_ALL, NULL))  != NULL)
						{
							const char *subrange_s = "Coordinates for a subrange of the query sequence. The BLAST search will apply only to the residues in the range. Valid sequence coordinates are from 1 to the sequence length. Set either From or To to 0 to ignore the range. The range includes the residue at the To coordinate.";

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							def.st_ulong_value = 0;

							if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, BS_SUBRANGE_FROM.npt_type, false, BS_SUBRANGE_FROM.npt_name_s, "From", subrange_s, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
								{
									def.st_ulong_value = 0;

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, BS_SUBRANGE_TO.npt_type, false, BS_SUBRANGE_TO.npt_name_s, "To", subrange_s, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
										{
											const char * const group_name_s = "Query Sequence Parameters";

											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, NULL, grouped_params_pp, num_group_params, & (data_p -> bsd_base_data)))
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
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


bool AddGeneralAlgorithmParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 6;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;
	uint8 level = PL_INTERMEDIATE | PL_ALL;
	const char *param_name_s = "max_target_sequences";
	ParameterType pt = PT_UNSIGNED_INT;

	def.st_ulong_value = 5;


	if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, pt, false, param_name_s, "Max target sequences", "Select the maximum number of aligned sequences to display (the actual number of alignments may be greater than this).", NULL, def, NULL, NULL, level, NULL)) != NULL)
		{
			def.st_boolean_value = true;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}


			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_BOOLEAN, false, "short_queries", "Short queries", "Automatically adjust parameters for short input sequences", NULL, def, NULL, NULL, level, NULL)) != NULL)
				{
					def.st_ulong_value = 10;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_UNSIGNED_INT, false, "expect_threshold", "Expect threshold", "Expected number of chance matches in a random model", NULL, def, NULL, NULL, level, NULL)) != NULL)
						{
							def.st_ulong_value = 28;

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_UNSIGNED_INT, false, "word_size", "Word size", "Expected number of chance matches in a random model", NULL, def, NULL, NULL, level, NULL)) != NULL)
								{
									def.st_ulong_value = 0;

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_UNSIGNED_INT, false, "max_matches_in_a_query_range", "Max matches in a query range", "Limit the number of matches to a query range. This option is useful if many strong matches to one part of a query may prevent BLAST from presenting weaker matches to another part of the query", NULL, def, NULL, NULL, level, NULL)) != NULL)
										{
											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}


											if ((param_p = SetUpOutputFormatParamater (data_p, param_set_p)) != NULL)
												{
													const char * const group_name_s = "General Algorithm Parameters";

													if (grouped_param_pp)
														{
															*grouped_param_pp = param_p;
															++ grouped_param_pp;
														}

													if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, NULL, grouped_params_pp, num_group_params, & (data_p -> bsd_base_data)))
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
															FreeMemory (grouped_params_pp);
														}

													success_flag = true;
												}
										}
								}
						}
				}
		}

	return success_flag;
}



bool AddProgramSelectionParameters (const BlastServiceData *blast_data_p, ParameterSet *param_set_p, const BlastTask *tasks_p, const size_t num_tasks)
{
  ParameterMultiOptionArray *options_p  = NULL;
  SharedType values_p [num_tasks];
  const char *descriptions_ss [num_tasks];
  size_t i;

  for (i = 0; i < num_tasks; ++ i)
    {
      (values_p + i) -> st_string_value_s = (char *)  (tasks_p + i) -> bt_name_s;
      * (descriptions_ss + i) = (tasks_p + i) -> bt_description_s;
    }

  options_p  = AllocateParameterMultiOptionArray (num_tasks, descriptions_ss, values_p, PT_STRING, true);

  if (options_p)
  	{
  		Parameter *param_p = NULL;
  		SharedType def;
  		const ParameterLevel level = PL_INTERMEDIATE | PL_ADVANCED;

  		def.st_string_value_s = (char *) tasks_p -> bt_name_s;

  		if ((param_p = CreateAndAddParameterToParameterSet (& (blast_data_p -> bsd_base_data), param_set_p, BS_TASK.npt_type, false, BS_TASK.npt_name_s, "Program Selection", "The program to use to run the search.", options_p, def, NULL, NULL, level, NULL)) != NULL)
  			{
  				return true;
  			}

  		FreeParameterMultiOptionArray (options_p);
  	}

	return false;
}


bool AddScoringParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 2;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;
	const ParameterLevel level = PL_INTERMEDIATE | PL_ADVANCED;
	ParameterBounds *bounds_p = AllocateParameterBounds ();

	/* Match must be positive */
	if (bounds_p)
		{
			bounds_p -> pb_lower.st_long_value = 1;
			bounds_p -> pb_upper.st_long_value = INT_MAX;
			def.st_long_value = 2;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_SIGNED_INT, false, "match", "Match", "Reward for a nucleotide match.", NULL, def, NULL, bounds_p, level, NULL)) != NULL)
				{
					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}


					/* Mismatch must be positive */
					bounds_p = AllocateParameterBounds ();

					if (bounds_p)
						{
							def.st_long_value = -3;
							bounds_p -> pb_lower.st_long_value = INT_MIN;
							bounds_p -> pb_upper.st_long_value = -1;

							if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, PT_SIGNED_INT, false, "mismatch", "Mismatch", "Penalty for a nucleotide mismatch.", NULL, def, NULL, bounds_p, level, NULL)) != NULL)
								{
									const char * const group_name_s = "Scoring Parameters";

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, NULL, grouped_params_pp, num_group_params, & (data_p -> bsd_base_data)))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
											FreeMemory (grouped_params_pp);
										}

									success_flag = true;
								}
							else
								{
									FreeParameterBounds (bounds_p, PT_SIGNED_INT);
								}
						}		/* if (bounds_p) */
				}
			else
				{
					FreeParameterBounds (bounds_p, PT_SIGNED_INT);
				}
		}


	return success_flag;
}


bool AddBlastPParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	return success_flag;
}


bool AddBlastNParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	return success_flag;

}


bool AddBlastXParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	return success_flag;
}


