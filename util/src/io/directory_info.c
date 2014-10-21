#include "directory_info.h"




BaseDirEntry *AllocateBaseDirEntry (char *filename_s, DirEntryType entry_type, BaseDirEntryNode *parent_node_p)
{
	char *copied_filename_s = CopyToNewString (filename_s, 0, false);

	if (copied_filename_s)
		{
			BaseDirEntry *entry_p = NULL;

			switch (entry_type)
				{
					case DET_FILE:
						{
							entry_p = (BaseDirEntry *) AllocMemory (sizeof (BaseDirEntry));
						}
						break;

					case DET_DIR:
						{
							DirEntry *dir_entry_p = (DirEntry *) AllocMemory (sizeof (DirEntry));

							if (dir_entry_p)
								{
									InitLinkedList (& (dir_entry_p -> de_entries), FreeBaseDirEntryNode);
									entry_p = & (dir_entry_p -> de_base);
								}
						}
						break;

					default:
						break;
				}

			if (entry_p)
				{
					entry_p -> bde_filename_s = copied_filename_s;
					entry_p -> bde_type = entry_type;
					entry_p -> bden_parent_p = parent_node_p;
					
					return entry_p;
				}		/* if (entry_p) */

		}		/* if (copied_filename_s) */

	return NULL;
}


void FreeBaseDirEntry (BaseDirEntry *entry_p)
{
	switch (entry_p -> bde_type)
		{
			case DET_DIR:
				FreeLinkedList (& (dir_entry_p -> de_entries));

			/* deliberate drop through */
			case DET_FILE:
				FreeCopiedString (entry_p -> bde_filename_s);
				break;

			default:
				break;
		}

	FreeMemory (entry_p);
}



BaseDirEntryNode *AllocateBaseDirEntryAndNode (char *filename_s, DirEntryType entry_type, BaseDirEntryNode *parent_node_p)
{
	BaseDirEntry *entry_p = AllocateBaseDirEntry (filename_s, entry_type, parent_node_p);
	
	if (entry_p)
		{
			BaseDirEntryNode *node_p = AllocateBaseDirEntryNode (entry_p);
			
			if (node_p)
				{
					return node_p;
				}
				
			FreeBaseDirEntry (entry_p);
		}
	
	return NULL;
}


BaseDirEntryNode *AllocateBaseDirEntryNode (BaseDirEntry *entry_p)
{
	BaseDirEntryNode *node_p = (BaseDirEntryNode *) AllocMemory (sizeof (BaseDirEntryNode));
	
	if (node_p)
		{
			node_p -> bden_entry_p = entry_p;
			
			return node_p;
		}
					
	return NULL;
}


void FreeBaseDirEntryNode (ListItem *node_p)
{
	BaseDirEntryNode *dir_node_p = (BaseDirEntryNode *) node_p;

	FreeBaseDirEntry (dir_node_p -> bden_entry_p);
	FreeMemory (node_p);
}


bool AddDirEntry (BaseDirEntryNode *parent_node_p, BaseDirEntry *child_entry_p)
{
	bool success_flag = false;
	BaseDirEntryNode *child_node_p = AllocateBaseDirEntryNode (child_entry_p);

	if (child_node_p)
		{
			
		}

	
	
	return sucecss_flag;
}
