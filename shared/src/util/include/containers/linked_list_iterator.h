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
#ifndef LINKED_LIST_ITERATOR_H
#define LINKED_LIST_ITERATOR_H

#include "grassroots_util_library.h"
#include "linked_list.h"

struct Iterator;

/**
 * A datatype for abstracting out the method of moving from
 * one entry in a LinkedList to the next. This moves
 * from one ListItem to the next and returns the data
 * starting directly *after* the base ListItem, *not* the
 * ListItem itself.
 *
 * @see LinkedList
 */
typedef struct Iterator 
{
	/**
	 * The callback function to move from one entry in a LinkedList
	 * to the next.
	 * @param iterator_p This iterator.
	 */
	void *(*it_get_next_item_fn) (struct Iterator *iterator_p);

} Iterator;

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Get an Iterator for a LinkedList.
 *
 * @param list_p The LinkedList to get the Iterator for.
 * @return The new Iterator which will be positioned at the
 * beginning of the LinkedList or <code>NULL</code> upon
 * error.
 * @memberof Iterator
 */
GRASSROOTS_UTIL_API Iterator *GetLinkedListIterator (LinkedList *list_p);


/**
 * Get the next entry in the LinkedList associated with an Iterator.
 *
 * @param iterator_p The Iterator to get the next entry from.
 * @return The next entry in the underlying LinkedList or <code>
 * NULL</code> if the end of the LinkedList has been reached. Note that the
 * data returned is that starting directly *after* the base ListItem, *not* the
 * ListItem itself.
 * @memberof Iterator
 */
GRASSROOTS_UTIL_API void *GetNextItemFromIterator (Iterator *iterator_p);


/**
 * Free the Iterator.
 *
 * @param iterator_p The Iterator to free.
 * @memberof Iterator
 */
GRASSROOTS_UTIL_API void FreeIterator (Iterator *iterator_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef LINKED_LIST_ITERATOR_H */
