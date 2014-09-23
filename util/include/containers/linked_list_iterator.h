#ifndef LINKED_LIST_ITERATOR_H
#define LINKED_LIST_ITERATOR_H

#include "wheatis_util_library.h"
#include "linked_list.h"

struct Iterator;

typedef struct Iterator 
{
	void *(*it_get_next_item_fn) (struct Iterator *iterator_p);
} Iterator;

#ifdef __cplusplus
	extern "C" {
#endif


WHEATIS_UTIL_API Iterator *GetLinkedListIterator (LinkedList *list_p);


WHEATIS_UTIL_API void *GetNextItemFromIterator (Iterator *iterator_p);


WHEATIS_UTIL_API void FreeIterator (Iterator *iterator_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef LINKED_LIST_ITERATOR_H */
