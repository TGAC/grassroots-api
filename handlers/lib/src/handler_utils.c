#include "handler_utils.h"
#include "linked_list.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"



Handler *GetResourceHandler (const Resource *resource_p, const TagItem *tags_p)
{
	
}


LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p);


LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p)
{
	LinkedList *handlers_list_p = AllocateLinkedList (FreeHandlerNode);
	
	if (handlers_list_p)
		{
			const char *plugin_pattern_s = GetPluginPattern ();
			
			if (plugin_pattern_s)
				{
					char *path_and_pattern_s = MakeFilename (handlers_path_s, plugin_pattern_s);

					if (path_and_pattern_s)
						{
							LinkedList *matching_filenames_p = GetMatchingFiles (path_and_pattern_s, true);
							
							if (matching_filenames_p)
								{
									StringListNode *node_p = (StringListNode *) (matching_filenames_p -> ll_head_p);
									
									while (node_p)
										{
											Plugin *plugin_p = AllocatePlugin (node_p -> sln_string_s);
											bool using_handler_flag = false;
											
											if (plugin_p)
												{
													if (OpenPlugin (plugin_p))
														{
															Handler *handler_p = GetHandlerFromPlugin (plugin_p);
															
															if (handler_p)
																{
																	
																	if (IsHandlerForResource (handler_p, resource_p))
																		{
																			HandlerNode *node_p = AllocateHandlerNode (handler_p);
																			
																			if (node_p)
																				{
																					LinkedListAddTail (handlers_list_p, (ListItem *) node_p);
																					using_handler_flag = true;
																				}
																			else
																				{
																					/* failed to allocate service node */
																				}
																		}
																		
																	if (!using_handler_flag)
																		{
																			FreeHandler (handler_p);
																		}
																}		/* if (handler_p) */
															else
																{
																	/* failed to get service from plugin */
																}
																
														}		/* if (OpenPlugin (plugin_p)) */

													if (!using_handler_flag)
														{
															ClosePlugin (plugin_p);
														}
														
												}		/* if (plugin_p) */
											
											
											node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
										}		/* while (node_p) */
									
									FreeLinkedList (matching_filenames_p);
								}		/* if (matching_filenames_p) */
							
							FreeCopiedString (path_and_pattern_s);
						}		/* if (path_and_pattern_s) */
					
				}		/* if (plugin_pattern_s) */
			
		}		/* if (services_list_p) */
	
	if (handlers_list_p -> ll_size == 0)
		{
			FreeLinkedList (handlers_list_p);
			handlers_list_p = NULL;
		}
	
	return handlers_list_p;
}

