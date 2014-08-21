/** @file string_linked_list.h
 *  @brief A doubly-linked list of strings
 *
 *  
 */
#ifndef STRING_LINKED_LIST_H
#define STRING_LINKED_LIST_H

#include <stdio.h>

#include "wheatis_util_library.h"
#include "linked_list.h"
#include "memory_allocations.h"


#ifdef __cplusplus
	extern "C" {
#endif


/**
 * A ListNode for LinkedLists that also stores
 * a string value.
 * 
 * @see ListNode ()
 */
typedef struct StringListNode
{
	/** The ListNode. */
	ListNode sln_node;

	/** The string value. */
	char *sln_string_s;

	/** How the memory for the string is stored and freed. */
	MEM_FLAG sln_string_flag;
} StringListNode;


/**
 * Create a new StringListNode.
 *
 * @param str_p The string to store in the newly-created StringListNode.
 * @param mem_flag How the StringListNode should store its string..
 * @return The new StringListNode or NULL upon error.
 */
WHEATIS_UTIL_API StringListNode *AllocateStringListNode (const char * const str_p, const MEM_FLAG mem_flag);


/**
 * Free a StringListNode.
 * Whether the sln_string_p will be freed depends upon the
 * sln_string_flag value.
 *
 * @param node_p The ListNode to free.
 */
WHEATIS_UTIL_API void FreeStringListNode (ListNode * const node_p);


/**
 * Create a LinkedList designed to hold StringListNodes.
 *
 * @return The new LinkedList or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *AllocateStringLinkedList (void);


/**
 * Make a copy of a LinkedList of StringListNodes. For each StringListNode on the source
 * list if sln_string_flag is MF_SHADOW_USE then the new LinkedList will just copy the 
 * address that sln_string_p points to directly. Any other value and the new StringListNodes
 * will make deep copies of the strings.
 *
 * @param src_p The LinkedList of StringListNodes to copy.
 * @result The copied LinkedList or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *CopyStringLinkedList (const LinkedList * const src_p);

#ifdef __cplusplus
}
#endif

#endif	/* #ifndef STRING_LINKED_LIST_H */
