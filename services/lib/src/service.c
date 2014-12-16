
#include "service.h"
#include "plugin.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service_matcher.h"
#include "service_config.h"
#include "json_util.h"
#include "streams.h"

static bool AddServiceNameToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceDescriptionToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceParameterSetToJSON (Service * const service_p, json_t *root_p, const bool full_definition_flag, Resource *resource_p, const json_t *json_p);

static void GetMatchingServices (const char * const services_path_s, ServiceMatcher *matcher_p, const json_t *config_p, LinkedList *services_list_p, bool multiple_match_flag);

static const char *GetPluginNameFromJSON (const json_t * const root_p);



void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (Service *service_p),
	const char *(*get_service_description_fn) (Service *service_p),
	int (*run_fn) (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p),
	bool (*match_fn) (Service *service_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (Service *service_p, Resource *resource_p, const json_t *json_p),
	bool (*close_fn) (struct Service *service_p),
	bool specific_flag,
	ServiceData *data_p)
{
	service_p -> se_get_service_name_fn = get_service_name_fn;
	service_p -> se_get_service_description_fn = get_service_description_fn;
	service_p -> se_run_fn = run_fn;
	service_p -> se_match_fn = match_fn;
	service_p -> se_get_params_fn = get_parameters_fn;
	service_p -> se_close_fn = close_fn;
	service_p -> se_data_p = data_p;
	
	service_p -> se_is_specific_service_flag = specific_flag;
	
	if (service_p -> se_data_p)
		{
			service_p -> se_data_p -> sd_service_p = service_p;
		}
}


void FreeService (Service *service_p)
{
	FreeMemory (service_p);
}

bool CloseService (Service *service_p)
{
	return service_p -> se_close_fn (service_p);
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


json_t *GetServiceConfig (const char * const filename_s)
{
	json_t *config_p = NULL;
	FILE *config_f = fopen (filename_s, "r");
	
	if (config_f)
		{
			json_error_t error;
			
			config_p = json_loadf (config_f, 0, &error);
			
			if (!config_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to parse %s, error at line %d column %d\n", filename_s, error.line, error.column);
				}
				
			if (fclose (config_f) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to close service config file \"%s\"", filename_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to open service config file \"%s\"", filename_s);			
		}
	
	
	return config_p;
}


/**
 * Load any json stubs for external services that are used to configure generic services, 
 * e.g. web services
 */
void AddReferenceServices (LinkedList *services_p, const char * const references_path_s, const char * const services_path_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)
{
	const char *root_path_s = GetServerRootDirectory ();
	char *full_references_path_s = MakeFilename (root_path_s, references_path_s);
	
	if (full_references_path_s)
		{
			char *path_and_pattern_s = MakeFilename (full_references_path_s, "*.json");

			if (path_and_pattern_s)
				{
					LinkedList *matching_filenames_p = GetMatchingFiles (path_and_pattern_s, true);
					
					if (matching_filenames_p)
						{
							StringListNode *node_p = (StringListNode *) (matching_filenames_p -> ll_head_p);
							PluginNameServiceMatcher matcher;
	
							while (node_p)
								{
									json_t *config_p = GetServiceConfig (node_p -> sln_string_s);

									if (config_p)
										{
											char *json_s = json_dumps (config_p, JSON_INDENT (2));

											json_t *services_json_p = json_object_get (config_p, SERVICES_NAME_S);
											
											if (services_json_p)
												{
													const char * const service_name_s = GetPluginNameFromJSON (services_json_p);
													
													if (service_name_s)
														{
															InitPluginNameServiceMatcher (&matcher, MatchServiceByPluginName, service_name_s);			
															
															GetMatchingServices (services_path_s, (ServiceMatcher *) &matcher, services_json_p, services_p, false);
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, "Failed to get service name from", node_p -> sln_string_s);
														}
													
												}		/* if (services_json_p) */
																																
											if (json_s)
												{
													free (json_s);
												}
																							
										}		/* if (config_p) */
																				
									node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
								}		/* while (node_p) */
							
							FreeLinkedList (matching_filenames_p);
						}		/* if (matching_filenames_p) */
					
					FreeCopiedString (path_and_pattern_s);
				}		/* if (path_and_pattern_s) */
			
			FreeCopiedString (full_references_path_s);
		}		/* if (full_services_path_s) */			
}




bool GetService (const char * const plugin_name_s, Service **service_pp, ServiceMatcher *matcher_p, const json_t *config_p)
{
	Plugin *plugin_p = AllocatePlugin (plugin_name_s);
	bool success_flag = false;
	
	if (plugin_p)
		{
			if (OpenPlugin (plugin_p))
				{																							
					ServicesArray *services_p = GetServicesFromPlugin (plugin_p, config_p);
					
					if (services_p)
						{
							Service *service_p = NULL;
							uint32 i = services_p -> sa_num_services;
							Service **s_pp = services_p -> sa_services_pp;
							
							for ( ; i > 0; -- i, ++ s_pp)
								{
									bool using_service_flag = true;
							
									if (matcher_p)
										{
											using_service_flag = RunServiceMatcher (matcher_p, *s_pp);
										}
										
									if (using_service_flag)
										{
											*service_pp = *s_pp;
										}
									else
										{
											CloseService (*s_pp);
											*s_pp = NULL;
										}
								}
						}
					else
						{
							/* failed to get service from plugin */
							PrintErrors (STM_LEVEL_WARNING, "Failed to get service from plugin with name \"%s\"", plugin_name_s);	
						}
						
				}		/* if (OpenPlugin (plugin_p)) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to open plugin with name \"%s\"", plugin_name_s);
				}
				
		}		/* if (plugin_p) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to allocate plugin with name \"%s\"", plugin_name_s);
		}

		
	return success_flag;
}


void AddMatchingServicesFromServicesArray (ServicesArray *services_p, LinkedList *matching_services_p, ServiceMatcher *matcher_p, bool multiple_match_flag)
{
	Service **service_pp = services_p -> sa_services_pp;
	uint32 i = services_p -> sa_num_services;
	bool loop_flag = (i > 0);
	
	while (loop_flag)
		{
			if (*service_pp)
				{
					bool using_service_flag = RunServiceMatcher (matcher_p, *service_pp);
					
					if (using_service_flag)
						{
							ServiceNode *service_node_p = AllocateServiceNode (*service_pp);
							
							if (service_node_p)
								{
									LinkedListAddTail (matching_services_p, (ListItem *) service_node_p);
									using_service_flag = true;
									
									if (!multiple_match_flag)
										{
											loop_flag = false;	
										}
								}
							else
								{
									/* failed to allocate service node */
								}
						}
						
					if (!using_service_flag)
						{
							FreeService (*service_pp);
							*service_pp = NULL;
						}	
				}
				
			if (loop_flag)
				{
					-- i;
					++ service_pp;
					
					loop_flag = (i > 0);
				}
			
		}		/* while (loop_flag) */
	
}

void GetMatchingServices (const char * const services_path_s, ServiceMatcher *matcher_p, const json_t *config_p, LinkedList *services_list_p, bool multiple_match_flag)
{
	const char *plugin_pattern_s = GetPluginPattern ();
	
	if (plugin_pattern_s)
		{
			const char *root_path_s = GetServerRootDirectory ();
			char *full_services_path_s = MakeFilename (root_path_s, services_path_s);
			
			if (full_services_path_s)
				{
					char *path_and_pattern_s = MakeFilename (full_services_path_s, plugin_pattern_s);

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
															ServicesArray *services_p = GetServicesFromPlugin (plugin_p, config_p);
															
															if (services_p)
																{
																	AddMatchingServicesFromServicesArray (services_p, services_list_p, matcher_p, multiple_match_flag);

																	FreeServicesArray (services_p);
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
											
											if (node_p)
												{
													node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
												}
										}		/* while (node_p) */
									
									FreeLinkedList (matching_filenames_p);
								}		/* if (matching_filenames_p) */
							
							FreeCopiedString (path_and_pattern_s);
						}		/* if (path_and_pattern_s) */
					
					FreeCopiedString (full_services_path_s);
				}		/* if (full_services_path_s) */
			
		}		/* if (plugin_pattern_s) */

}


void LoadMatchingServicesByName (LinkedList *services_p, const char * const services_path_s, const char *service_name_s, const json_t *json_config_p)
{
	NameServiceMatcher matcher;
	
	InitNameServiceMatcher (&matcher, MatchServiceByName, service_name_s);
	
	return GetMatchingServices (services_path_s, & (matcher.nsm_base_matcher), json_config_p, services_p, true);
}



void LoadMatchingServices (LinkedList *services_p, const char * const services_path_s, Resource *resource_p, Handler *handler_p, const json_t *json_config_p)
{
	ResourceServiceMatcher matcher;
	
	InitResourceServiceMatcher (&matcher, MatchServiceByResource, resource_p, handler_p);
	
	GetMatchingServices (services_path_s, & (matcher.rsm_base_matcher), json_config_p, services_p, true);
}


int RunService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	return service_p -> se_run_fn (service_p, param_set_p, credentials_p);
}


bool IsServiceMatch (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return service_p -> se_match_fn (service_p, resource_p, handler_p);	
}


/** Get the user-friendly name of the service. */
const char *GetServiceName (Service *service_p)
{
	return service_p -> se_get_service_name_fn (service_p);
}


/** Get the user-friendly description of the service. */
const char *GetServiceDescription (Service *service_p)
{
	return service_p -> se_get_service_description_fn (service_p);	
}


ParameterSet *GetServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	return service_p -> se_get_params_fn (service_p, resource_p, json_p);
}


static const char *GetPluginNameFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, PLUGIN_NAME_S);
}


//
//	Get Symbol
//
ServicesArray *GetServicesFromPlugin (Plugin * const plugin_p, const json_t *service_config_p)
{
	if (!plugin_p -> pl_services_p)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "GetServices");

			if (symbol_p)
				{
					ServicesArray *(*fn_p) (const json_t *) = (ServicesArray *(*) (const json_t *)) symbol_p;

					plugin_p -> pl_services_p = fn_p (service_config_p);

					if (plugin_p -> pl_services_p)
						{
							AssignPluginForServicesArray (plugin_p -> pl_services_p, plugin_p);
							plugin_p -> pl_type = PN_SERVICE;
						}
				}
		}

	return plugin_p -> pl_services_p;
}


bool DeallocatePluginService (Plugin * const plugin_p)
{
	bool success_flag = (plugin_p -> pl_services_p == NULL);

	if (!success_flag)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "ReleaseServices");

			if (symbol_p)
				{
					void (*fn_p) (ServicesArray * const) = (void (*) (ServicesArray * const)) symbol_p;

					fn_p (plugin_p -> pl_services_p);

					plugin_p -> pl_services_p = NULL;
					success_flag = true;
				}
		}

	return success_flag;
}


json_t *GetServiceAsJSON (Service * const service_p, Resource *resource_p, const json_t *json_p)
{
	json_t *root_p = json_object ();
	
	if (root_p)
		{
			const char *value_s = GetServiceName (service_p);
			bool success_flag = true;
			
			/* Add the key-value pair */
			if (value_s)
				{
					success_flag = (json_object_set_new (root_p, SERVICES_NAME_S, json_string (value_s)) == 0);
				}

			if (success_flag)
				{
					#ifdef _DEBUG
					PrintJSON (stderr, root_p, "GetServiceAsJSON - path :: ");
					#endif
					
					value_s = GetServiceDescription (service_p);
					
					if (value_s)
						{
							success_flag = (json_object_set_new (root_p, SERVICES_DESCRIPTION_S, json_string (value_s)) == 0);
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
													if (AddServiceParameterSetToJSON (service_p, operation_p, true, resource_p, json_p))
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
	return GetJSONString (root_p, SERVICES_DESCRIPTION_S);
}


const char *GetServiceNameFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, SERVICES_NAME_S);
}


const char *GetOperationDescriptionFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, OPERATION_DESCRIPTION_S);
}


const char *GetOperationNameFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, OPERATION_ID_S);
}




static bool AddServiceNameToJSON (Service * const service_p, json_t *root_p)
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


static bool AddServiceDescriptionToJSON (Service * const service_p, json_t *root_p)
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


static bool AddServiceParameterSetToJSON (Service * const service_p, json_t *root_p, const bool full_definition_flag, Resource *resource_p, const json_t *json_p)
{
	bool success_flag = false;
	ParameterSet *param_set_p = GetServiceParameters (service_p, resource_p, json_p);
	
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


json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p)
{
	json_t *services_list_json_p = json_array ();
			
	if (services_list_json_p)
		{
			bool success_flag = true;
						
			if (services_list_json_p)
				{
					if (services_list_p)
						{
							ServiceNode *service_node_p = (ServiceNode *) (services_list_p -> ll_head_p);

							while (success_flag && service_node_p)
								{
									json_t *service_json_p = GetServiceAsJSON (service_node_p -> sn_service_p, resource_p, json_p);
									
									if (service_json_p)
										{
											success_flag = (json_array_append_new (services_list_json_p, service_json_p) == 0);
											
											service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
										}
									else
										{
											success_flag = false;
										}
								}

						}		/* if (services_list_p) */

					if (!success_flag)
						{
							json_array_clear (services_list_json_p);
							json_decref (services_list_json_p);
							services_list_json_p = NULL;
						}
															
				}		/* if (operations_p) */
				
		}		/* if (services_list_json_p) */

	return services_list_json_p;
}


ServicesArray *AllocateServicesArray (const uint32 num_services)
{
	Service **services_pp = (Service **) AllocMemoryArray (num_services, sizeof (Service *));
	
	if (services_pp)
		{
			ServicesArray *services_array_p = (ServicesArray *) AllocMemory (sizeof (ServicesArray));

			if (services_array_p)
				{
					services_array_p -> sa_services_pp = services_pp;
					services_array_p -> sa_num_services = num_services;

					return services_array_p;					
				}
			
			FreeMemory (services_pp);
		}
	
	return NULL;
}


void FreeServicesArray (ServicesArray *services_p)
{
	uint32 i = services_p -> sa_num_services;
	Service **service_pp = services_p -> sa_services_pp;
	
	for ( ; i > 0; -- i, ++ service_pp)
		{
			if (*service_pp)
				{
					FreeService (*service_pp);
				}
		}
	
	FreeMemory (services_p -> sa_services_pp);
	FreeMemory (services_p);	
}



void AssignPluginForServicesArray (ServicesArray *services_p, Plugin *plugin_p)
{
	Service **service_pp = services_p -> sa_services_pp;
	int i = services_p -> sa_num_services;

	for ( ; i > 0; -- i, ++ service_pp)
		{
			if (*service_pp)
				{
					(*service_pp) -> se_plugin_p = plugin_p;
				}
		}
}


