#include "linked_list_iterator.h"

#include "memory_allocations.h"


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
	
	return ((Iterator *) iterator_p);
}


static void *GetNextItemFromLinkedListIterator (Iterator *iterator_p)
{
	LinkedListIterator *list_iterator_p = (LinkedListIterator *) iterator_p;
	void *item_p = NULL;
	
	if (list_iterator_p -> lli_current_node_p)
		{
			item_p = list_iterator_p -> lli_current_node_p + sizeof (list_iterator_p -> lli_current_node_p);
			list_iterator_p -> lli_current_node_p = list_iterator_p -> lli_current_node_p -> ln_next_p;
		}
		
	return item_p;
}


void *GetNextItemFromIterator (Iterator *iterator_p)
{
	return (iterator_p -> it_get_next_item_fn (iterator_p));
}

void FreeIterator (Iterator *iterator_p)
{
	FreeMemory (iterator_p);
}
