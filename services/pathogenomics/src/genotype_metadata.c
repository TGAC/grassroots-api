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
 * genotype_metadata.c
 *
 *  Created on: 19 Oct 2015
 *      Author: tyrrells
 */
#include "genotype_metadata.h"


bool InsertGenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p, json_t *errors_p)
{
	bool success_flag = false;
	const char *genotype_primary_key_id_s = "ID";
	const char *genotype_primary_key_s = GetJSONString (values_p, genotype_primary_key_id_s);

	if (genotype_primary_key_s)
		{
			if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_samples_collection_s))
				{
					bson_t *query_p = bson_new ();

					if (query_p)
						{
							if (BSON_APPEND_UTF8 (query_p, genotype_primary_key_id_s, genotype_primary_key_id_s))
								{
									if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL))
										{
											json_t *genotype_p = json_object ();

											if (genotype_p)
												{
													if (json_object_set (genotype_p, PG_GENOTYPE_S, values_p) == 0)
														{
															const char *id_key_s = "ID";
															json_t *json_p = GetCurrentValuesAsJSON (tool_p, &id_key_s, 1);

															if (json_p)
																{
																	if (json_object_update (json_p, genotype_p) == 0)
																		{
																			if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_genotype_collection_s))
																				{
																					bson_oid_t *oid_p = InsertJSONIntoMongoCollection (tool_p, json_p);

																					if (oid_p)
																						{
																							success_flag = true;
																						}
																				}
																		}
																}

														}		/* if (json_object_set (genotype_p, PG_GENOTYPE_S, values_p) == 0) */

												}		/* if (genotype_p) */


										}		/* if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL)) */

								}		/* if (BSON_APPEND_UTF8 (query_p, mapped_key_id_s, phenotype_primary_key_s)) */

							bson_destroy (query_p);
						}		/* if (query_p) */


				}		/* if (SetMongoToolCollection (tool_p, data_p -> psd_samples_collection_s)) */

		}		/* if (phenotype_primary_key_s) */

	return success_flag;
}
