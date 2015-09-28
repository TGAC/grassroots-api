/*
 * phenotype_metadata.c
 *
 *  Created on: 16 Sep 2015
 *      Author: tyrrells
 */

#include "phenotype_metadata.h"


bool InsertPhenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p, json_t *errors_p)
{
	bool success_flag = false;
	const char *phenotype_primary_key_id_s = "Isolate";
	const char *phenotype_primary_key_s = GetJSONString (values_p, phenotype_primary_key_id_s);

	if (phenotype_primary_key_s)
		{
			const char *mapped_key_id_s = "UKCPVS ID";

			if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_samples_collection_s))
				{
					bson_t *query_p = bson_new ();

					if (query_p)
						{
							if (BSON_APPEND_UTF8 (query_p, mapped_key_id_s, phenotype_primary_key_s))
								{
									if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL))
										{
											const char *id_key_s = "ID";
											json_t *json_p = GetCurrentValuesAsJSON (tool_p, &id_key_s, 1);

											if (json_p)
												{
													if (json_object_update (values_p, json_p) == 0)
														{
															if (SetMongoToolCollection (tool_p, data_p -> psd_database_s, data_p -> psd_phenotype_collection_s))
																{
																	bson_oid_t *oid_p = InsertJSONIntoMongoCollection (tool_p, values_p);

																	if (oid_p)
																		{
																			success_flag = true;
																		}
																}
														}
												}

										}		/* if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL)) */

								}		/* if (BSON_APPEND_UTF8 (query_p, mapped_key_id_s, phenotype_primary_key_s)) */

							bson_destroy (query_p);
						}		/* if (query_p) */


				}		/* if (SetMongoToolCollection (tool_p, data_p -> psd_samples_collection_s)) */

		}		/* if (phenotype_primary_key_s) */

	return success_flag;
}

