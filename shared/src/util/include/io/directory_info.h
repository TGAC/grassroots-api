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
#ifndef DIRECTORY_INFO_H
#define DIRECTORY_INFO_H

#include "string_linked_list.h"


typedef enum DirEntryType
{
	DET_FILE,

	DET_DIR,

	DET_NUM_TYPES
}  DirEntryType;


/**
 * The datatype used for storing directory listings.
 */
typedef struct BaseDirEntry
{
	/** The name of this directory, file, link, etc. */
	char *bde_filename_s;

	/** The type of this entry. */
	DirEntryType bde_type;
} BaseDirEntry;


typedef struct BaseDirEntryNode BaseDirEntryNode;

/**
 * A datatype for storing directory entries
 * on a LinkedList.
 *
 * @extends ListItem
 */
struct BaseDirEntryNode
{
	/** The base node. */
	ListItem bden_node;

	/** The directory entry to store on this node. */
	BaseDirEntry *bden_entry_p;

	/**
	 * The parent of this node or <code>NULL</code>
	 * if it is the root node.
	 */
	BaseDirEntryNode *bden_parent_p;
};


/**
 * A datatype for storing a directory.
 *
 * @extends BaseDirEntry
 */
typedef struct DirEntry
{
	/** The base BaseDirEntry */
	BaseDirEntry de_base;

	/** List of all of the child entries for this dir */
	LinkedList de_entries;
} DirEntry;


GRASSROOTS_UTIL_API BaseDirEntry *AllocateBaseDirEntry (char *filename_s, DirEntryType entry_type);


GRASSROOTS_UTIL_API void FreeBaseDirEntry (BaseDirEntry *entry_p);


GRASSROOTS_UTIL_API DirEntry *AllocateDirEntry (char *filename_s);


GRASSROOTS_UTIL_API BaseDirEntryNode *AllocateBaseDirEntryNode (char *filename_s, DirEntryType entry_type);


GRASSROOTS_UTIL_API void FreeBaseDirEntry (ListItem *node_p);


GRASSROOTS_UTIL_API bool AddDirEntry (DirEntry *parent_entry_p, BaseDirEntry *child_entry_p);


#endif		/* #ifndef DIRECTORY_INFO_H */
