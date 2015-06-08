/** \file linked_list.h
 *  \brief A doubly-linked list.
 *
 *  A doubly-linked list that is the base class for many containers within the program.
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "typedefs.h"
#include "wheatis_util_library.h"



/******** FORWARD DECLARATION ******/
typedef struct ListItem ListItem;

/**
 * A doubly-linked node that points to
 * its predecessor and successor.
 */
struct ListItem
{
	ListItem *ln_prev_p;		/**< A pointer to the previous ListItem */
	ListItem *ln_next_p;		/**< A pointer to the next ListItem */
};


/**
 * A doubly-linked list that can be traversed in
 * either direction.
 */
typedef struct LinkedList
{
	/** A pointer to the first ListNode on this list */
  ListItem *ll_head_p;

  /** A pointer to the last ListNode on this list */
  ListItem *ll_tail_p;

  /** The number of ListNodes on this list. */
  uint32 ll_size;

  /** Callback function to use when freeing a ListNode */
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
 * freeing ListNodes. If this isn't set the default is to
 * call free ().
 * @return The new LinkedList or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *AllocateLinkedList (void (*free_node_fn_p) (ListItem * const node_p));


/**
 * Free a LinkedList.
 * Each node is passed to the list's ll_free_node_fn_p or
 * stdlib's free if ll_free_node_fn_p is NULL to be freed.
 * Then the LinkedList is also freed
 *
 * @param list_p The LinkedList to free.
 */
WHEATIS_UTIL_API void FreeLinkedList (LinkedList * const list_p);


/**
 * Initialise/clear a LinkedList to be empty.
 *
 * @param list_p The LinkedList to initialise.
 */
WHEATIS_UTIL_API void InitLinkedList (LinkedList * const list_p);


/**
 * Add a ListNode to the start of a LinkedList.
 *
 * @param list_p The LinkedList to add the ListNode to.
 * @param node_p The ListNode to add.
 */
WHEATIS_UTIL_API void LinkedListAddHead (LinkedList * const list_p, ListItem * const node_p);


/**
 * Add a ListNode to the end of a LinkedList.
 *
 * @param list_p The LinkedList to add the ListNode to.
 * @param node_p The ListNode to add.
 */
WHEATIS_UTIL_API void LinkedListAddTail (LinkedList * const list_p, ListItem * const node_p);


/**
 * Remove the first ListNode from a LinkedList.
 *
 * @param list_p The LinkedList to remove the first ListNode from.
 * @return The removed ListNode or NULL if the LinkedList is empty.
 */
WHEATIS_UTIL_API ListItem *LinkedListRemHead (LinkedList * const list_p);


/**
 * Remove the last ListNode from a LinkedList.
 *
 * @param list_p The LinkedList to remove the last ListNode from.
 * @return The removed ListNode or NULL if the LinkedList is empty.
 */
WHEATIS_UTIL_API ListItem *LinkedListRemTail (LinkedList * const list_p);


/**
 * Remove a ListNode from a LinkedList.
 *
 * @param list_p The LinkedList to remove the ListNode from.
 * @param node_p The ListNode to be removed.
 */
WHEATIS_UTIL_API void LinkedListRemove (LinkedList * const list_p, ListItem * const node_p);


/**
 * Sort a LinkedList.
 *
 * @param list_p The LinkedList to sort.
 * @param compare_nodes_fn Function used to sort the ListNodes.
 * @return
 * It returns < 0 if node1_p should be before node2_p,
 * It returns > 0 if node1_p should be after node2_p,
 * It returns  0 if node1_p is equal to node2_p.
 */
WHEATIS_UTIL_API bool LinkedListSort (LinkedList * const list_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Set the function used to free ListsNodes when the LinkedList is freed.
 *
 * @param list_p The LinkedList to set the function for.
 * @param free_node_fn The function that will be used to free the ListNodes.
 */
WHEATIS_UTIL_API void SetLinkedListFreeNodeFunction (LinkedList * const list_p, void (*free_node_fn) (ListItem * const node_p));


/**
 * Insert a node into a list using the given sorting algorithm
 * to determine the node's position on the list.
 *
 * @param list_p The LinkedList to insert the node on.
 * @param node_p The ListNode to insert into the list.
 * @param compare_nodes_fn Function used to compare the ListNodes.
 */
WHEATIS_UTIL_API void LinkedListPrioritisedInsert (LinkedList * const list_p, ListItem * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Insert a node into a list after a given node.
 *
 * @param list_p The LinkedList to insert the node on.
 * @param prev_node_p The ListNode to insert our new node directly after. If this NULL, then our node will
 * be inserted at the start of the list.
 * @param node_to_insert_p The ListNode to insert on the list.
 */
WHEATIS_UTIL_API void LinkedListInsert (LinkedList * const list_p, ListItem * const prev_node_p, ListItem * const node_to_insert_p);


/**
 * Search a sorted list using the given sorting algorithm
 * for a given node
 *
 * @param list_p The LinkedList to search.
 * @param node_p The ListNode to search for.
 * @param compare_nodes_fn Function used to compare the ListNodes.
 * @param index_p Address where the index of the matching item will be stored
 * if it is found.
 * @return The matching ListNode or <code>NULL</code> if it could not be found on the LinkedList.
 */
WHEATIS_UTIL_API ListItem *LinkedListBinarySearch (const LinkedList * const list_p, const ListItem * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p);


/**
 * Move the contents of one list to another.
 * This function is the equivalent of a "cut and paste" of all of the nodes from one list
 * to another. The nodes are appended to the end of the destination list in the same order in
 * which they appeared in the source list which is then left empty.
 *
 * @param src_list_p The LinkedList to remove all of the nodes from.
 * @param dest_list_p The LinkedList to which the nodes will be appended.
 */
WHEATIS_UTIL_API void MoveListContents (LinkedList * const src_list_p, LinkedList * const dest_list_p);


/**
 * Cut a list into two lists at the given index.
 *
 * @param list_p The LinkedList to split.
 * @param split_list_head_index The index of the ListNode that will be the first ListNode
 * on the newly created list, i.e. its ll_head_p.
 * @return The newly created list or NULL if there was an error or if split_list_head_index
 * was not less than the length of list_p.
 */
WHEATIS_UTIL_API LinkedList *SplitList (LinkedList * const list_p, const uint32 split_list_head_index);


/**
 * Empty a LinkedList by freeing all of its ListNodes.
 *
 * @param list_p The LinkedList to empty.
 */
WHEATIS_UTIL_API void ClearLinkedList (LinkedList * const list_p);


#ifdef __cplusplus
}
#endif

#endif	/* LINKED_LIST_H */


