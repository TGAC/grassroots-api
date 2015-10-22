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
	return InsertOrUpdateMongoData (tool_p, values_p, data_p -> psd_database_s, collection_s, PG_ID_S, NULL, PG_GENOTYPE_S);
}
