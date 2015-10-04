/**
 * @file meta_search.h
 *
 *  Created on: 2 Feb 2015
 *      Author: tyrrells
 *
 *  @brief A set of routines to perform a metadata-based search
 *  of an iRODS server. It uses the iCAT functionality
 *  @addtogroup iRODS
 *  @{
 */

#ifndef META_SEARCH_H
#define META_SEARCH_H

#include "jansson.h"
#include "irods_util_library.h"

#include "linked_list.h"
#include "typedefs.h"
#include "query.h"


/**
 * A set of SearchTerms to use when running a search.
 *
 */
typedef struct IrodsSearch
{
	/**
	 * A LinkedList of SearchTermNodes
	 * @see SearchTermNode
	 */
	LinkedList *is_search_terms_p;
} IrodsSearch;


/**
 * @brief A datatype for storing the values needed for an iRODS search.
 *
 * A SearchTerm contains the values needed to build an SQL-style query
 * to run on an iRODS server.
 */
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


/**
 * A datatype for storing a SearchTerm on a LinkedList.
 */
typedef struct SearchTermNode
{
	/** The Listnode */
	ListItem stn_node;

	/** The SearchTerm to store on the list */
	SearchTerm stn_term;
} SearchTermNode;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate an IrodsSearch.
 *
 * @return A newly-allocated IrodsSearch or <code>NULL</code> upon error.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API IrodsSearch *AllocateIrodsSearch (void);


/**
 * Free an IrodsSearch.
 *
 * @param search_p The IrodsSearch to free.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API void FreeIrodsSearch (IrodsSearch *search_p);


/**
 * Clear an IrodsSearch.
 *
 * @param search_p The IrodsSearch to clear.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API void ClearIrodsSearch (IrodsSearch *search_p);




/**
 * Perform an iRODS search.
 *
 * @param search_p The IrodsSearch to perform.
 * @param connection_p The connection to the iRODS server to perform the search on.
 * @return The QueryResults from the search.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API QueryResults *DoIrodsSearch (IrodsSearch *search_p, rcComm_t *connection_p);


/**
 * Add a search term to an IrodsSearch.
 *
 * This adds a search term in the form of a key op value triplet. For example
 * key could be "foo", value = "bar" and op = "="
 * @param search_p The IrodsSearch to add the term to.
 * @param clause_s The search term's clause.
 * @param key_s The search term's key.
 * @param key_id The id of the key's column.
 * @param op_s The search term's operation such as "=", "<", <i>etc.</i>
 * @param value_s The search term's value.
 * @param value_id The id of the value's column.
 * @return <code>true</code> if the search term was added successfully, <code>false</code> otherwise.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API bool AddIrodsSearchTerm (IrodsSearch *search_p, const char *clause_s, const char *key_s, const int key_id, const char *op_s, const char *value_s, const int value_id);


/**
 * Parse a JSON fragment and add SearchTermNodes to a LinkedList.
 *
 * @param terms_p The LinkedList where the SearchTermNodes will get added.
 * @param json_p The JSON fragment to parse. The JSON is of the form:
 *
 * ~~~~~~~~~~~{.json}
 * {
 *     "key": "key_value",
 *     "key_id": key_id,
 *     "value": "value_value",
 *     "value_id": value_id,
 *     "clause": "clause_value"
 * }
 * ~~~~~~~~~~~
 *
 * @return The number of search terms added.
 * @memberof IrodsSearch
 */
IRODS_UTIL_API int32 DetermineSearchTerms (LinkedList *terms_p, const json_t *json_p);


/**
 * Perform a meta-based search
 *
 * @param search_p The IrodsSearch to perform.
 * @param connection_p The connection to the iRODS server to perform the search on.
 * @param select_column_ids_p An array of the column ids to use for the select clause.
 * @param num_select_columns The number of ids pointed to by select_column_ids_p.
 * @param upper_case_flag whether all values should be transformed into upper case values.
 * @param zone_s Add an extra clause where all matches must be in this zone. If this is <code>NULL</code>
 * it can be ignored.
 * @return The QueryResults from the search.
 */
IRODS_UTIL_API QueryResults *DoMetaSearch (const IrodsSearch * const search_p, rcComm_t *connection_p, int *select_column_ids_p, const int num_select_columns, const bool upper_case_flag, char *zone_s);


/** @} */


#ifdef __cplusplus
}
#endif


#endif /* META_SEARCH_H_ */
