#include "directory_info.h"




BaseDirEntry *AllocateBaseDirEntry (char *filename_s, DirEntryType entry_type)
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


DirEntry *AllocateDirEntry (char *filename_s)
{

}


BaseDirEntryNode *AllocateBaseDirEntryNode (char *filename_s, DirEntryType entry_type)
{

}


void FreeBaseDirEntry (ListItems *node_p)
{

}


bool AddDirEntry (DirEntry *parent_entry_p, BaseDirEntry *child_entry_p)
{

}
