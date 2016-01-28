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
/**@file blast_service.h
*/ 

#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H


#include "blast_service_api.h"
#include "blast_tool_set.hpp"
#include "blast_formatter.h"


#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_JOB_ID MAKE_TAG ('B', 'I', 'J', 'I')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_DATABASE_NAME MAKE_TAG ('B', 'D', 'B', 'N')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')
#define TAG_BLAST_SUBRANGE_FROM MAKE_TAG ('B', 'Q', 'F', 'R')
#define TAG_BLAST_SUBRANGE_TO MAKE_TAG ('B', 'Q', 'T', 'O')
#define TAG_BLAST_MATCH_SCORE MAKE_TAG ('B', 'M', 'T', 'C')
#define TAG_BLAST_MISMATCH_SCORE MAKE_TAG ('B', 'M', 'S', 'M')
#define TAG_BLAST_OUTPUT_FORMAT MAKE_TAG ('B', 'F', 'M', 'T')


typedef struct BLAST_SERVICE_LOCAL DatabaseInfo
{
	const char *di_name_s;
	const char *di_description_s;
	bool di_active_flag;
} DatabaseInfo;


 typedef struct BLAST_SERVICE_LOCAL BlastServiceData
{
	ServiceData bsd_base_data;
	BlastToolSet *bsd_blast_tools_p;

	const char *bsd_working_dir_s;

	BlastFormatter *bsd_formatter_p;

	/* A NULL-terminated array of the databases available to search */
	DatabaseInfo *bsd_databases_p;

} BlastServiceData;



#ifdef ALLOCATE_BLAST_SERVICE_CONSTANTS
	#define BLAST_SERVICE_PREFIX BLAST_SERVICE_LOCAL
	#define BLAST_SERVICE_VAL(x)	= x
#else
	#define BLAST_SERVICE_PREFIX extern
	#define BLAST_SERVICE_VAL(x)
#endif


#define BS_DEFAULT_OUTPUT_FORMAT (11)

BLAST_SERVICE_PREFIX const char *BS_INPUT_SUFFIX_S BLAST_SERVICE_VAL (".input");
BLAST_SERVICE_PREFIX const char *BS_OUTPUT_SUFFIX_S BLAST_SERVICE_VAL (".output");
BLAST_SERVICE_PREFIX const char *BS_LOG_SUFFIX_S BLAST_SERVICE_VAL (".log");
//BLAST_SERVICE_PREFIX const uint32 BS_DEFAULT_OUTPUT_FORMAT BLAST_SERVICE_VAL (11);
BLAST_SERVICE_PREFIX const char *BS_DEFAULT_OUTPUT_FORMAT_S BLAST_SERVICE_VAL ("11");

BLAST_SERVICE_PREFIX const char *BS_DATABASE_GROUP_NAME_S BLAST_SERVICE_VAL ("Available Databases");


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


BLAST_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
