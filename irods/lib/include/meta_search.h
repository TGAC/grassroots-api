/*
 * meta_search.h
 *
 *  Created on: 2 Feb 2015
 *      Author: tyrrells
 */

#ifndef META_SEARCH_H
#define META_SEARCH_H

#include "jansson.h"
#include "irods_util_library.h"

#include "linked_list.h"
#include "typedefs.h"

typedef struct SearchTerm
{
	const char *st_clause_s;
	const char *st_key_s;
	const char *st_op_s;
	const char *st_value_s;
} SearchTerm;


typedef struct SearchTermNode
{
	ListItem stn_node;
	SearchTerm stn_term;
} SearchTermNode;


#ifdef __cplusplus
extern "C"
{
#endif


IRODS_UTIL_API SearchTermNode *AllocateSearchTermNode (const char *clause_s, const char *key_s, const char *op_s, const char *value_s);


IRODS_UTIL_API int32 DetermineSearchTerms (LinkedList *terms_p, const json_t *json_p);


IRODS_UTIL_API int DoMetaSearch (rcComm_t *connection_p, const int *column_ids_p, const uint32 num_columns, const char **search_tags_pp, bool upper_case_flag, char * const zone_s);


#ifdef __cplusplus
}
#endif


#endif /* META_SEARCH_H_ */
