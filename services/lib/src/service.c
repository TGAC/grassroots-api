
#include "service.h"
#include "plugin.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"

#include "json_util.h"


static bool AddServiceNameToJSON (const Service * const service_p, json_t *root_p);

static bool AddServiceDescriptionToJSON (const Service * const service_p, json_t *root_p);

static bool AddServiceParameterSetToJSON (const Service * const service_p, json_t *root_p, const bool full_definition_flag, TagItem *tags_p);




void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (void),
	const char *(*get_service_description_fn) (void),
	int (*run_fn) (ServiceData *service_data_p, ParameterSet *param_set_p),
	bool (*match_fn) (ServiceData *service_data_p, TagItem *tags_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (ServiceData *service_data_p, TagItem *tags_p),
	ServiceData *data_p)
{
	service_p -> se_get_service_name_fn = get_service_name_fn;
	service_p -> se_get_service_description_fn = get_service_description_fn;
	service_p -> se_run_fn = run_fn;
	service_p -> se_match_fn = match_fn;
	service_p -> se_get_params_fn = get_parameters_fn;
	service_p -> se_data_p = data_p;
	
	if (service_p -> se_data_p)
		{
			service_p -> se_data_p -> sd_service_p = service_p;
		}
}


void FreeService (Service *service_p)
{
	FreeMemory (service_p);
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


void FreeServiceNode (ListItem * const node_p)
{
	ServiceNode *service_node_p = (ServiceNode *) node_p;

	FreeMemory (service_node_p);
}




LinkedList *LoadMatchingServices (const char * const services_path_s, TagItem *tags_p, Handler *handler_p)
{
	LinkedList *services_list_p = AllocateLinkedList (FreeServiceNode);
	
	if (services_list_p)
		{
			const char *plugin_pattern_s = GetPluginPattern ();
			
			if (plugin_pattern_s)
				{
					char *path_and_pattern_s = MakeFilename (services_path_s, plugin_pattern_s);

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
															Service *service_p = GetServiceFromPlugin (plugin_p);
															
															if (service_p)
																{
																	using_service_flag = true;
																	
																	if (tags_p && (tags_p -> ti_tag != TAG_DONE))
																		{
																			using_service_flag = IsServiceMatch (service_p, tags_p, handler_p);
																		}
																	
																	if (using_service_flag)
																		{
																			ServiceNode *service_node_p = AllocateServiceNode (service_p);
																			
																			if (service_node_p)
																				{
																					LinkedListAddTail (services_list_p, (ListItem *) service_node_p);
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
			
		}		/* if (services_list_p) */
	
	if (services_list_p -> ll_size == 0)
		{
			FreeLinkedList (services_list_p);
			services_list_p = NULL;
		}
	
	return services_list_p;
}


int RunService (Service *service_p, ParameterSet *param_set_p)
{
	return service_p -> se_run_fn (service_p -> se_data_p, param_set_p);
}


bool IsServiceMatch (Service *service_p, TagItem *tags_p, Handler *handler_p)
{
	return service_p -> se_match_fn (service_p -> se_data_p, tags_p, handler_p);	
}


/** Get the user-friendly name of the service. */
const char *GetServiceName (const Service *service_p)
{
	return service_p -> se_get_service_name_fn ();
}


/** Get the user-friendly description of the service. */
const char *GetServiceDescription (const Service *service_p)
{
	return service_p -> se_get_service_description_fn ();	
}


ParameterSet *GetServiceParameters (const Service *service_p, TagItem *tags_p)
{
	return service_p -> se_get_params_fn (service_p -> se_data_p, tags_p);
}


json_t *GetServiceAsJSON (const Service * const service_p, TagItem *tags_p)
{
	json_t *root_p = json_object ();
	
	if (root_p)
		{
			const char *value_s = GetServiceName (service_p);
			bool success_flag = true;
			
			/* Add the key-value pair */
			if (value_s)
				{
					success_flag = (json_object_set_new (root_p, SERVICE_NAME_S, json_string (value_s)) == 0);
				}

			if (success_flag)
				{
					#ifdef _DEBUG
					PrintJSON (stderr, root_p, "GetServiceAsJSON - path :: ");
					#endif
					
					value_s = GetServiceDescription (service_p);
					
					if (value_s)
						{
							success_flag = (json_object_set_new (root_p, SERVICE_DESCRIPTION_S, json_string (value_s)) == 0);
						}
					
					if (success_flag)
						{
							/* Add the operations for this service */
							json_t *operation_p = json_object ();

							#ifdef _DEBUG
							PrintJSON (stderr, root_p, "GetServiceAsJSON - description :: ");
							#endif

							if (operation_p)
								{
									success_flag = false;

									if (AddServiceNameToJSON (service_p, operation_p))
										{
											if (AddServiceDescriptionToJSON (service_p, operation_p))
												{
													if (AddServiceParameterSetToJSON (service_p, operation_p, true, tags_p))
														{
															success_flag = true;
														}
												}		/* if (AddServiceDescriptionToJSON (service_p, operation_p)) */	
																						
										}		/* if (AddServiceNameToJSON (service_p, operation_p)) */
										
									if (success_flag)
										{
											success_flag = (json_object_set (root_p, SERVER_OPERATIONS_S, operation_p) == 0);
											json_decref (operation_p);
										}
									else
										{
											json_object_clear (root_p);
											json_decref (root_p);
											root_p = NULL;
										}
										
								}		/* if (operation_p) */
						
						}
				}
		}		/* if (root_p) */
		
		
	return root_p;
}


const char *GetServiceDescriptionFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, SERVICE_DESCRIPTION_S);
}


const char *GetServiceNameFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, SERVICE_NAME_S);
}



static bool AddServiceNameToJSON (const Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *name_s = GetServiceName (service_p);

	if (name_s)
		{
			success_flag = (json_object_set_new (root_p, "nickname", json_string (name_s)) == 0);
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddServiceNameToJSON :: nickname -> ");
	#endif

	return success_flag;
}


static bool AddServiceDescriptionToJSON (const Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *description_s = GetServiceDescription (service_p);

	if (description_s)
		{
			success_flag = (json_object_set_new (root_p, "summary", json_string (description_s)) == 0);
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddServiceDescriptionToJSON :: description -> ");
	#endif

	return success_flag;
}


static bool AddServiceParameterSetToJSON (const Service * const service_p, json_t *root_p, const bool full_definition_flag, TagItem *tags_p)
{
	bool success_flag = false;
	ParameterSet *param_set_p = GetServiceParameters (service_p, tags_p);
	
	if (param_set_p)
		{
			json_t *param_set_json_p = GetParameterSetAsJSON (param_set_p, full_definition_flag);
			
			if (param_set_json_p)
				{
					success_flag = (json_object_set (root_p, PARAM_SET_PARAMS_S, param_set_json_p) == 0);
					json_decref (param_set_json_p);
				}
				
			/* could set this to be cached ... */
			FreeParameterSet (param_set_p);
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddServiceParameterSetToJSON :: parameters -> ");
	#endif
	
	return success_flag;
}
