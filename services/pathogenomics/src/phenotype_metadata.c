/*
 * phenotype_metadata.c
 *
 *  Created on: 16 Sep 2015
 *      Author: tyrrells
 */

#include "phenotype_metadata.h"


static char *ConvertKey (const char * const src_s);


bool InsertPhenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p, json_t *errors_p)
{
	bool success_flag = false;
	const char *phenotype_primary_key_id_s = "Isolate";
	const char *phenotype_primary_key_s = GetJSONString (values_p, phenotype_primary_key_id_s);

	if (phenotype_primary_key_s)
		{
			const char *mapped_key_id_s = "UKCPVS ID";

			if (SetMongoToolCollection (tool_p, data_p -> psd_samples_collection_s))
				{
					bson_t *query_p;

					if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL))
						{

						}		/* if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL)) */

				}		/* if (SetMongoToolCollection (tool_p, data_p -> psd_samples_collection_s)) */

		}		/* if (phenotype_primary_key_s) */

	return success_flag;
}

