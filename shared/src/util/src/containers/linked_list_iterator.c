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
			item_p = list_iterator_p -> lli_current_node_p + sizeof (* (list_iterator_p -> lli_current_node_p));
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
