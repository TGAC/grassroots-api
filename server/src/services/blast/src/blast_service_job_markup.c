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
/*
 * blast_service_job_markup.c
 *
 *  Created on: 21 Jan 2016
 *      Author: tyrrells
 */
#include <string.h>

#include "blast_service_job_markup.h"
#include "blast_service_job.h"
#include "blast_service_params.h"

#include "string_utils.h"


/*
 * STATIC VARIABLES
 */


const char * const S_REPORTS_S = "reports";
const char * const S_RESULTS_S = "blast_search_results";
const char * const S_REPORT_RESULTS_S = "hits";

/*
 * STATIC FUNCTION PROTOTYPES
 */

static bool AddSequenceOntologyTerms (json_t *context_p);

static bool AddEdamOntologyTerms (json_t *context_p);

static bool AddFaldoOntologyTerms (json_t *context_p);

static bool AddGap (json_t *gaps_p, const int32 from, const int32 to, const bool forward_strand_flag);

static bool AddGaps (json_t *marked_up_hits_p, const char * const gaps_key_s, const char *sequence_s);

static bool IsGap (const char c);

static json_t *AddAndGetMarkedUpReport (json_t *markup_reports_p, const char *database_s, const json_t *blast_result_search_p);

static bool AddMarkedUpHit (json_t *marked_up_results_p, const json_t *blast_hit_p, const DatabaseType db_type);

static json_t *GetBlastResult (BlastServiceJob *job_p, BlastServiceData *data_p);

static const char *GetDatabaseNameFromResult (const json_t *blast_report_p, const BlastServiceData *data_p);

static json_t *GetMarkupReports (json_t *markup_p);

static bool PopulateMarkedUpHit (json_t *marked_up_hit_p, const json_t *blast_hit_p, const DatabaseType db_type);



/*
 * FUNCTION DEFINITIONS
 */

bool AddSequence (json_t *root_p, const char *key_s, const char *query_sequence_s)
{
	bool success_flag = false;

	if (json_object_set_new (root_p, key_s, json_string (query_sequence_s)) == 0)
		{
			char *gaps_key_s = ConcatenateStrings (key_s, "_gaps");

			if (gaps_key_s)
				{
					success_flag = AddGaps (root_p, gaps_key_s, query_sequence_s);

					FreeCopiedString (gaps_key_s);
				}

		}

	return success_flag;
}


bool AddHitDetails (json_t *marked_up_result_p, const json_t *blast_hit_p, const DatabaseType db_type)
{
	bool success_flag = false;
	const json_t *hsps_p = json_object_get (blast_hit_p, "hsps");

	if (hsps_p)
		{
			if (PopulateMarkedUpHit	(marked_up_result_p, blast_hit_p, db_type))
				{
					json_t *marked_up_hsps_p = json_object_get (marked_up_result_p, "hsps");

					if (marked_up_hsps_p)
						{
							if (json_is_array (hsps_p))
								{
									size_t i;
									json_t *hsp_p;

									json_array_foreach (hsps_p, i, hsp_p)
										{
											json_t *marked_up_hsp_p = json_object ();

											if (marked_up_hsp_p)
												{
													bool added_flag = false;

													if (AddHsp (marked_up_hsp_p, hsp_p))
														{
															if (json_array_append_new (marked_up_hsps_p, marked_up_hsp_p) == 0)
																{
																	added_flag = true;
																}
															else
																{
																	PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, marked_up_hsp_p, "Failed to add marked_up_hit to hits array");
																}

														}		/* if (AddHsp (marked_up_hit_p, hsp_p)) */
													else
														{
															PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, hsp_p, "Failed to add hsp to hit");
														}

													if (!added_flag)
														{
															json_decref (marked_up_hsp_p);
														}

												}		/* if (marked_up_hsp_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create marked up hsp");

												}
										}

									success_flag = (json_array_size (hsps_p) == json_array_size (marked_up_hsps_p));
								}		/* if (json_is_array (hsps_p)) */


						}		/* if (json_object_set_new (marked_up_result_p, "hsps", marked_up_hsps_p) == 0) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, marked_up_result_p, "Failed to set marked up hsps array for marked up hits");
							json_decref (marked_up_hsps_p);
						}

				}		/* if (marked_up_hsps_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create hsps array for marked up hits");
				}



		}		/* if (hsps_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get source hsps from blast result");
		}

	return success_flag;
}




bool AddHsp (json_t *marked_up_hsp_p, const json_t *hsp_p)
{
	if (json_object_set_new (marked_up_hsp_p, "@type", json_string ("match")) == 0)
		{
			if (GetAndAddDoubleScoreValue (marked_up_hsp_p, hsp_p, "bit_score", "bit_score"))
				{
					if (GetAndAddDoubleScoreValue (marked_up_hsp_p, hsp_p, "evalue", "evalue"))
						{
							if (GetAndAddIntScoreValue (marked_up_hsp_p, hsp_p, "score", "score"))
								{
									if (GetAndAddIntScoreValue (marked_up_hsp_p, hsp_p, "num", "hsp_num"))
										{
											if (GetAndAddSequenceValue (marked_up_hsp_p, hsp_p, "hseq", "hit_sequence"))
												{
													if (GetAndAddHitLocation (marked_up_hsp_p, hsp_p, "hit_from", "hit_to", "hit_strand", "hit_location"))
														{
															if (GetAndAddSequenceValue (marked_up_hsp_p, hsp_p, "qseq", "query_sequence"))
																{
																	if (GetAndAddHitLocation (marked_up_hsp_p, hsp_p, "query_from", "query_to", "query_strand", "query_location"))
																		{
																			if (GetAndAddSequenceValue (marked_up_hsp_p, hsp_p, "midline", "midline"))
																				{
																					const char *midline_s = GetJSONString (hsp_p, "midline");
																					const char *query_sequence_s = GetJSONString (hsp_p, "qseq");
																					const char *hit_sequence_s = GetJSONString (hsp_p, "hseq");
																					const uint32 hit_index = 0;
																					const int inc_value = 1;

																					if (GetAndAddNucleotidePolymorphisms (marked_up_hsp_p, query_sequence_s, hit_sequence_s, midline_s, hit_index, inc_value))
																						{
																							return true;
																						}
																				}		/* if (GetAndAddSequenceValue (marked_up_hsp_p, hsp_p, "midline", "midline")) */
																			else
																				{
																					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add midline");
																				}

																		}		/* if (GetAndAddHitLocation (marked_up_hsp_p, hsp_p, "query_from", "query_to", "query_strand", "query_location")) */
																	else
																		{
																			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add query location details");
																		}

																}		/* if (GetAndAddSequenceValue (marked_up_hsp_p, hsp_p, "qseq", "query_sequence")) */
															else
																{
																	PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add query_sequence");
																}

														}		/* if (GetAndAddHitLocation (marked_up_hsp_p, hsp_p, "hit_from", "hit_to", "hit_strand", "hit_location")) */
													else
														{
															PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add hit location details");
														}

												}		/* if (GetAndAddSequenceValue (marked_up_hsp_p, hsps_p, "hseq", "hit_sequence")) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add hit_sequence");
												}

										}		/* if (GetAndAddIntScoreValue (marked_up_hsp_p, hsps_p, "num", "num") */
									else
										{
											PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add num");
										}

								}		/* if (GetAndAddIntScoreValue (marked_up_hsp_p, hsps_p, "score", "score") */
							else
								{
									PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add evalue");
								}

						}		/* if (GetAndAddDoubleScoreValue (marked_up_hsp_p, hsps_p, "evalue", "evalue")) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add evalue");
						}

				}		/* if (GetAndAddDoubleScoreValue (marked_up_hsp_p, hsps_p, "bit_score", "bit_score")) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add bit_score");
				}
		}		/* if (json_object_set_new (marked_up_hsp_p, "@type", json_string ("match")) == 0) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_hsp_p, "failed to add @type: match");
		}

	return false;
}


bool AddHitLocation (json_t *parent_p, const char *child_key_s, const int32 from, const int32 to, const bool forward_strand_flag)
{
	json_t *location_p = json_object ();

	if (location_p)
		{
			if (AddFaldoTerminus (location_p, "faldo:begin", from, forward_strand_flag))
				{
					if (AddFaldoTerminus (location_p, "faldo:end", to, forward_strand_flag))
						{
							if (json_object_set_new (location_p, "@type", json_string ("faldo:Region")) == 0)
								{
									if (json_object_set_new (parent_p, child_key_s, location_p) == 0)
										{
											return true;
										}		/* if (json_object_set_new (marked_up_result_p, child_key_s, location_p) == 0) */

								}		/* if (json_object_set_new (location_p, "@type", json_string ("faldo:Region")) == 0) */

						}		/* if (AddFaldoTerminus (location_p, "faldo:end", to, forward_strand_flag)) */

				}		/* if (AddFaldoTerminus (location_p, "faldo:begin", from, forward_strand_flag)) */

			json_decref (location_p);
		}		/* if (location_p) */

	return false;
}


bool GetAndAddHitLocation (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_from_key_s, const char *hsp_to_key_s, const char *strand_key_s, const char *child_key_s)
{
	bool success_flag = false;
	int32 from;

	if (GetJSONInteger (hsps_p, hsp_from_key_s, &from))
		{
			int32 to;

			if (GetJSONInteger (hsps_p, hsp_to_key_s, &to))
				{

					/*
						"faldo:location": {
							"@type": "faldo:Region",
							"faldo:begin": {
								"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
								"faldo:position": "99"
							},
							"faldo:end": {
								"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
								"faldo:position": "168"
							}
						}
					 */

					bool forward_strand_flag = true;
					const char *strand_s = GetJSONString (hsps_p, strand_key_s);

					if (strand_s)
						{
							if (strcmp (strand_s, "Plus") == 0)
								{
									forward_strand_flag = true;
								}
							else if (strcmp (strand_s, "Minus") == 0)
								{
									forward_strand_flag = false;
								}

							if (AddHitLocation (marked_up_result_p, child_key_s, from, to, forward_strand_flag))
								{
									success_flag = true;
								}
						}

				}		/* if (GetJSONInteger (hsps_p, hsp_to_key_s, &to)) */

		}		/* if (GetJSONInteger (hsps_p, hsp_from_key_s, &from)) */

	return success_flag;
}


bool AddFaldoTerminus (json_t *parent_json_p, const char *child_key_s, const int32 position, const bool forward_strand_flag)
{
	json_t *faldo_p = json_object ();

	if (faldo_p)
		{
			json_t *type_array_p = json_array ();

			if (type_array_p)
				{
					if (json_object_set_new (faldo_p, "@type", type_array_p) == 0)
						{
							if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0)
								{
									if (json_array_append_new (type_array_p, json_string ("faldo:ExactPosition")) == 0)
										{
											const char *strand_s = forward_strand_flag ? "faldo:ForwardStrandPosition" : "faldo:ReverseStrandPosition";

											if (json_array_append_new (type_array_p, json_string (strand_s)) == 0)
												{
													if (json_object_set_new (faldo_p, "faldo:position", json_integer (position)) == 0)
														{
															if (json_object_set_new (parent_json_p, child_key_s, faldo_p) == 0)
																{
																	return true;
																}

														}		/* if (json_object_set_new (faldo_p, "faldo:position", json_integer (position)) */

												}		/* if (json_array_append_new (type_array_p, json_string (strand_s)) == 0) */

										}		/* if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0) */

								}		/* if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0) */


						}		/* if (json_object_set_new (faldo_p, "@type", type_array_p) == 0) */
					else
						{
							json_decref (type_array_p);
						}
				}		/* if (type_array_p) */

			json_decref (faldo_p);
		}		/* if (faldo_p) */

	return false;
}





bool GetAndAddSequenceValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *sequence_key_s)
{
	bool success_flag = false;
	const char *sequence_value_s = GetJSONString (hsps_p, hsp_key_s);

	if (sequence_value_s)
		{
			if (AddSequence (marked_up_result_p, sequence_key_s, sequence_value_s))
				{
					success_flag = true;
				}
		}

	return success_flag;
}


bool GetAndAddDoubleScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s)
{
	bool success_flag = false;
	double64 value;

	if (GetJSONReal (hsps_p, hsp_key_s, &value))
		{
			if (AddDoubleScoreValue (marked_up_result_p, marked_up_key_s, value))
				{
					success_flag = true;
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_result_p, "failed to add int value for %s=" DOUBLE64_FMT, hsp_key_s, value);
				}
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, hsps_p, "failed to get real value for %s", hsp_key_s);
		}

	return success_flag;
}


bool GetAndAddIntScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s)
{
	bool success_flag = false;
	int32 value;

	if (GetJSONInteger (hsps_p, hsp_key_s, &value))
		{
			if (AddIntScoreValue (marked_up_result_p, marked_up_key_s, value))
				{
					success_flag = true;
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, marked_up_result_p, "failed to add int value for %s=" INT32_FMT, hsp_key_s, value);
				}
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, hsps_p, "failed to get int value for %s", hsp_key_s);
		}

	return success_flag;
}


bool AddIntScoreValue (json_t *parent_p, const char *key_s, int score_value)
{
	bool success_flag = false;

	if (json_object_set_new (parent_p, key_s, json_integer (score_value)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}



bool AddDoubleScoreValue (json_t *parent_p, const char *key_s, double64 score_value)
{
	bool success_flag = false;

	if (json_object_set_new (parent_p, key_s, json_real (score_value)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}


bool GetAndAddDatabaseDetails (json_t *marked_up_result_p, const char *database_s)
{
	json_t *database_p = json_object ();

	if (database_p)
		{
			if (json_object_set_new (database_p, "database_name", json_string (database_s)) == 0)
				{
					if (json_object_set_new (database_p, "database_uri", json_string (database_s)) == 0)
						{
							if (json_object_set_new (marked_up_result_p, "database", database_p) == 0)
								{
									return true;
								}
						}
				}

			json_decref (database_p);
		}

	return false;
}


json_t *MarkUpBlastResult (BlastServiceJob *job_p)
{
	json_t *markup_p = GetInitialisedProcessedRequest ();

	if (markup_p)
		{
			bool success_flag = false;
			BlastServiceData *data_p = (BlastServiceData *) (job_p -> bsj_job.sj_service_p -> se_data_p);
			json_t *blast_job_output_p = GetBlastResult (job_p, data_p);

			if (blast_job_output_p)
				{
					/*
					 * We currently understand hits objects and database names
					 */
					const json_t *blast_output_p = json_object_get (blast_job_output_p, "BlastOutput2");

					if (blast_output_p)
						{
							if (json_is_array (blast_output_p))
								{
									size_t i;
									json_t *result_p;

									json_t *markup_reports_p = GetMarkupReports (markup_p);

									json_array_foreach (blast_output_p, i, result_p)
										{
											json_t *blast_report_p = json_object_get (result_p, "report");

											if (blast_report_p)
												{
													const char *database_s = GetDatabaseNameFromResult (blast_report_p, data_p);

													if (database_s)
														{
															/* Get the hits */
															const json_t *blast_result_search_p = GetCompoundJSONObject (blast_report_p, "results.search");

															if (blast_result_search_p)
																{
																	json_t *marked_up_report_p = AddAndGetMarkedUpReport (markup_reports_p, database_s, blast_result_search_p);

																	if (marked_up_report_p)
																		{
																			const json_t *blast_hits_p =  json_object_get (blast_result_search_p, "hits");

																			if (blast_hits_p)
																				{
																					if (json_is_array (blast_hits_p))
																						{
																							size_t j = 0;
																							const size_t num_hits = json_array_size (blast_hits_p);

																							json_t *marked_up_hits_p = json_object_get (marked_up_report_p, S_REPORT_RESULTS_S);

																							success_flag = true;

																							while ((j < num_hits) && success_flag)
																								{
																									const json_t *blast_hit_p = json_array_get (blast_hits_p, j);

																									if (AddMarkedUpHit (marked_up_hits_p, blast_hit_p, data_p -> bsd_type))
																										{
																											++ j;
																										}
																									else
																										{
																											success_flag = false;
																										}
																								}		/* while ((j < num_hits) && success_flag) */


																						}		/* if (json_is_array (blast_hits_p)) */

																				}		/* if (blast_hits_p) */

																		}		/* if (marked_up_report_p) */

																}		/* if (blast_result_search_p) */

														}		/* if (database_s) */

												}		/* if (report_p) */

									}		/* json_array_foreach (root_p, i, output_p) */

								}		/* if (json_is_array (root_p)) */

						}		/* if (root_p) */

					json_decref (blast_job_output_p);
				}		/* if (blast_job_output_p) */

		}		/* if (markup_p) */

	return markup_p;
}



json_t *GetInitialisedProcessedRequest (void)
{
	json_t *root_p = json_object ();

	if (root_p)
		{
			json_t *context_p = json_object ();

			if (context_p)
				{
					if (json_object_set_new (root_p, "@context", context_p) == 0)
						{
							if (AddEdamOntologyTerms (context_p))
								{
									if (AddFaldoOntologyTerms (context_p))
										{
											if (AddSequenceOntologyTerms (context_p))
												{
													json_t *sequence_search_results_p = json_object ();

													if (sequence_search_results_p)
														{
															if (json_object_set_new (root_p, S_RESULTS_S, sequence_search_results_p) == 0)
																{
																	json_t *reports_p = json_array ();

																	if (reports_p)
																		{
																			if (json_object_set_new (sequence_search_results_p, S_REPORTS_S, reports_p) == 0)
																				{
																					return root_p;
																				}
																			else
																				{
																					json_decref (reports_p);
																				}
																		}

																}
															else
																{
																	json_decref (sequence_search_results_p);
																}

														}
												}
										}
								}

						}		/* if (json_object_set_new (root_p, "@context", context_p) == 0) */
					else
						{
							json_decref (context_p);
						}

				}		/* if (context_p) */

			json_decref (root_p);
		}		/* if (root_p) */

	return NULL;
}



bool GetAndAddScaffoldsFromHit (const json_t *hit_p, json_t *mark_up_p)
{
	bool success_flag = false;

	LinkedList *scaffolds_p = GetScaffoldsFromHit (hit_p);

	if (scaffolds_p)
		{
			json_t *scaffolds_array_p = json_array ();

			if (scaffolds_array_p)
				{
					if (json_object_set_new (mark_up_p, "scaffolds", scaffolds_array_p) == 0)
						{
							StringListNode *node_p = (StringListNode *) (scaffolds_p -> ll_head_p);

							success_flag = true;

							while (node_p && success_flag)
								{
									json_t *scaffold_p = json_object ();

									if (scaffold_p)
										{
											if (json_object_set_new (scaffold_p, "scaffold", json_string (node_p -> sln_string_s)) == 0)
												{
													if (json_array_append_new (scaffolds_array_p, scaffold_p) != 0)
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add scaffold object to scaffolds array for \"%s\"", node_p -> sln_string_s);
															success_flag = false;
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add scaffold \"%s\" to scaffold object", node_p -> sln_string_s);
													success_flag = false;
												}

											if (!success_flag)
												{
													json_decref (scaffold_p);
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create JSON scaffold object");
											success_flag = false;
										}

									node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
								}		/* while (node_p) */

						}		/* if (json_object_set_new (mark_up_p, "scaffolds", scaffolds_array_p) == 0) */

				}		/* if (scaffolds_array_p) */

		}		/* if (scaffolds_p) */

	return success_flag;
}


bool MarkUpHit (const json_t *hit_p, json_t *mark_up_p,  const DatabaseType db_type)
{
	bool success_flag = false;

	if (GetAndAddScaffoldsFromHit (hit_p, mark_up_p))
		{
			if (AddHitDetails (mark_up_p, hit_p, db_type))
				{
					success_flag = true;
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, mark_up_p, "Failed to add hit details");
				}

		}		/* if (GetAndAddScaffoldsFromHit (hit_p, mark_up_p)) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, mark_up_p, "Failed to add scaffolds");
		}

	return success_flag;
}


bool GetAndAddQueryMetadata (const json_t *blast_search_p, json_t *mark_up_p)
{
	bool success_flag = false;


	if (CopyJSONKeyStringValuePair (blast_search_p, mark_up_p, "query_id", false))
		{
			/* Not all dbs have a query_title so make it optional */
			if (CopyJSONKeyStringValuePair (blast_search_p, mark_up_p, "query_title", true))
				{
					if (CopyJSONKeyIntegerValuePair (blast_search_p, mark_up_p, "query_len", false))
						{
							success_flag = true;
						}
					else
						{
							PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, blast_search_p, "Failed to copy key \"%s\"", "query_len");
						}
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, blast_search_p, "Failed to copy key \"%s\"", "query_title");
				}
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, blast_search_p, "Failed to copy key \"%s\"", "query_id");
		}


	return success_flag;
}



LinkedList *GetScaffoldsFromHit (const json_t *hit_p)
{
	LinkedList *scaffolds_p = AllocateStringLinkedList ();

	if (scaffolds_p)
		{
			json_t *description_p = json_object_get (hit_p, "description");

			if (description_p)
				{
					if (json_is_array (description_p))
						{
							size_t k;
							json_t *item_p;

							json_array_foreach (description_p, k, item_p)
							{
								json_t *data_p = json_object ();

								if (data_p)
									{
										const char *full_title_s = GetJSONString (item_p, "title");

										if (full_title_s)
											{
												/*
												 * There may be more on this line than just the scaffold name
												 * so lets get up until the first space or |
												 */
												const char *id_end_p = strpbrk (full_title_s, " |");
												const uint32 size  = id_end_p ? id_end_p - full_title_s : 0;
												char *scaffold_s = CopyToNewString (full_title_s, size, false);

												if (scaffold_s)
													{
														StringListNode *node_p = AllocateStringListNode (scaffold_s, MF_SHALLOW_COPY);

														if (node_p)
															{
																LinkedListAddTail (scaffolds_p, (ListItem *) node_p);
															}		/* if (node_p) */

													}		/* if (scaffold_s) */

											}		/* if (full_title_s) */

									}		/* if (data_p) */
							}
						}
				}

		}		/* if (scaffolds_p) */

	return scaffolds_p;
}



bool GetAndAddNucleotidePolymorphisms (json_t *marked_up_hsp_p, const char *reference_sequence_s, const char *hit_sequence_s, const char *midline_s, uint32 hit_index, const int32 inc_value)
{
	bool success_flag = false;

	if (*midline_s != '\0')
		{
			bool loop_flag;
			uint32 start_of_region = hit_index;
			const char *hit_gap_start_p = NULL;
			const char *reference_gap_start_p = NULL;
			bool match_flag = (*midline_s == '|');

			++ midline_s;
			hit_index += inc_value;
			loop_flag = (*midline_s != '\0');

			success_flag = true;

			while (loop_flag && success_flag)
				{
					bool current_match_flag = (*midline_s == '|');

					/* have we moved to a different region? */
					if (match_flag != current_match_flag)
						{
							if (match_flag)
								{
									/* we've just started a gap */
									start_of_region = hit_index + inc_value;
									hit_gap_start_p = hit_sequence_s + 1;
									reference_gap_start_p = reference_sequence_s + 1;
								}
							else
								{
									/* we've just finished a gap */
									const uint32 end_of_region = hit_index; // - inc_value;

									if (!AddPolymorphism (marked_up_hsp_p, hit_gap_start_p, reference_gap_start_p, start_of_region, end_of_region))
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to the polymorphism starting at reference \"%s\"", reference_gap_start_p);
											success_flag = false;
										}
								}

							match_flag = current_match_flag;
						}


					hit_index += inc_value;
					++ midline_s;
					++ reference_sequence_s;
					++ hit_sequence_s;

					loop_flag = (*midline_s != '\0');
				}

		}

	return success_flag;
}


static bool IsGap (const char c)
{
	return ((c == '-') || (c == 'n') || (c == 'N'));
}


static bool AddGaps (json_t *marked_up_hits_p, const char * const gaps_key_s, const char *sequence_s)
{
	bool success_flag = false;
	bool dec_flag = true;
	json_t *gaps_p = json_array ();

	if (gaps_p)
		{
			bool loop_flag = (*sequence_s != '\0');

			if (loop_flag)
				{
					bool gap_flag = IsGap (*sequence_s);
					int32 gap_start = -1;
					int32 index = 2;

					if (gap_flag)
						{
							gap_start = 0;
						}

					++ sequence_s;
					loop_flag = (*sequence_s != '\0');

					success_flag = true;

					while (loop_flag && success_flag)
						{
							bool current_gap_flag = IsGap (*sequence_s);

							if (gap_flag != current_gap_flag)
								{
									if (current_gap_flag)
										{
											/* we've just started a gap */
											gap_start = index;
										}
									else
										{
											/* we've just finished a gap */
											int32 gap_end = index - 1;

											if (!AddGap (gaps_p, gap_start, gap_end, true))
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to the gap starting at reference \"%s\"", sequence_s);
													success_flag = false;
												}

											gap_start = -1;
										}

									gap_flag = current_gap_flag;
								}

							++ sequence_s;
							++ index;
							loop_flag = (*sequence_s != '\0');
						}

					if (gap_start != -1)
						{
							/* we've just finished a gap */
							int32 gap_end = index;

							if (!AddGap (gaps_p, gap_start, gap_end, true))
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to the gap starting at reference \"%s\"", sequence_s);
									success_flag = false;
								}
						}

				}


			if (success_flag)
				{
					if (json_array_size (gaps_p) > 0)
						{
							if (json_object_set_new (marked_up_hits_p, gaps_key_s, gaps_p) == 0)
								{
									dec_flag = false;
								}
							else
								{
									success_flag = false;
								}		/* if (json_object_set_new (marked_up_hits_p, gaps_key_s, gaps_p) == 0) */
						}
				}

			if (dec_flag)
				{
					json_decref (gaps_p);
				}

		}		/* if (gaps_p) */



	return success_flag;
}


static bool AddGap (json_t *gaps_p, const int32 from, const int32 to, const bool forward_strand_flag)
{
	json_t *gap_p = json_object ();

	if (gap_p)
		{
			if (json_object_set_new (gap_p, "@type", json_string ("gap")) == 0)
				{
					if (AddHitLocation (gap_p, "faldo:location", from, to, forward_strand_flag))
						{
							if (json_array_append_new (gaps_p, gap_p) == 0)
								{
									return true;
								}
						}		/* if (AddHitLocation (gap_p, "faldo:location", from, to, forward_strand_flag)) */

				}		/* if (json_object_set_new (gap_p, "@type", json_string ("gap")) == 0) */

			json_decref (gap_p);
		}		/* if (gap_p) */

	return false;
}


bool AddPolymorphism (json_t *marked_up_hsp_p, const char *hit_gap_start_p, const char *reference_gap_start_p, const uint32 start_of_region, const uint32 end_of_region)
{
	bool success_flag = false;
	uint32 length;
	bool forward_strand_flag;
	json_t *polymorphisms_p = NULL;

	if (start_of_region > end_of_region)
		{
			length = 1 + start_of_region - end_of_region;
			forward_strand_flag = false;
		}
	else
		{
			length = 1 + end_of_region - start_of_region;
			forward_strand_flag = true;
		}

	polymorphisms_p = json_object_get (marked_up_hsp_p, "polymorphisms");

	if (!polymorphisms_p)
		{
			polymorphisms_p = json_array ();

			if (polymorphisms_p)
				{
					if (json_object_set_new (marked_up_hsp_p, "polymorphisms", polymorphisms_p) != 0)
						{
							json_decref (polymorphisms_p);
							polymorphisms_p = NULL;
						}
				}
		}

	if (polymorphisms_p)
		{
			json_t *polymorphism_p = json_object ();

			if (polymorphism_p)
				{
					const char *type_s = (length == 1) ? "snp" : "mnp";

					if (AddHitLocation (polymorphism_p, "faldo:location", start_of_region, end_of_region, forward_strand_flag))
						{
							if (json_object_set_new (polymorphism_p, "@type", json_string (type_s)) == 0)
								{
									json_t *diff_p = json_object ();

									if (diff_p)
										{
											if (json_object_set_new (polymorphism_p, "sequence_difference", diff_p) == 0)
												{
													if (AddSubsequenceMarkup (diff_p, "query", reference_gap_start_p, length))
														{
															if (AddSubsequenceMarkup (diff_p, "hit", hit_gap_start_p, length))
																{
																	if (json_array_append_new (polymorphisms_p, polymorphism_p) == 0)
																		{
																			success_flag = true;
																		}
																}

														}

												}
											else
												{
													json_decref (diff_p);
												}
										}


								}

						}

					if (!success_flag)
						{
							json_decref (polymorphism_p);
						}

				}		/* if (polymorphism_p) */

		}


	return success_flag;
}


bool AddSubsequenceMarkup (json_t *parent_p, const char *key_s, const char *subsequence_start_s, const uint32 length)
{
	bool success_flag = false;
	char *diff_s = CopyToNewString (subsequence_start_s, length, false);

	if (diff_s)
		{
			if (json_object_set_new (parent_p, key_s, json_string (diff_s)) == 0)
				{
					success_flag = true;
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, parent_p, "Failed to add \"%s\": \"%s\"", key_s, diff_s);
				}

			FreeCopiedString (diff_s);
		}		/* if (diff_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to the first " UINT32_FMT " characters of \"%s\" for subsequence for key \"%s\"", length, subsequence_start_s, key_s);
		}

	return success_flag;
}


bool AddTerm (json_t *root_p, const char *key_s, const char *term_s, const bool add_id_flag)
{
	json_t *term_p = json_object ();

	if (term_p)
		{
			if (json_object_set_new (term_p, "@context", json_string (term_s)) == 0)
				{
					if ((!add_id_flag) || (json_object_set_new (term_p, "@type", json_string ("@id")) == 0))
						{
							if (json_object_set_new (root_p, key_s, term_p) == 0)
								{
									return true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object to root object with context \"%s\" with key \"%s", term_s, key_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object type for context \"%s\" with key \"%s", term_s, key_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object context \"%s\" with key \"%s", term_s, key_s);
				}

			json_decref (term_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json object for context \"%s\" with key \"%s", term_s, key_s);
		}


	return false;
}



static bool AddSequenceOntologyTerms (json_t *context_p)
{
	bool success_flag = false;

	if (AddTerm (context_p, "scaffold", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000148", true))
		{
			if (AddTerm (context_p, "query_sequence", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000149", true))
				{
					if (AddTerm (context_p, "hit_sequence", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000149", true))
						{
							if (AddTerm (context_p, "evalue", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001686", true))
								{
									if (AddTerm (context_p, "bit_score", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001685", true))
										{
											if (AddTerm (context_p, "contained_by", "http://www.sequenceontology.org/browser/current_svn/term/contained_by", true))
												{
													if (AddTerm (context_p, "snp", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000694", false))
														{
															if (AddTerm (context_p, "mnp", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001013", false))
																{
																	if (AddTerm (context_p, "match", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000039", false))
																		{
																			if (AddTerm (context_p, "nucleotide_match", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000347", false))
																				{
																					if (AddTerm (context_p, "protein_match", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000349", false))
																						{
																							if (AddTerm (context_p, "sequence_difference", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000413", true))
																								{
																									if (AddTerm (context_p, "gap", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000730", false))
																										{
																											success_flag = true;
																										}
																								}
																						}
																				}
																		}
																}
														}
												}
										}
								}
						}
				}
		}

	return success_flag;
}


static bool AddEdamOntologyTerms (json_t *context_p)
{
	bool success_flag = false;

	if (AddTerm (context_p, "sequence_length", "http://edamontology.org/data_1249", false))
		{
			if (AddTerm (context_p, S_RESULTS_S, "http://edamontology.org/data_0857",true))
				{
					if (AddTerm (context_p, "database_metadata", "http://edamontology.org/data_0957",true))
						{
							if (AddTerm (context_p, "database_name", "http://edamontology.org/data_1056",true))
								{
									success_flag = true;
								}
						}
				}
		}

	return success_flag;
}


static bool AddFaldoOntologyTerms (json_t *context_p)
{
	bool success_flag = false;

	if (json_object_set_new (context_p, "faldo", json_string ("http://biohackathon.org/resource/faldo")) == 0)
		{
			success_flag = true;
		}


	return success_flag;
}



static bool AddMarkedUpHit (json_t *marked_up_results_p, const json_t *blast_hit_p, const DatabaseType db_type)
{
	json_t *output_p = json_object ();

	if (output_p)
		{
			if (MarkUpHit (blast_hit_p, output_p, db_type))
				{
					if (json_array_append_new (marked_up_results_p, output_p) == 0)
						{
							return true;
						}
				}

			json_decref (output_p);
		}		/* if (output_p) */

	return false;
}



/*
 * Get the result. Ideally we'd like to get this in a format that we can parse, so to begin with we'll use the single json format
 * available in blast 2.3+
 */
static json_t *GetBlastResult (BlastServiceJob *job_p, BlastServiceData *data_p)
{
	json_t *blast_output_p = NULL;

	/*
	 * Get the result. Ideally we'd like to get this in a format that we can parse, so to begin with we'll use the single json format
	 * available in blast 2.3+
	 */
	char *raw_result_s = GetBlastResultByUUID (data_p, job_p -> bsj_job.sj_id, BOF_SINGLE_FILE_JSON_BLAST);

	if (raw_result_s)
		{
			json_error_t err;
			blast_output_p = json_loads (raw_result_s, 0, &err);

			if (!blast_output_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "error decoding blast result: \"%s\"\n\"%s\"\n%d %d %d\n%s\n", err.text, err.source, err.line, err.column, err.position, raw_result_s);
				}

			FreeCopiedString (raw_result_s);
		}
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", uuid_s);

		}

	return blast_output_p;
}


static json_t *AddAndGetMarkedUpReport (json_t *markup_reports_p, const char *database_s, const json_t *blast_result_search_p)
{
	json_t *report_p = json_object ();

	if (report_p)
		{
			json_t *results_p = json_array ();

			if (results_p)
				{
					if (json_object_set_new (report_p, S_REPORT_RESULTS_S, results_p) == 0)
						{
							if (GetAndAddDatabaseDetails (report_p, database_s))
								{
									if (GetAndAddQueryMetadata (blast_result_search_p, report_p))
										{
											if (json_array_append_new (markup_reports_p, report_p) == 0)
												{
													return report_p;
												}
										}
								}
						}
					else
						{
							json_decref (results_p);
						}
				}

			json_decref (report_p);
		}

	return NULL;
}



static const char *GetDatabaseNameFromResult (const json_t *blast_report_p, const BlastServiceData *data_p)
{
	const char *database_s = NULL;

	json_t *db_p = GetCompoundJSONObject (blast_report_p, "search_target.db");

	/* Get the database name */
	if (db_p)
		{
			if (json_is_string (db_p))
				{
					const char *database_filename_s = json_string_value (db_p);

					if (database_filename_s)
						{
							database_s = GetMatchingDatabaseName (data_p, database_filename_s);
						}		/* if (database_s) */
				}
		}

	return database_s;
}


static json_t *GetMarkupReports (json_t *markup_p)
{
	json_t *value_p = json_object_get (markup_p, S_RESULTS_S);

	if (value_p)
		{
			value_p = json_object_get (value_p, S_REPORTS_S);
		}

	return value_p;
}



static bool PopulateMarkedUpHit (json_t *marked_up_hit_p, const json_t *blast_hit_p, const DatabaseType db_type)
{
	bool success_flag = false;
	int hit_num;
	const char *hit_type_s = NULL;

	if (db_type == DT_NUCLEOTIDE)
		{
			hit_type_s = "nucelotide_match";
		}
	else if (db_type == DT_PROTEIN)
		{
			hit_type_s = "protein_match";
		}


	if (hit_type_s)
		{
			if (json_object_set_new (marked_up_hit_p, "@type", json_string (hit_type_s)) == 0)
				{
					if (GetJSONInteger (blast_hit_p, "num", &hit_num))
						{
							if (json_object_set_new (marked_up_hit_p, "hit_num", json_integer (hit_num)) == 0)
								{
									if (GetAndAddIntScoreValue (marked_up_hit_p, blast_hit_p, "len", "sequence_length"))
										{
											json_t *marked_up_hsps_p = json_array ();

											if (marked_up_hsps_p)
												{
													if (json_object_set_new (marked_up_hit_p, "hsps", marked_up_hsps_p) == 0)
														{
															success_flag = true;
														}
													else
														{
															json_decref (marked_up_hsps_p);
														}
												}

										}

								}
						}

				}		/* if (json_object_set_new (marked_up_hit_p, "@type", json_string (hit_type_s)) == 0) */

		}		/* if (hit_type_s) */



	return success_flag;
}


