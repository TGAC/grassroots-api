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
 * @file
 * @brief
 */
/** \file linked_list.h
 *  \brief A doubly-linked list.
 *
 *  A doubly-linked list that is the base class for many containers within the program.
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "typedefs.h"
#include "grassroots_util_library.h"



/******** FORWARD DECLARATION ******/
typedef struct ListItem ListItem;

/**
 * A doubly-linked node that points to
 * its predecessor and successor.
 *
 * @ingroup utility_group
 */
struct ListItem
{
	ListItem *ln_prev_p;		/**< A pointer to the previous ListItem */
	ListItem *ln_next_p;		/**< A pointer to the next ListItem */
};


/**
 * A doubly-linked list that can be traversed in
 * either direction.
 *
 * @ingroup utility_group
 */
typedef struct LinkedList
{
	/** A pointer to the first ListItem on this list */
  ListItem *ll_head_p;

  /** A pointer to the last ListItem on this list */
  ListItem *ll_tail_p;

  /** The number of ListItems on this list. */
  uint32 ll_size;

  /** Callback function to use when freeing a ListItem */
  void (*ll_free_node_fn_p) (ListItem * node_p);

} LinkedList;

#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * Create a new LinkedList.
 * The new LinkedList is initialised as an empty list.
 *
 * @param free_node_fn_p The callback function to use when
 * freeing ListItems. If this isn't set the default is to
 * call free ().
 * @return The new LinkedList or NULL upon error.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API LinkedList *AllocateLinkedList (void (*free_node_fn_p) (ListItem * const node_p));


/**
 * Free a LinkedList.
 * Each node is passed to the list's ll_free_node_fn_p or
 * stdlib's free if ll_free_node_fn_p is NULL to be freed.
 * Then the LinkedList is also freed
 *
 * @param list_p The LinkedList to free.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void FreeLinkedList (LinkedList * const list_p);


/**
 * Initialise/clear a LinkedList to be empty.
 *
 * @param list_p The LinkedList to initialise.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void InitLinkedList (LinkedList * const list_p);


/**
 * Add a ListItem to the start of a LinkedList.
 *
 * @param list_p The LinkedList to add the ListItem to.
 * @param node_p The ListItem to add.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void LinkedListAddHead (LinkedList * const list_p, ListItem * const node_p);


/**
 * Add a ListItem to the end of a LinkedList.
 *
 * @param list_p The LinkedList to add the ListItem to.
 * @param node_p The ListItem to add.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void LinkedListAddTail (LinkedList * const list_p, ListItem * const node_p);


/**
 * Remove the first ListItem from a LinkedList.
 *
 * @param list_p The LinkedList to remove the first ListItem from.
 * @return The removed ListItem or NULL if the LinkedList is empty.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API ListItem *LinkedListRemHead (LinkedList * const list_p);


/**
 * Remove the last ListItem from a LinkedList.
 *
 * @param list_p The LinkedList to remove the last ListItem from.
 * @return The removed ListItem or NULL if the LinkedList is empty.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API ListItem *LinkedListRemTail (LinkedList * const list_p);


/**
 * Remove a ListItem from a LinkedList.
 *
 * @param list_p The LinkedList to remove the ListItem from.
 * @param node_p The ListItem to be removed.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void LinkedListRemove (LinkedList * const list_p, ListItem * const node_p);


/**
 * Sort a LinkedList.
 *
 * @param list_p The LinkedList to sort.
 * @param compare_nodes_fn Function used to sort the ListItems.
 * @return
 * It returns < 0 if node1_p should be before node2_p,
 * It returns > 0 if node1_p should be after node2_p,
 * It returns  0 if node1_p is equal to node2_p.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API bool LinkedListSort (LinkedList * const list_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Set the function used to free ListsNodes when the LinkedList is freed.
 *
 * @param list_p The LinkedList to set the function for.
 * @param free_node_fn The function that will be used to free the ListItems.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void SetLinkedListFreeNodeFunction (LinkedList * const list_p, void (*free_node_fn) (ListItem * const node_p));


/**
 * Insert a node into a list using the given sorting algorithm
 * to determine the node's position on the list.
 *
 * @param list_p The LinkedList to insert the node on.
 * @param node_p The ListItem to insert into the list.
 * @param compare_nodes_fn Function used to compare the ListItems.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void LinkedListPrioritisedInsert (LinkedList * const list_p, ListItem * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Insert a node into a list after a given node.
 *
 * @param list_p The LinkedList to insert the node on.
 * @param prev_node_p The ListItem to insert our new node directly after. If this NULL, then our node will
 * be inserted at the start of the list.
 * @param node_to_insert_p The ListItem to insert on the list.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void LinkedListInsert (LinkedList * const list_p, ListItem * const prev_node_p, ListItem * const node_to_insert_p);


/**
 * Search a sorted list using the given sorting algorithm
 * for a given node
 *
 * @param list_p The LinkedList to search.
 * @param node_p The ListItem to search for.
 * @param compare_nodes_fn Function used to compare the ListItems.
 * @param index_p Address where the index of the matching item will be stored
 * if it is found.
 * @return The matching ListItem or <code>NULL</code> if it could not be found on the LinkedList.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API ListItem *LinkedListBinarySearch (const LinkedList * const list_p, const ListItem * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p);


/**
 * Move the contents of one list to another.
 * This function is the equivalent of a "cut and paste" of all of the nodes from one list
 * to another. The nodes are appended to the end of the destination list in the same order in
 * which they appeared in the source list which is then left empty.
 *
 * @param src_list_p The LinkedList to remove all of the nodes from.
 * @param dest_list_p The LinkedList to which the nodes will be appended.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void MoveListContents (LinkedList * const src_list_p, LinkedList * const dest_list_p);


/**
 * Cut a list into two lists at the given index.
 *
 * @param list_p The LinkedList to split.
 * @param split_list_head_index The index of the ListItem that will be the first ListItem
 * on the newly created list, i.e. its ll_head_p.
 * @return The newly created list or NULL if there was an error or if split_list_head_index
 * was not less than the length of list_p.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API LinkedList *SplitList (LinkedList * const list_p, const uint32 split_list_head_index);


/**
 * Empty a LinkedList by freeing all of its ListItems.
 *
 * @param list_p The LinkedList to empty.
 * @memberof LinkedList
 */
GRASSROOTS_UTIL_API void ClearLinkedList (LinkedList * const list_p);


#ifdef __cplusplus
}
#endif

#endif	/* LINKED_LIST_H */


