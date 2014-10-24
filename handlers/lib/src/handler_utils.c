#include "handler_utils.h"
#include "linked_list.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"


#ifdef _DEBUG
	#define HANDLER_UTILS_DEBUG	(DL_FINE)
#else
	#define HANDLER_UTILS_DEBUG	(DL_NONE)
#endif


/**
 * This is a json array which contains elements where
 * the keys are the protocol names of the Handlers given 
 * by GetHandlerName () and the values are json objects. 
 * These values contain the key value pairs mapping any 
 * remote filename to a local copy where needed. 
 * 
 * For example if the dropbox handler had downloaded "/hello.txt"
 * to a temporary file at "/tmp/file90wsef" then this variable 
 * would be
 * 
 * 	[
 * 		{ "dropbox handler":
 * 			{
 * 				"/hello.txt": "/tmp/file90wsef",
 * 			}
 *		} 
 * 	]
 */
static json_t *s_mapped_filenames_p = NULL;


static LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p, const json_t *tags_p);


bool InitHandlerUtil (void)
{
	s_mapped_filenames_p = json_array ();
	
	return (s_mapped_filenames_p != NULL);
}


bool DestoyHandlerUtil (void)
{
	bool success_flag = true;
	
	if (s_mapped_filenames_p)
		{
			if (json_array_clear (s_mapped_filenames_p) == 0)
				{
					json_decref (s_mapped_filenames_p);
				}
			else
				{
					success_flag = false;
				}
		}
			
	return success_flag;
}





Handler *GetResourceHandler (const Resource *resource_p, const json_t *tags_p)
{
	Handler *handler_p = NULL;
	LinkedList *matching_handlers_p = NULL;
	
	#if HANDLER_UTILS_DEBUG >= DL_FINE
		{
			char *dump_s = json_dumps (tags_p, 0);
			
			free (dump_s);
		}
	#endif
	
	
	matching_handlers_p = LoadMatchingHandlers ("handlers", resource_p, tags_p);
	
	if (matching_handlers_p) 
		{
			if (matching_handlers_p -> ll_size == 1)
				{
					HandlerNode *node_p = (HandlerNode *) (matching_handlers_p -> ll_head_p);

					/* 
					 * Detach the handler from the node so that it stays in scope when
					 * we deallocate the list.
					 */
					handler_p = node_p -> hn_handler_p;
					node_p -> hn_handler_p = NULL;
					
					FreeLinkedList (matching_handlers_p);
					
				}
		}
	
	return handler_p;
}


static LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p, const json_t *tags_p)
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
									const char *tags_s = (tags_p != NULL) ? json_dumps (tags_p, 0) : NULL;

									while (node_p)
										{
											Plugin *plugin_p = AllocatePlugin (node_p -> sln_string_s);
											bool using_handler_flag = false;
											
											if (plugin_p)
												{
													if (OpenPlugin (plugin_p))
														{
															Handler *handler_p = GetHandlerFromPlugin (plugin_p, tags_p);
															
															if (handler_p)
																{
																	/* If resource_p is NULL or it matches, then add it */
																	if ((!resource_p) || IsHandlerForResource (handler_p, resource_p))
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



const char *GetMappedFilename (const char *protocol_s, const char *filename_s)
{
	
}


bool GetMappedFilename (const char *protocol_s, const char *filename_s, const char *mapped_filename_s)
{
	
}



