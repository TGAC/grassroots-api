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
#include "json_tools.h"


#ifdef _DEBUG
	#define GENOTYPE_METADATA_DEBUG	(STM_LEVEL_FINE)
#else
	#define GENOTYPE_METADATA_DEBUG	(STM_LEVEL_NONE)
#endif


const char *InsertGenotypeData (MongoTool *tool_p, json_t *values_p, PathogenomicsServiceData *data_p)
{
	const char *error_s = NULL;
	const char * const key_s = PG_ID_S;
	const char *id_s = GetJSONString (values_p, key_s);

	if (id_s)
		{
			/* Create a json doc with "genotype"=values_p and PG_ID_S=id_s */
			json_t *doc_p = json_object ();

			if (doc_p)
				{
					if (json_object_set_new (doc_p, PG_ID_S, json_string (id_s)) == 0)
						{
							if (json_object_del (values_p, key_s) != 0)
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to remove %s from genotype", key_s);
								}

							if (json_object_set (doc_p, PG_GENOTYPE_S, values_p) == 0)
								{
									error_s = InsertOrUpdateMongoData (tool_p, doc_p, NULL, NULL, PG_ID_S, NULL, NULL);
								}
							else
								{
									error_s = "Failed to add values to new genotype data";
								}

						}		/* if (json_object_set_new (doc_p, PG_ID_S, json_string (id_s)) == 0) */
					else
						{
							error_s = "Failed to add id to new genotype data";
						}

					WipeJSON (doc_p);
				}		/* if (doc_p) */
			else
				{
					error_s = "Failed to create genotype data to add";
				}

		}		/* if (id_s) */
	else
		{
			error_s = "Failed to get ID value";
		}

	return error_s;
}
