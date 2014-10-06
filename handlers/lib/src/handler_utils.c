#include "handler_utils.h"
#include "linked_list.h"

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
											bool using_service_flag = false;
											
											if (plugin_p)
												{
													if (OpenPlugin (plugin_p))
														{																							
															Handler *handler_p = GetHandlerFromPlugin (plugin_p);
															
															if (handler_p)
																{
																	using_handler_flag = true;
																	
																	if (tags_p && (tags_p -> ti_tag != TAG_DONE))
																		{
																			using_handler_flag = IsServiceMatch (service_p, tags_p, handler_p);
																		}
																	
																	if (using_handler_flag)
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
	
	if (services_list_p -> ll_size == 0)
		{
			FreeLinkedList (services_list_p);
			services_list_p = NULL;
		}
	
	return services_list_p;
}



Handler *GetResourceHandler (const Resource *resource_p, const TagItem *tags_p)
{
	Handler *handler_p = NULL;
	/*
	if (resource_p)
		{
			switch (resource_p -> re_protocol)
				{
					case FILE_LOCATION_LOCAL:
						handle_p = AllocateFileHandle ();
						break;
						
					case FILE_LOCATION_REMOTE:
						handle_p = AllocateFileHandle ();
						break;
						
					case FILE_LOCATION_IRODS:
						{
							IRodsReource *irods_resource_p = (IRodsReource *) resource_p;
							TagItem *ti_p = FindMatchingTag (tags_p, TAG_IRODS_CONNECTION);

							if (ti_p)
								{
									rcComm_t *connection_p = (rcComm_t *) (ti_p -> ti_value.st_generic_value_p);
				
									handle_p = AllocateIRodsHandle (connection_p);
								}
							else
								{
									TagItem *ti_p = FindMatchingTag (tags_p, TAG_IRODS_USER);
									
									if (ti_p)
										{
											const char *username_s = ti_p -> ti_value.st_string_value_s;
											
											ti_p = FindMatchingTag (tags_p, TAG_IRODS_PASSWORD);
											
											if (ti_p)
												{
													const char *password_s = ti_p -> ti_value.st_string_value_s;
													
													handle_p = GetIRodsHandle (username_s, password_s);
												}
										}
								}							
						}
						break;
						
					case FILE_LOCATION_UNKNOWN:
					default:
						break;
				}
		}
	*/
	
	return handler_p;
}
