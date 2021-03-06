/*
** Copyright 2014-2016 The Earlham Institute
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

/**
 * @file meta_search.h
 *
 *  Created on: 2 Feb 2015
 *      Author: tyrrells
 *
 *  @brief A set of routines to perform a metadata-based search
 *  of an iRODS server. It uses the iCAT functionality
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
 * @ingroup irods_group
 */
typedef struct IRodsSearch
{
	/**
	 * A LinkedList of SearchTermNodes
	 * @see SearchTermNode
	 */
	LinkedList *is_search_terms_p;
} IRodsSearch;


/**
 * @brief A datatype for storing the values needed for an iRODS search.
 *
 * A SearchTerm contains the values needed to build an SQL-style query
 * to run on an iRODS server.
 *
 * @ingroup irods_group
 */
typedef struct SearchTerm
{
	/**
	 * @private
	 *
	 * The clause used to join this SearchTerm to others.
	 * this can be "AND", "OR", etc.
	 */
	const char *st_clause_s;

	/**
	 * @private
	 *
	 * The id of the column used for the key.
	 */
	int st_key_column_id;

	/**
	 * @private
	 *
	 * The key in the key-value pair for this SearchTerm.
	 */
	const char *st_key_s;


	/**
	 * @private
	 *
	 * The comparison operator used to compare the key and value of this SearchTerm
	 * such as "=", "<", <i>etc.</i>
	 */
	const char *st_op_s;


	/**
	 * @private
	 *
	 * The id of the column used for the value.
	 */
	int st_value_column_id;

	/**
	 * @private
	 *
	 * The value in the key-value pair for this SearchTerm.
	 */
	const char *st_value_s;

	/**
	 * @private
	 * The internal storage for the storing the key in the
	 * format that iRODS expects for the search.
	 */
	char *st_key_buffer_s;

	/**
	 * @private
	 * The internal storage for the storing the value in the
	 * format that iRODS expects for the search.
	 */
	char *st_value_buffer_s;
} SearchTerm;


/**
 * A datatype for storing a SearchTerm on a LinkedList.
 *
 * @extends ListItem
 * @ingroup irods_group
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
 * Allocate an IRodsSearch.
 *
 * @return A newly-allocated IRodsSearch or <code>NULL</code> upon error.
 * @memberof IRodsSearch
 */
IRODS_UTIL_API IRodsSearch *AllocateIRodsSearch (void);


/**
 * Free an IRodsSearch.
 *
 * @param search_p The IRodsSearch to free.
 * @memberof IRodsSearch
 */
IRODS_UTIL_API void FreeIRodsSearch (IRodsSearch *search_p);


/**
 * Clear an IRodsSearch.
 *
 * @param search_p The IRodsSearch to clear.
 * @memberof IRodsSearch
 */
IRODS_UTIL_API void ClearIRodsSearch (IRodsSearch *search_p);




/**
 * Perform an iRODS search.
 *
 * @param search_p The IRodsSearch to perform.
 * @param connection_p The connection to the iRODS server to perform the search on.
 * @return The QueryResults from the search.
 * @memberof IRodsSearch
 */
IRODS_UTIL_API QueryResults *DoIRodsSearch (IRodsSearch *search_p, struct IRodsConnection *connection_p);


/**
 * Create and add a SearchTerm to an IRodsSearch.
 *
 * This adds a search term in the form of a key op value triplet as described in
 * AddIRodsSearchTerm
 *
 * @param search_p The IRodsSearch to add the SearchTerm to.
 * @param clause_s The search term's clause.
 * @param key_s The search term's key.
 * @param op_s The search term's operation such as "=", "<", <i>etc.</i>
 * @param value_s The search term's value.
 * @return <code>true</code> if the search term was added successfully, <code>false</code> otherwise.
 * @memberof IRodsSearch
 * @see AddIRodsSearchTerm
 */
IRODS_UTIL_API bool AddMetadataDataAttributeSearchTerm (IRodsSearch *search_p, const char *clause_s, const char *key_s, const char *op_s, const char *value_s);



/**
 * Add a search term to an IRodsSearch.
 *
 * This adds a search term in the form of a key op value triplet. For example
 * key could be "foo", value = "bar" and op = "="
 * @param search_p The IRodsSearch to add the term to.
 * @param clause_s The search term's clause.
 * @param key_s The search term's key.
 * @param key_id The id of the key's column.
 * @param op_s The search term's operation such as "=", "<", <i>etc.</i>
 * @param value_s The search term's value.
 * @param value_id The id of the value's column.
 * @return <code>true</code> if the search term was added successfully, <code>false</code> otherwise.
 * @memberof IRodsSearch
 */
IRODS_UTIL_API bool AddIRodsSearchTerm (IRodsSearch *search_p, const char *clause_s, const char *key_s, const int key_id, const char *op_s, const char *value_s, const int value_id);


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
 * @memberof IRodsSearch
 */
IRODS_UTIL_API int32 DetermineSearchTerms (LinkedList *terms_p, const json_t *json_p);


/**
 * Perform a meta-based search
 *
 * @param search_p The IRodsSearch to perform.
 * @param connection_p The connection to the iRODS server to perform the search on.
 * @param select_column_ids_p An array of the column ids to use for the select clause.
 * @param num_select_columns The number of ids pointed to by select_column_ids_p.
 * @param upper_case_flag whether all values should be transformed into upper case values.
 * @param zone_s Add an extra clause where all matches must be in this zone. If this is <code>NULL</code>
 * it can be ignored.
 * @return The QueryResults from the search.
 */
IRODS_UTIL_API QueryResults *DoMetaSearch (const IRodsSearch * const search_p, struct IRodsConnection *connection_p, int *select_column_ids_p, const int num_select_columns, const bool upper_case_flag, char *zone_s);




/**
 * Perform a meta-based search for all matching Data Objects and Collections.
 *
 * @param search_p The IRodsSearch to perform.
 * @param connection_p The connection to the iRODS server to perform the search on.
 * @param upper_case_flag whether all values should be transformed into upper case values.
 * @param zone_s Add an extra clause where all matches must be in this zone. If this is <code>NULL</code>
 * it can be ignored.
 * @return The QueryResults from the search.
 */
IRODS_UTIL_API QueryResults *DoMetaSearchForAllDataAndCollections (const IRodsSearch * const search_p, struct IRodsConnection *connection_p, const bool upper_case_flag, char *zone_s);


#ifdef __cplusplus
}
#endif


#endif /* META_SEARCH_H_ */
