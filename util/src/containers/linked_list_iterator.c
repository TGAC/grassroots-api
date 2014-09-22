#include "linked_list_iterator.h"

typedef struct LinkedListIterator 
{
	Iterator lli_base_iterator;
	ListItem *lli_current_node_p;
} LinkedListIterator;


static void *GetNextItemFromLinkedListIterator (Iterator *iterator_p);


Iterator *GetLinkedListIterator (LinkedList *list_p)
{
	LinkedListIterator *iterator_p = (LinkedListIterator *) AllocMemory (sizeof (LinkedListIterator));
	
	if (iterator_p)
		{
			iterator_p -> lli_base_iterator.it_get_next_item_fn = GetNextItemFromLinkedListIterator;
			iterator_p -> lli_current_node_p = list_p -> ll_head_p;
		}
	
	return iterator_p;
}


void *GetNextItemFromIterator (Iterator *iterator_p)
{
	LinkedListIterator *list_iterator_p = (LinkedListIterator *) iterator_p;
	void *item_p = NULL;
	
	if (lli_current_node_p)
		{
			item_p = lli_current_node_p + sizeof (lli_current_node_p);
			lli_current_node_p = lli_current_node_p -> ln_next_p;
		}
		
	return item_p;
}


WHEATIS_UTIL_API void FreeIterator (Iterator *iterator_p)
{
	FreeMemory (iterator_p);
}
