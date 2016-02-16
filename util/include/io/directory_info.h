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
#ifndef DIRECTORY_INFO_H
#define DIRECTORY_INFO_H

#include "string_linked_list.h"


typedef enum DirEntryType
{
	DET_FILE,

	DET_DIR,

	DET_NUM_TYPES
}  DirEntryType;


typedef struct BaseDirEntry
{
	char *bde_filename_s;
	DirEntryType bde_type;
} BaseDirEntry;


typedef struct BaseDirEntryNode BaseDirEntryNode;

struct BaseDirEntryNode
{
	ListItem bden_node;
	BaseDirEntry *bden_entry_p;
	struct BaseDirEntryNode *bden_parent_p;
};


typedef struct DirEntry
{
	BaseDirEntry de_base;

	/** List of all of the child entries for this dir */
	LinkedList de_entries;
} DirEntry;


GRASSROOTS_UTIL BaseDirEntry *AllocateBaseDirEntry (char *filename_s, DirEntryType entry_type);


GRASSROOTS_UTIL void FreeBaseDirEntry (BaseDirEntry *entry_p);


GRASSROOTS_UTIL DirEntry *AllocateDirEntry (char *filename_s);


GRASSROOTS_UTIL BaseDirEntryNode *AllocateBaseDirEntryNode (char *filename_s, DirEntryType entry_type);


GRASSROOTS_UTIL void FreeBaseDirEntry (ListItems *node_p);


GRASSROOTS_UTIL bool AddDirEntry (DirEntry *parent_entry_p, BaseDirEntry *child_entry_p);



typedef DirectoryInfo
{


};


#endif		/ #ifndef DIRECTORY_INFO_H */
