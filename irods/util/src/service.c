#include "service.h"


ServiceNode *AllocateServiceNode (void)
{

}


void FreeServiceNode (ListNode *node_p)
{

}


LinkedList *LoadServices (const char * const path_s)
{
	LinkedList *services_list_p = NULL;
	const char *plugin_pattern_s = GetPluginPattern ();
	
	if (plugin_pattern_s)
		{
			char *path_and_pattern_s = MakeFilename (path_s, plugin_pattern_s);

			if (path_and_pattern_s)
				{
					LinkedList *matching_filenames_p = GetMatchingFiles (path_and_pattern_s);
					
					if (matching_filenames_p)
						{
							StringListNode *node_p = (StringListNode *) (matching_filenames_p -> ll_head_p);
							
							while (node_p)
								{
									
									node_p = (StringListNode *) (node_p -> sn_node.ln_next_p);
								}		/* while (node_p) */
							
							
							FreeLinkedList (matching_filenames_p);
						}		/* if (matching_filenames_p) */
					
					FreeCopiedString (path_and_pattern_s);
				}		/* if (path_and_pattern_s) */
			
		}		/* if (plugin_pattern_s) */
	
	return services_list_p;
}

