/**@file blast_service.h
*/ 

#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H


#include "blast_service_api.h"
#include "blast_tool_set.hpp"



#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
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
} DatabaseInfo;


 typedef struct BLAST_SERVICE_LOCAL BlastServiceData
{
	ServiceData bsd_base_data;
	BlastToolSet *bsd_blast_tools_p;

	const char *bsd_working_dir_s;

	/* A NULL-terminated array of the databases available to search */
	DatabaseInfo *bsd_databases_p;

} BlastServiceData;


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
