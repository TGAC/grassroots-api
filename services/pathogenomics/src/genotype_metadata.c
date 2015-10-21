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


#ifdef _DEBUG
	#define GENOTYPE_METADATA_DEBUG	(STM_LEVEL_FINE)
#else
	#define GENOTYPE_METADATA_DEBUG	(STM_LEVEL_NONE)
#endif


const char *InsertGenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p)
{
	const char *error_s = NULL;
	const char *genotype_primary_key_id_s = "ID";
	const char *genotype_primary_key_s = GetJSONString (values_p, genotype_primary_key_id_s);

	if (genotype_primary_key_s)
		{
			if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_samples_collection_s))
				{
					bson_t *query_p = bson_new ();

					if (query_p)
						{
							if (BSON_APPEND_UTF8 (query_p, genotype_primary_key_id_s, genotype_primary_key_s))
								{
									#if GENOTYPE_METADATA_DEBUG >= STM_LEVEL_FINE
									LogBSON (query_p, GENOTYPE_METADATA_DEBUG, "genotype query");
									#endif

									/* remove the ID field */
									json_object_del (values_p, PG_ID_S);

									if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_genotype_collection_s))
										{
											if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL))
												{
													json_t *genotype_p = json_object ();

													if (genotype_p)
														{
															if (json_object_set (genotype_p, PG_GENOTYPE_S, values_p) == 0)
																{
																	if (!UpdateMongoDocumentByBSON (tool_p, query_p, genotype_p))
																		{
																			error_s = "Failed to create update genotype document";
																		}
																}
															else
																{
																	error_s = "Failed to update sub-document";
																}
														}
													else
														{
															error_s = "Failed to create sub-document for updating";
														}

												}		/* if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL)) */
											else
												{
													json_error_t err;
													json_t *doc_p = json_pack_ex (&err, 0, "{s:s,s:o}", PG_ID_S, genotype_primary_key_s, PG_GENOTYPE_S, values_p);

													if (doc_p)
														{
															bson_oid_t *oid_p = InsertJSONIntoMongoCollection (tool_p, doc_p);

															if (!oid_p)
																{
																	error_s = "Failed to update genotype data";
																}
														}
													else
														{
															error_s = "Failed to create genotype document to insert";
														}

												}

										}		/* if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_genotype_collection_s)) */

								}		/* if (BSON_APPEND_UTF8 (query_p, mapped_key_id_s, phenotype_primary_key_s)) */

							bson_destroy (query_p);
						}		/* if (query_p) */

				}		/* if (SetMongoToolCollection (tool_p, data_p -> psd_samples_collection_s)) */

		}		/* if (genotype_primary_key_s) */

	return error_s;
}
