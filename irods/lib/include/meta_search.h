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
#include "query.h"


typedef struct IrodsSearch
{
	LinkedList *is_search_terms_p;
} IrodsSearch;



typedef struct SearchTerm
{
	const char *st_clause_s;
	int st_key_column_id;
	const char *st_key_s;
	const char *st_op_s;
	int st_value_column_id;
	const char *st_value_s;

	char *st_key_buffer_s;
	char *st_value_buffer_s;
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


IRODS_UTIL_API IrodsSearch *AllocateIrodsSearch (void);


IRODS_UTIL_API void FreeIrodsSearch (IrodsSearch *search_p);


IRODS_UTIL_API QueryResults *DoIrodsSearch (IrodsSearch *search_p, rcComm_t *connection_p);


IRODS_UTIL_API bool AddIrodsSearchTerm (IrodsSearch *search_p, const char *clause_s, const char *key_s, const int key_id, const char *op_s, const char *value_s, const int value_id);


IRODS_UTIL_API int32 DetermineSearchTerms (LinkedList *terms_p, const json_t *json_p);


IRODS_UTIL_API QueryResults *DoMetaSearch (const IrodsSearch * const search_p, rcComm_t *connection_p, int *select_column_ids_p, const int num_select_columns, const bool upper_case_flag, char *zone_s);


#ifdef __cplusplus
}
#endif


#endif /* META_SEARCH_H_ */
