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
 * phenotype_metadata.c
 *
 *  Created on: 16 Sep 2015
 *      Author: tyrrells
 */

#include "phenotype_metadata.h"

#ifdef _DEBUG
	#define PHENOTYPE_METADATA_DEBUG	(STM_LEVEL_FINE)
#else
	#define PHENOTYPE_METADATA_DEBUG	(STM_LEVEL_NONE)
#endif


const char *InsertPhenotypeData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p)
{
	return InsertOrUpdateMongoData (tool_p, values_p, data_p -> psd_database_s, collection_s, "Isolate", PG_UKCPVS_ID_S, PG_PHENOTYPE_S);
}


