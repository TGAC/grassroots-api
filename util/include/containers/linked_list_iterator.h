/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#ifndef LINKED_LIST_ITERATOR_H
#define LINKED_LIST_ITERATOR_H

#include "grassroots_util_library.h"
#include "linked_list.h"

struct Iterator;

typedef struct Iterator 
{
	void *(*it_get_next_item_fn) (struct Iterator *iterator_p);
} Iterator;

#ifdef __cplusplus
	extern "C" {
#endif


GRASSROOTS_UTIL_API Iterator *GetLinkedListIterator (LinkedList *list_p);


GRASSROOTS_UTIL_API void *GetNextItemFromIterator (Iterator *iterator_p);


GRASSROOTS_UTIL_API void FreeIterator (Iterator *iterator_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef LINKED_LIST_ITERATOR_H */
