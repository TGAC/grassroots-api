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
/** @file string_linked_list.h
 *  @brief A doubly-linked list of strings
 *
 *  
 */
#ifndef STRING_LINKED_LIST_H
#define STRING_LINKED_LIST_H

#include <stdio.h>

#include "grassroots_util_library.h"
#include "linked_list.h"
#include "memory_allocations.h"


#ifdef __cplusplus
	extern "C" {
#endif


/**
 * A ListNode for LinkedLists that also stores
 * a string value.
 * 
 * @extends ListItem
 *
 * @ingroup utility_group
 */
typedef struct StringListNode
{
	/** The ListNode. */
	ListItem sln_node;

	/** The string value. */
	char *sln_string_s;

	/** How the memory for the string is stored and freed. */
	MEM_FLAG sln_string_flag;
} StringListNode;


/**
 * Create a new StringListNode.
 *
 * @param str_p The string to store in the newly-created StringListNode.
 * @param mem_flag How the StringListNode should store its string.
 * @return The new StringListNode or <code>NULL</code> upon error.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API StringListNode *AllocateStringListNode (const char * const str_p, const MEM_FLAG mem_flag);



/**
 * Initialise a StringListNode.
 *
 * @param node_p The StringListNode to initialise.
 * @param value_s The string to store in the StringListNode.
 * @param mem_flag How the StringListNode should store its string.
 * @return If successful, any previous value stored in the given StringListNode will be
 * freed based upon the mem_flag and the new value will be set. The return code will be
 * <code>true</code>. Upon failure, the StringListNode is unaltered and <code>false</code>
 * is returned.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API bool InitStringListNode (StringListNode *node_p, const char * const value_s, const MEM_FLAG mem_flag);


/**
 * Free a StringListNode.
 * Whether the sln_string_p will be freed depends upon the
 * sln_string_flag value.
 *
 * @param node_p The ListNode to free.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API void FreeStringListNode (ListItem * const node_p);


/**
 * Clear a StringListNode by deleting its sln_string_s value as appropriate.
 *
 * @param node_p The StringListNode to clear.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API void ClearStringListNode (StringListNode *node_p);


/**
 * Create a LinkedList designed to hold StringListNodes.
 *
 * @return The new LinkedList or <code>NULL</code> upon error.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API LinkedList *AllocateStringLinkedList (void);


/**
 * Make a copy of a LinkedList of StringListNodes. For each StringListNode on the source
 * list if sln_string_flag is MF_SHADOW_USE then the new LinkedList will just copy the 
 * address that sln_string_p points to directly. Any other value and the new StringListNodes
 * will make deep copies of the strings.
 *
 * @param src_p The LinkedList of StringListNodes to copy.
 * @result The copied LinkedList or <code>NULL</code> upon error.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API LinkedList *CopyStringLinkedList (const LinkedList * const src_p);


/**
 * Get a new string that is the concatenation of all of the entries on a 
 * StringLinkedList.
 * 
 * @param src_p The LinkedList of StringListNodes to generate the value from.
 * @return The newly-allocated string or <code>NULL</code> upon error.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API char *GetStringLinkedListAsString (const LinkedList * const src_p);



/**
 * Create and add a new StringListNode to the tail of a LinkedList.
 *
 * @param list_p The List to add the node to the end of.
 * @param str_p The string to store in the newly-created StringListNode.
 * @param mem_flag How the StringListNode should store its string..
 * @return <code>true</code> upon success, <code>false</code> on error.
 * @memberof StringListNode
 */
GRASSROOTS_UTIL_API bool AddStringToStringLinkedList (LinkedList *list_p, const char * const str_p, const MEM_FLAG mem_flag);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef STRING_LINKED_LIST_H */

