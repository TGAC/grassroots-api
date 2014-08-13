
#include "service.h"
#include "plugin.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"

void InitialiseService (Service * const service_p,
	const char *(*get_module_name_fn) (void),
	const char *(*get_module_description_fn) (void),
	LinkedList *(*get_parameters_fn) (void),
	ServiceData *data_p,
	uint32 priority)
{
	
}


void FreeService (Service *service_p)
{
	
}


ServiceNode *AllocateServiceNode (Service *service_p)
{
	ServiceNode *node_p = (ServiceNode *) AllocMemory (sizeof (ServiceNode));

	if (node_p)
		{
			node_p -> sn_service_p = service_p;

			return node_p;
		}

	return NULL;
}


void FreeServiceNode (ListNode * const node_p)
{
	ServiceNode *service_node_p = (ServiceNode *) node_p;

	FreeMemory (service_node_p);
}


LinkedList *LoadMatchingServices (const char * const path_s, const char * const filename_s)
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
									Plugin *plugin_p = AllocatePlugin (node_p -> sln_string_s);
									bool using_service_flag = false;
									
									if (plugin_p)
										{
											if (OpenPlugin (plugin_p))
												{																							
													Service *service_p = GetServiceFromPlugin (plugin_p);
													
													if (service_p)
														{
															bool using_service_flag = false;
															
															if (DoesFileMatchService (service_p, filename_s))
																{
																	ServiceNode *node_p = AllocateServiceNode (service_p);
																	
																	if (node_p)
																		{
																			LinkedListAddTail (services_list_p, (ListNode *) node_p);
																			using_service_flag = true;
																		}
																	else
																		{
																			/* failed to allocate service node */
																		}
																}
																
															if (!using_service_flag)
																{
																	FreeService (service_p);
																}
														}
													else
														{
															/* failed to get service from plugin */
														}
														
												}		/* if (OpenPlugin (plugin_p)) */

											if (!using_service_flag)
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
	
	return services_list_p;
}


int RunService (Service *service_p, const char *filename_s, ParameterSet *param_set_p)
{
	return service_p -> se_run_fn (filename_s, param_set_p);
}


bool DoesFileMatchService (Service *service_p, const char *filename_s)
{
	return service_p -> se_match_fn (filename_s);	
}


/** Get the user-friendly name of the service. */
const char *GetServiceName (Service *service_p)
{
	return service_p -> se_get_service_name_fn ();
}


/** Get the user-friendly description of the service. */
const char *GetServiceDescription (Service *service_p)
{
	return service_p -> se_get_service_description_fn ();	
}


ParameterSet *GetServiceParameters (Service *service_p)
{
	return service_p -> se_get_params_fn ();
}




