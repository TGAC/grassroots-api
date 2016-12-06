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
#include "handler_utils.h"
#include "linked_list.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service_config.h"
#include "json_util.h"

#ifdef _DEBUG
	#define HANDLER_UTILS_DEBUG	(STM_LEVEL_FINE)
#else
	#define HANDLER_UTILS_DEBUG	(STM_LEVEL_NONE)
#endif


/**
 * This is a json array which contains elements where
 * the keys are the protocol names of the Handlers given
 * by GetHandlerName () and the values are json objects.
 * These values contain the key value pairs mapping any
 * remote filename to a local copy where needed.
 *
 * For example if the dropbox handler had downloaded "/hello.txt"
 * from user "bob" 
 * to a temporary file at "/tmp/file90wsef" then this variable
 * would be
 *
 * 	[
 * 		{ "dropbox handler":
 * 			{ "bob":
 * 				{ "/hello.txt": 
 * 					{ "filename": "/tmp/file90wsef",
 * 						"socket": 1,
 * 						"timestamp": 1231231231,
 * 						"write_count": 1,
 * 						"open_count": 1,
 * 					}
 *				} 
 * 			}
 *		}
 * 	]
 * 
 * filename: The mapped filename,
 * socket: The socket number, this is so that two different people can work on the same file.
 * timestamp: The last modified time of the remote file when it was downloaded.
 * write_count: The current number of processes writing to the file.
 * open_count: The current number of processes writing to the file.
 */
 
static const char * const S_FILENAME_KEY_S = "filename"; 
static const char * const S_SOCKET_KEY_S = "socket"; 
static const char * const S_TIME_KEY_S = "timestamp"; 
static const char * const S_WRITE_COUNT_KEY_S = "write_count"; 
static const char * const S_OPEN_COUNT_KEY_S = "open_count"; 
 
static json_t *s_mapped_filenames_p = NULL;


static LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p, const UserDetails *user_p);

static json_t *GetMappedObject (const char *protocol_s, const char *user_id_s, const char *filename_s, const bool create_flag);


/***************************************/


bool InitHandlerUtil (void)
{
	s_mapped_filenames_p = json_array ();

	return (s_mapped_filenames_p != NULL);
}


bool DestroyHandlerUtil (void)
{
	bool success_flag = true;

	if (s_mapped_filenames_p)
		{
			size_t handler_index;
			json_t *handler_p;

			json_array_foreach (s_mapped_filenames_p, handler_index, handler_p) 
				{
					const char *user_key_s;
					json_t *user_p;

					json_object_foreach (handler_p, user_key_s, user_p) 
						{
							const char *obj_key_s;
							json_t *obj_p;

							json_object_foreach (user_p, obj_key_s, obj_p) 
								{
									json_t *filename_p = json_object_get (obj_p, S_FILENAME_KEY_S);
									
									if (filename_p)
										{
											if (json_is_string (filename_p))
												{
													const char *filename_s = json_string_value (filename_p);
													
													if (!RemoveFile (filename_s))
														{
															
														}
												}
										}		/* if (filename_p) */
									
								}		/* json_object_foreach (user_p, obj_index, obj_p) */					
							
						}		/* json_object_foreach (handler_p, user_index, user_p) */					
						
				}		/* json_array_foreach (s_mapped_filenames_p, handler_index, handler_p) */

			
			
			if (json_array_clear (s_mapped_filenames_p) == 0)
				{
					json_decref (s_mapped_filenames_p);
				}
			else
				{
					success_flag = false;
				}
		}

	#if HANDLER_UTILS_DEBUG >= STM_LEVEL_FINE
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "exit DestroyHandlerUtil %d\n", success_flag);
	#endif

	return success_flag;
}



const char *GetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, time_t *time_p)
{
	const char *value_s = NULL;
	json_t *obj_p = GetMappedObject (protocol_s, user_id_s, filename_s, false);

	if (obj_p)
		{
			json_t *filename_p = json_object_get (obj_p, S_FILENAME_KEY_S);
			
			if (time_p)
				{
					
				}
			
			
			if (filename_p && json_is_string (filename_p))
				{
					value_s = json_string_value (filename_p);
				}		/* if (filename_p && json_is_string (filename_p)) */
		}		/* if (obj_p) */

	return value_s;
}


bool SetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, const char *mapped_filename_s, const time_t last_mod_time)
{
	bool success_flag = false;

	if (s_mapped_filenames_p)
		{	
			json_t *obj_p = GetMappedObject (protocol_s, user_id_s, filename_s, true);

			if (obj_p)
				{
					json_t *filename_value_p = json_string (mapped_filename_s);
					
					if (filename_value_p)
						{
							if (json_object_set_new (obj_p, S_FILENAME_KEY_S, filename_value_p) == 0)
								{
									json_t *timestamp_p = json_integer (last_mod_time);
									
									if (timestamp_p)
										{
											if (json_object_set_new (obj_p, S_TIME_KEY_S, timestamp_p) == 0)
												{
													success_flag = true;
												}
										}
									else
										{
											json_decref (timestamp_p);
											timestamp_p = NULL;
										}
								}
							else
								{
									json_decref (filename_value_p);
									filename_value_p = NULL;
								}
								
						}		/* if (filename_value_p) */
						
				}		/* if (obj_p) */

		}		/* if (s_mapped_filenames_p) */

	return success_flag;
}



Handler *GetResourceHandler (const Resource *resource_p, const UserDetails *user_p)
{
	Handler *handler_p = NULL;
	LinkedList *matching_handlers_p = NULL;
	const char *root_path_s = GetServerRootDirectory ();
	char *handlers_path_s = MakeFilename (root_path_s, "handlers");

	if (handlers_path_s)
		{
			matching_handlers_p = LoadMatchingHandlers (handlers_path_s, resource_p, user_p);

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

				}		/* if (matching_handlers_p) */

			FreeCopiedString (handlers_path_s);
		}		/* if (handlers_path_s) */

	return handler_p;
}


static LinkedList *LoadMatchingHandlers (const char * const handlers_path_s, const Resource * const resource_p, const UserDetails *user_p)
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
															Handler *handler_p = GetHandlerFromPlugin (plugin_p, user_p);

															if (handler_p)
																{
																	/* If resource_p is NULL or it matches, then add it */
																	if ((!resource_p) || IsHandlerForResource (handler_p, resource_p))
																		{
																			HandlerNode *handler_node_p = AllocateHandlerNode (handler_p);

																			if (handler_node_p)
																				{
																					LinkedListAddTail (handlers_list_p, (ListItem *) handler_node_p);
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


static json_t *GetMappedObject (const char *protocol_s, const char *user_id_s, const char *filename_s, const bool create_flag)
{
	json_t *value_p = NULL;

	if (s_mapped_filenames_p)
		{
			#if HANDLER_UTILS_DEBUG >= STM_LEVEL_FINE
			char *mappings_s = json_dumps (s_mapped_filenames_p, JSON_INDENT (2));
			#endif
			
			json_t *protocol_obj_p = json_object_get (s_mapped_filenames_p, protocol_s);

			if ((!protocol_obj_p) && create_flag)
				{
					protocol_obj_p = json_object ();
					
					if (protocol_obj_p)
						{
							if (json_object_set_new (s_mapped_filenames_p, protocol_s, protocol_obj_p) != 0)
								{
									json_decref (protocol_obj_p);
									protocol_obj_p = NULL;
								}		/* if (json_object_set_new (s_mapped_filenames_p, protocol_s, protocol_obj_p) != 0) */

						}		/* if (protocol_obj_p) */

				}		/* if ((!protocol_obj_p) && create_flag) */

			if (protocol_obj_p)
				{
					json_t *user_p = json_object_get (protocol_obj_p, user_id_s);
					
					if ((!user_p) && create_flag)
						{
							user_p = json_object ();
					
							if (user_p)
								{
									if (json_object_set_new (protocol_obj_p, user_id_s, user_p) != 0)
										{
											json_decref (user_p);
											user_p = NULL;
										}		/* if (json_object_set_new (protocol_obj_p, user_id_s, user_p) != 0) */
								}
								
						}		/* if ((!user_p) && create_flag) */

					if (user_p)
						{
							value_p = json_object_get (user_p, filename_s);
								
							if ((!value_p) && create_flag)
								{
									value_p = json_object ();
							
									if (value_p)
										{
											if (json_object_set_new (user_p, filename_s, value_p) != 0)
												{
													json_decref (value_p);
													value_p = NULL;
												}		/* if (json_object_set_new (protocol_obj_p, filename_s, value_p) != 0) */
												
										}		/* if (value_p) */							
										
								}		/* if ((!value_p) && create_flag) */										
												
						}		/* if (user_p) */		
						
				}		/* if (protocol_obj_p) */

			#if HANDLER_UTILS_DEBUG >= STM_LEVEL_FINE
			if (mappings_s)
				{
					free (mappings_s);
				}
			#endif

		}		/* if (s_mapped_filenames_p) */

	return value_p;
}



