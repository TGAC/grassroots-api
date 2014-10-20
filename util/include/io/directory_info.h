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
	BaseDirEntry *de_base;
	LinkedList de_entries;
} BaseDirEntry;




typedef DirectoryInfo
{
	
	LinkedList *
}


#endif		/ #ifndef DIRECTORY_INFO_H */
