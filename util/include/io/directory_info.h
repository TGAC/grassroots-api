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


WHEATIS_UTIL BaseDirEntry *AllocateBaseDirEntry (char *filename_s, DirEntryType entry_type);


WHEATIS_UTIL void FreeBaseDirEntry (BaseDirEntry *entry_p);


WHEATIS_UTIL DirEntry *AllocateDirEntry (char *filename_s);


WHEATIS_UTIL BaseDirEntryNode *AllocateBaseDirEntryNode (char *filename_s, DirEntryType entry_type);


WHEATIS_UTIL void FreeBaseDirEntry (ListItems *node_p);


WHEATIS_UTIL bool AddDirEntry (DirEntry *parent_entry_p, BaseDirEntry *child_entry_p);



typedef DirectoryInfo
{

	LinkedList *
}


#endif		/ #ifndef DIRECTORY_INFO_H */
