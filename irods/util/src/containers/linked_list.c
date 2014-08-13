#include <stdlib.h>

#include "memory_allocations.h"

#include "linked_list.h"


#ifdef _DEBUG
	#define LINKED_LIST_DEBUG (STM_LEVEL_INFO)
#else
	#define LINKED_LIST_DEBUG (STM_LEVEL_NONE)
#endif


static void FreeNode (ListNode  *node_p);



LinkedList *AllocateArrayOfLinkedLists (const uint32 num_lists, void (*free_node_fn_p) (ListNode * const node_p))
{
	LinkedList *lists_p = (LinkedList *) AllocMemory (num_lists * sizeof (LinkedList));

	if (lists_p)
		{
			LinkedList *list_p = lists_p;
			uint32 i = num_lists;

			while (i > 0)
				{
					list_p -> ll_free_node_fn_p = free_node_fn_p;
					InitLinkedList (list_p);

					++ list_p;
					-- i;
				}
		}

	return lists_p;
}



void SetLinkedListFreeNodeFunction (LinkedList * const list_p, void (*free_node_fn) (ListNode * const node_p))
{
	list_p -> ll_free_node_fn_p = free_node_fn;
}



LinkedList *AllocateLinkedList (void (*free_node_fn_p) (ListNode * const node_p))
{
	LinkedList *list_p = (LinkedList *) AllocMemory (sizeof (LinkedList));

	if (list_p)
		{
			list_p -> ll_free_node_fn_p = free_node_fn_p;
			list_p -> ll_size = 0;
			InitLinkedList (list_p);

			return list_p;
		}
	else
		{
			return NULL;
		}
}


void FreeLinkedList (LinkedList * const list_p)
{
	ClearLinkedList (list_p);
	FreeMemory (list_p);
}


void ClearLinkedList (LinkedList * const list_p)
{
	void (*free_node_fn_p) (ListNode * node_p) = list_p -> ll_free_node_fn_p;
	ListNode *this_node_p = list_p -> ll_head_p;
	ListNode *next_node_p = NULL;

	/* if we don't have a given function to free the nodes
	** use our generic function instead.
	*/
	if (free_node_fn_p == NULL)
		{
			free_node_fn_p = (void (*) (ListNode *)) FreeNode;
		}

	while (this_node_p != NULL)
		{
			next_node_p = this_node_p -> ln_next_p;

			free_node_fn_p (this_node_p);

			this_node_p = next_node_p;
		}

	list_p -> ll_head_p = NULL;
	list_p -> ll_tail_p = NULL;
	list_p -> ll_size = 0;
}


static void FreeNode (ListNode  *node_p)
{
	FreeMemory (node_p);
}



void InitLinkedList (LinkedList * const list_p)
{
	if (list_p -> ll_size > 0)
		{
			ClearLinkedList (list_p);
		}

	list_p -> ll_head_p = NULL;
	list_p -> ll_tail_p = NULL;
	list_p -> ll_size = 0;
}


void LinkedListAddHead (LinkedList * const list_p, ListNode * const node_p)
{
	if (list_p -> ll_size != 0)
		{
			ListNode *old_head_p = list_p -> ll_head_p;
			list_p -> ll_head_p = node_p;

			old_head_p -> ln_prev_p = node_p;
			node_p -> ln_next_p = old_head_p;

			node_p -> ln_prev_p = NULL;
		}
	else
		{
			list_p -> ll_head_p = list_p -> ll_tail_p = node_p;
			node_p -> ln_next_p = node_p -> ln_prev_p = NULL;
		}

	++ (list_p -> ll_size);
}


void LinkedListAddTail (LinkedList * const list_p, ListNode * const node_p)
{
	if (list_p -> ll_size != 0)
		{
			ListNode *old_tail_p = list_p -> ll_tail_p;
			list_p -> ll_tail_p = node_p;

			old_tail_p -> ln_next_p = node_p;
			node_p -> ln_prev_p = old_tail_p;

			node_p -> ln_next_p = NULL;
		}
	else
		{
			list_p -> ll_head_p = list_p -> ll_tail_p = node_p;
			node_p -> ln_next_p = node_p -> ln_prev_p = NULL;
		}

	++ (list_p -> ll_size);
}


void LinkedListInsert (LinkedList * const list_p, ListNode * const prev_node_p, ListNode * const node_to_insert_p)
{
	if (prev_node_p != NULL)
		{
			ListNode *next_node_p = prev_node_p -> ln_next_p;

			node_to_insert_p -> ln_prev_p = prev_node_p;
			prev_node_p -> ln_next_p = node_to_insert_p;

			node_to_insert_p -> ln_next_p = next_node_p;

			if (next_node_p)
				{
					next_node_p -> ln_prev_p = node_to_insert_p;
				}
			else
				{
					/*
						If next_node_p is NULL then prev_node_p must
						be the tail of the list, so we need to update
						the list structure.
					*/
					list_p -> ll_tail_p = node_to_insert_p;
				}

			++ (list_p -> ll_size);
		}
	else
		{
			LinkedListAddHead (list_p, node_to_insert_p);
		}

}


ListNode *LinkedListRemHead (LinkedList * const list_p)
{
	ListNode *node_p = NULL;

	switch (list_p -> ll_size)
		{
			case 0:
				break;

			case 1:
				node_p = list_p -> ll_head_p;

				list_p -> ll_head_p = list_p -> ll_tail_p = NULL;
				list_p -> ll_size = 0;

				node_p -> ln_next_p = NULL;
				break;

			default:
				{
					ListNode *new_head_p;

					node_p = list_p -> ll_head_p;
					new_head_p = node_p -> ln_next_p;

					node_p -> ln_next_p = NULL;
					new_head_p -> ln_prev_p = NULL;
					list_p -> ll_head_p = new_head_p;

					-- (list_p -> ll_size);
					break;
				}
		}

	return node_p;
}


ListNode *LinkedListRemTail (LinkedList * const list_p)
{
	ListNode *node_p = NULL;

	switch (list_p -> ll_size)
		{
			case 0:
				break;

			case 1:
				node_p = list_p -> ll_tail_p;

				list_p -> ll_head_p = list_p -> ll_tail_p = NULL;
				list_p -> ll_size = 0;

				node_p -> ln_prev_p = NULL;
				break;

			default:
				{
					ListNode *new_tail_p;

					node_p = list_p -> ll_tail_p;
					new_tail_p = node_p -> ln_prev_p;

					node_p -> ln_prev_p = NULL;
					new_tail_p -> ln_next_p = NULL;

					list_p -> ll_tail_p = new_tail_p;

					-- (list_p -> ll_size);
					break;
				}
		}

	return node_p;
}


void LinkedListRemove (LinkedList * const list_p, ListNode * const node_p)
{

	if (list_p -> ll_size == 0)
		{
			return;
		}

  if (list_p -> ll_head_p == node_p)
    {
      LinkedListRemHead (list_p);
    }
  else if (list_p -> ll_tail_p == node_p)
    {
      LinkedListRemTail (list_p);
    }
  else
    {
      ListNode *prev_p = node_p -> ln_prev_p;
      ListNode *next_p = node_p -> ln_next_p;

      prev_p -> ln_next_p = next_p;
      next_p -> ln_prev_p = prev_p;

			node_p -> ln_next_p = node_p -> ln_prev_p = NULL;

      -- (list_p -> ll_size);
    }
}


bool LinkedListSort (LinkedList * const list_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p))
{
	const uint32 num_nodes = list_p -> ll_size;

	if (num_nodes > 1)
		{
			ListNode **node_index_pp = (ListNode **) AllocMemoryArray (num_nodes, sizeof (ListNode *));

			if (node_index_pp)
        {
					ListNode **index_pp = node_index_pp;
					ListNode *node_p = list_p -> ll_head_p;
					uint32 i;

					/* Cache the nodes... */
					for (i = num_nodes; i > 0; -- i, ++ index_pp)
						{
							*index_pp = node_p;
							node_p = node_p -> ln_next_p;
						}

          /* ... sort the cached nodes... */
          qsort (node_index_pp, num_nodes, sizeof (ListNode *), compare_nodes_fn);

          /* ... and then reinsert them into the LinkedList in sorted order */
					list_p -> ll_head_p = NULL;
					list_p -> ll_tail_p = NULL;
					list_p -> ll_size = 0;

          index_pp = node_index_pp;
					for (i = num_nodes; i > 0; -- i, ++ index_pp)
						{
							LinkedListAddTail (list_p, *index_pp);
						}

          FreeMemory (node_index_pp);
        }
			else
				{
					return false;
				}

		}

	return true;
}

void LinkedListPrioritisedInsert (LinkedList * const list_p, ListNode * const node_to_insert_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p))
{
	if (list_p -> ll_size > 0)
		{
			int index;
			ListNode *node_p = LinkedListBinarySearch (list_p, node_to_insert_p, compare_nodes_fn, &index);

			/* node_to_insert_p is not on our list */
			if (index < 0)
				{
					uint32 i = (uint32) (- (index + 1));

					if (i < list_p -> ll_size)
						{
							if (i < ((list_p -> ll_size) >> 1))
								{
									node_p = list_p -> ll_head_p;

									while (i > 0)
										{
											node_p = node_p -> ln_next_p;
											-- i;
										}
								}
							else
								{
									node_p = list_p -> ll_tail_p;

									i -= (list_p -> ll_size) >> 1;

									while (i > 0)
										{
											node_p = node_p -> ln_prev_p;
											-- i;
										}
								}

							if (node_p)
								{
									LinkedListInsert (list_p, node_p, node_to_insert_p);
								}
							else
								{
									//PrintErrors (STM_LEVEL_SEVERE, "Failed to get prioritised insert postion\n");
								}
						}
					else
						{
							LinkedListAddTail (list_p, node_to_insert_p);
						}

				}		/* if (index < 0) */
		
		}		/* if (list_p -> ll_size > 0) */
	else
		{
			LinkedListAddHead (list_p, node_to_insert_p);
		}
}


ListNode *SearchUnsortedLinkedList (const LinkedList * const list_p, const ListNode * const node_to_search_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p)
{
	ListNode *node_p = list_p -> ll_head_p;

	while (node_p)
		{
			if (compare_nodes_fn (node_p, node_to_search_p) == 0)
				{
					return node_p;
				}
			else
				{
					node_p = node_p -> ln_next_p;
				}
		}		/* while (loop_flag) */

	return NULL;
}


ListNode *NewLinkedListBinarySearch (const LinkedList * const list_p, const ListNode * const node_to_search_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p)
{
	ListNode *node_p = list_p -> ll_head_p;
  int low = 0;
	int high = (int) ((list_p -> ll_size) - 1);
	int i;

	while (low <= high)
		{
			int mid = (low + high) >> 1;
			int result;

			node_p = list_p -> ll_head_p;

			for (i = 0; i < mid; ++ i)
				{
					node_p = node_p -> ln_next_p;
				}

			result = compare_nodes_fn (node_to_search_p, node_p);

			if (result < 0)
				{
					low = mid + 1;
				}
			else if (result > 0)
				{
					high = mid - 1;
				}
			else
				{
					*index_p = mid;
					return node_p;
				}
		}

	*index_p = - (low + 1);  // key not found.
	return NULL;
}


ListNode *LinkedListBinarySearch (const LinkedList * const list_p, const ListNode * const node_to_search_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p)
{
	ListNode *node_p = list_p -> ll_head_p;
	int low = 0;
	int high = ((int) (list_p -> ll_size)) - 1;
	int middle = 0;
	int result = 0;
	bool loop_flag = true;
	bool first_time_flag = true;

	while (loop_flag)
		{
			/* scroll to the new middle node */
			int new_middle = (low + high) >> 1;

			if (new_middle > middle)
				{
					uint32 i = new_middle - middle;

					while (i > 0)
						{
							node_p = node_p -> ln_next_p;
							-- i;
						}
				}
			else if (new_middle < middle)
				{
					uint32 i = middle - new_middle;

					while (i > 0)
						{
							node_p = node_p -> ln_prev_p;
							-- i;
						}
				}
			else
				{
					if (!first_time_flag)
						{
							/* we haven't moved from the last iteration, so finish */
							loop_flag = false;		/* force exit from loop */
						}
				}

			if (loop_flag)
				{
					middle = new_middle;

					result = compare_nodes_fn (node_to_search_p, node_p);

					if (result == 0)
						{
							loop_flag = false;

							if (index_p)
								{
									*index_p = middle;
								}
						}
					else if (result > 0)
						{
							high = middle - 1;
						}
					else if (result < 0)
						{
							low = middle + 1;
						}

					loop_flag = (low <= high);
				}		/* if (loop_flag) */

			first_time_flag = false;
		}		/* while (loop_flag) */


	if (result < 0)
		{
			if (index_p)
				{
					*index_p = - middle - 1;
				}
		}
	else if (result > 0)
		{
			node_p = node_p -> ln_next_p;

			if (index_p)
				{
					*index_p = - middle - 2;
				}
		}

	return node_p;
}



void MoveListContents (LinkedList * const src_list_p, LinkedList * const dest_list_p)
{
	ListNode *src_node_p = src_list_p -> ll_head_p;
	ListNode *dest_node_p = dest_list_p -> ll_tail_p;

	/* join the list contents */
	dest_node_p -> ln_next_p = src_node_p;
	src_node_p -> ln_prev_p = dest_node_p;

	/* update the dest list fields. */
	dest_list_p -> ll_tail_p = src_list_p -> ll_tail_p;
	dest_list_p -> ll_size += src_list_p -> ll_size;

	/* clear the source list */
	src_list_p -> ll_head_p = NULL;
	src_list_p -> ll_tail_p = NULL;
	src_list_p -> ll_size = 0;
}


/** 
 * Split a LinkedList into two sections.
 *
 * @param list_p The LinkedList to split.
 * @param split_list_length The length new LinkedList.
 * @return The newly created LinkedList or NULL upon error. If this is non-NULL then list_p will
 * have been reset so that its ll_head_p points to the node following ll_tail_p of the newly
 * created LinkedList and its ll_size will be decremented by split_list_length.
 */
LinkedList *SplitList (LinkedList * const list_p, const uint32 split_list_length)
{
	#if LINKED_LIST_DEBUG >= STM_LEVEL_FINEST
		{
			uint32 i = 0;
			ListNode *node_p = list_p -> ll_head_p;

			while (node_p)
				{
					++ i;
					node_p = node_p -> ln_next_p;
				}		/* while (node_p) */

			//PrintLog (STM_LEVEL_FINEST, "split list length: %lu list size: %lu explicit size: %lu\n", split_list_length, list_p -> ll_size, i);
		}
	#endif

	if ((split_list_length > 0) && (split_list_length < list_p -> ll_size))
		{
			LinkedList *split_list_p = AllocateLinkedList (list_p -> ll_free_node_fn_p);

			if (split_list_p)
				{
					uint32 i = split_list_length - 1;
					ListNode *split_tail_p = list_p -> ll_head_p;
					ListNode *node_p = NULL;

					while (i > 0)
						{
							split_tail_p = split_tail_p -> ln_next_p;
							-- i;
						}

					/* Detach split_tail_p from its successor node */
					if (split_tail_p)
						{
							node_p = split_tail_p -> ln_next_p;
							split_tail_p -> ln_next_p = NULL;

							if (node_p)
								{
									node_p -> ln_prev_p = NULL;
								}		/* if (node_p) */

						}		/* if (split_tail_p) */

					split_list_p -> ll_head_p = list_p -> ll_head_p;
					split_list_p -> ll_tail_p = split_tail_p;
					split_list_p -> ll_size = split_list_length;

					list_p -> ll_head_p = node_p;
					list_p -> ll_size -= split_list_length;

					return split_list_p;
				}		/* if (split_list_p) */

		}		/* if ((split_list_length > 0) && (split_list_length < list_p -> ll_size)) */

	return NULL;
}
