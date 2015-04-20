
/* Allocate the global constants in service.h */
#define ALLOCATE_PATH_TAGS (1)

#include <string.h>

#include "service.h"
#include "plugin.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service_matcher.h"
#include "service_config.h"
#include "json_util.h"
#include "streams.h"


#ifdef _DEBUG
	#define SERVICE_DEBUG	(STM_LEVEL_FINE)
#else
	#define SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


static bool AddServiceNameToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceDescriptionToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceParameterSetToJSON (Service * const service_p, json_t *root_p, const bool full_definition_flag, Resource *resource_p, const json_t *json_p);

static bool AddOperationInformationURIToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceUUIDToJSON (Service * const service_p, json_t *root_p);

static void GetMatchingServices (const char * const services_path_s, ServiceMatcher *matcher_p, const json_t *config_p, LinkedList *services_list_p, bool multiple_match_flag);

static const char *GetPluginNameFromJSON (const json_t * const root_p);

static uint32 AddMatchingServicesFromServicesArray (ServicesArray *services_p, LinkedList *matching_services_p, ServiceMatcher *matcher_p, bool multiple_match_flag);


void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (Service *service_p),
	const char *(*get_service_description_fn) (Service *service_p),
	const char *(*get_service_info_uri_fn) (struct Service *service_p),
	json_t *(*run_fn) (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p),
	bool (*match_fn) (Service *service_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (Service *service_p, Resource *resource_p, const json_t *json_p),
	void (*release_parameters_fn) (Service *service_p, ParameterSet *params_p),
	bool (*close_fn) (struct Service *service_p),
	bool specific_flag,
	ServiceData *data_p)
{
	service_p -> se_get_service_name_fn = get_service_name_fn;
	service_p -> se_get_service_description_fn = get_service_description_fn;
	service_p -> se_get_service_info_uri_fn = get_service_info_uri_fn;
	service_p -> se_run_fn = run_fn;
	service_p -> se_match_fn = match_fn;
	service_p -> se_get_params_fn = get_parameters_fn;
	service_p -> se_release_params_fn = release_parameters_fn;
	service_p -> se_close_fn = close_fn;
	service_p -> se_data_p = data_p;
	
	service_p -> se_is_specific_service_flag = specific_flag;
	
	if (service_p -> se_data_p)
		{
			service_p -> se_data_p -> sd_service_p = service_p;
		}
		
	service_p -> se_plugin_p = NULL;
	service_p -> se_has_permissions_fn = NULL;
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
void AddReferenceServices (LinkedList *services_p, const char * const references_path_s, const char * const services_path_s, const char *operation_name_s, const json_t *config_p)
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
							StringListNode *reference_file_node_p = (StringListNode *) (matching_filenames_p -> ll_head_p);
							PluginNameServiceMatcher *matcher_p = NULL;				
	
							if (operation_name_s)
								{
									matcher_p = (PluginNameServiceMatcher *) AllocatePluginOperationNameServiceMatcher (NULL, operation_name_s);
								}
							else
								{
									matcher_p = (PluginNameServiceMatcher *) AllocatePluginNameServiceMatcher (NULL);											
								}
								
							if (matcher_p)
								{
									while (reference_file_node_p)
										{
											json_t *config_p = GetServiceConfig (reference_file_node_p -> sln_string_s);

											if (config_p)
												{
													char *json_s = json_dumps (config_p, JSON_INDENT (2));

													json_t *services_json_p = json_object_get (config_p, SERVICES_NAME_S);
													
													if (services_json_p)
														{
															const char * const plugin_name_s = GetPluginNameFromJSON (services_json_p);
															
															if (plugin_name_s)
																{
																	SetPluginNameForServiceMatcher (matcher_p, plugin_name_s);		
																	
																	GetMatchingServices (services_path_s, (ServiceMatcher *) matcher_p, services_json_p, services_p, true);
																}
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, "Failed to get service name from", reference_file_node_p -> sln_string_s);
																}
															
														}		/* if (services_json_p) */
																																		
													if (json_s)
														{
															free (json_s);
														}
																									
												}		/* if (config_p) */
																						
											reference_file_node_p = (StringListNode *) (reference_file_node_p -> sln_node.ln_next_p);
										}		/* while (reference_file_node_p) */
									
									FreeServiceMatcher ((ServiceMatcher *) matcher_p);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to allocate memory for references service matcher\n");
								}
							
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


static uint32 AddMatchingServicesFromServicesArray (ServicesArray *services_p, LinkedList *matching_services_p, ServiceMatcher *matcher_p, bool multiple_match_flag)
{
	Service **service_pp = services_p -> sa_services_pp;
	uint32 i = services_p -> sa_num_services;
	bool loop_flag = (i > 0);
	uint32 num_matched_services = 0;
	
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
									
									++ num_matched_services;
									
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
						
					if (using_service_flag)
						{
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
	
	return num_matched_services;
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
										
											if (plugin_p)
												{
													bool using_plugin_flag = false;
													
													if (OpenPlugin (plugin_p))
														{																							
															ServicesArray *services_p = GetServicesFromPlugin (plugin_p, config_p);
															
															if (services_p)
																{
																	using_plugin_flag = (AddMatchingServicesFromServicesArray (services_p, services_list_p, matcher_p, multiple_match_flag) > 0);

																	FreeServicesArray (services_p);
																}
															else
																{
																	/* failed to get service from plugin */
																}
																
														}		/* if (OpenPlugin (plugin_p)) */

													if (!using_plugin_flag)
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
	
	InitOperationNameServiceMatcher (&matcher, service_name_s);
	
	GetMatchingServices (services_path_s, & (matcher.nsm_base_matcher), json_config_p, services_p, true);
	
	/* @TODO Add an AddReferenceServicesByName function to be called here */
	AddReferenceServices (services_p, REFERENCES_PATH_S, services_path_s, service_name_s, json_config_p);
}



void LoadMatchingServices (LinkedList *services_p, const char * const services_path_s, Resource *resource_p, Handler *handler_p, const json_t *json_config_p)
{
	ResourceServiceMatcher matcher;
	
	InitResourceServiceMatcher (&matcher, resource_p, handler_p);
	
	GetMatchingServices (services_path_s, & (matcher.rsm_base_matcher), json_config_p, services_p, true);
		
	AddReferenceServices (services_p, REFERENCES_PATH_S, services_path_s, NULL, json_config_p);
}


void LoadKeywordServices (LinkedList *services_p, const char * const services_path_s, const json_t *json_config_p)
{
	KeywordServiceMatcher matcher;

	InitKeywordServiceMatcher (&matcher);

	GetMatchingServices (services_path_s, & (matcher.ksm_base_matcher), json_config_p, services_p, true);

	/* @TODO Add an AddReferenceServicesByName function to be called here */
	AddReferenceServices (services_p, REFERENCES_PATH_S, services_path_s, NULL, json_config_p);
}


json_t *RunService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
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



const char *GetServiceInformationURI (Service *service_p)
{
	const char *uri_s = NULL;

	if (service_p -> se_get_service_info_uri_fn)
		{
			uri_s = service_p -> se_get_service_info_uri_fn (service_p);
		}

	return uri_s;
}


const uuid_t *GetServiceUUID (const Service *service_p)
{
	return & (service_p -> se_id);
}


char *GetServiceUUIDAsString (Service *service_p)
{
	char *uuid_s = NULL;
	const uuid_t *id_p = GetServiceUUID (service_p);

	if (uuid_is_null (*id_p) == 0)
		{
			uuid_s = GetUUIDAsString (*id_p);
		}

	return uuid_s;
}



ParameterSet *GetServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	return service_p -> se_get_params_fn (service_p, resource_p, json_p);
}


void ReleaseServiceParameters (Service *service_p, ParameterSet *params_p)
{
	return service_p -> se_release_params_fn (service_p, params_p);
}


OperationStatus GetCurrentServiceStatus (const Service *service_p)
{
	return service_p -> se_status;
}


void SetCurrentServiceStatus (Service *service_p, const OperationStatus status)
{
	service_p -> se_status = status;
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
					#if SERVICE_DEBUG >= STM_LEVEL_FINER
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

							#if SERVICE_DEBUG >= STM_LEVEL_FINER
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
															if (AddServiceUUIDToJSON (service_p, operation_p))
																{
																	AddOperationInformationURIToJSON (service_p, operation_p);

																	success_flag = true;
																}		/* if (AddServiceUUIDToJSON (service_p, operation_p)) */

														}		/* if (AddServiceParameterSetToJSON (service_p, operation_p, true, resource_p, json_p)) */

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

			#if SERVICE_DEBUG >= STM_LEVEL_FINE
			PrintJSON (stderr, root_p, "GetServiceAsJSON - service :: ");
			#endif

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


const char *GetOperationInformationURIFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, OPERATION_INFORMATION_URI_S);
}


const char *GetIconPathFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, OPERATION_ICON_PATH_S);
}


static bool AddServiceNameToJSON (Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *name_s = GetServiceName (service_p);

	if (name_s)
		{
			success_flag = (json_object_set_new (root_p, "nickname", json_string (name_s)) == 0);
		}

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
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

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddServiceDescriptionToJSON :: description -> ");
	#endif

	return success_flag;
}


static bool AddServiceUUIDToJSON (Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	char *uuid_s = GetServiceUUIDAsString (service_p);

	if (uuid_s)
		{
			success_flag = (json_object_set_new (root_p, SERVICE_UUID_S, json_string (uuid_s)) == 0);
			FreeUUIDString (uuid_s);
		}

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddServiceUUIDToJSON :: uuid -> ");
	#endif

	return success_flag;
}



static bool AddOperationInformationURIToJSON (Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *uri_s = GetServiceInformationURI (service_p);

	if (uri_s)
		{
			success_flag = (json_object_set_new (root_p, OPERATION_INFORMATION_URI_S, json_string (uri_s)) == 0);
		}


	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddOperationInformationURIToJSON :: uri_s -> ");
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
					success_flag = (json_object_set (root_p, PARAM_SET_KEY_S, param_set_json_p) == 0);
					json_decref (param_set_json_p);
				}
				
			/* could set this to be cached ... */
			ReleaseServiceParameters (service_p, param_set_p);
		}

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddServiceParameterSetToJSON :: parameters -> ");
	#endif
	
	return success_flag;
}


json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p)
{
	json_t *services_list_json_p = json_array ();
			
	if (services_list_json_p)
		{
			if (services_list_json_p)
				{
					if (services_list_p)
						{
							ServiceNode *service_node_p = (ServiceNode *) (services_list_p -> ll_head_p);

							while (service_node_p)
								{
									json_t *service_json_p = GetServiceAsJSON (service_node_p -> sn_service_p, resource_p, json_p);

									#if SERVICE_DEBUG >= STM_LEVEL_FINE
										{
											char *response_s = json_dumps (service_json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
											printf ("service:\n%s\n\n", response_s);
											free (response_s);
										}
									#endif
									
									if (service_json_p)
										{
											if (json_array_append_new (services_list_json_p, service_json_p) != 0)
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to add service json description for %s to list\n", GetServiceName (service_node_p -> sn_service_p));
												}
											
											service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to get service json description for %s\n", GetServiceName (service_node_p -> sn_service_p));
										}
								}

						}		/* if (services_list_p) */
															
				}		/* if (operations_p) */

		#if SERVICE_DEBUG >= STM_LEVEL_FINE
			{
				char *response_s = json_dumps (services_list_json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
				printf ("services list:\n%s\n\n", response_s);
				free (response_s);
			}
		#endif

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




/*
 "response": {
      "type": "object",
      "description": "The server's response from having run a service",
      "properties": {
        "service": { 
          "type": "string",
          "description": "The name of the service"
        },
        "status": { 
          "type": "string",
          "description": "The status of the job on the service",
          "enum": [
            "waiting",
            "failed to start",
            "started",
            "finished",
            "failed"
          ]    
        },
        "result": { 
          "type": "string",
          "description": "A value to be shown to the user"
        },
      }
    }, 
*/
json_t *CreateServiceResponseAsJSON (Service *service_p, OperationStatus status, json_t *result_json_p)
{
	json_error_t error;
	const char *service_name_s = GetServiceName (service_p);
	const char *service_description_s = GetServiceDescription (service_p);
	json_t *json_p = json_pack_ex (&error, 0, "{s:s,s:s,s:i}", SERVICE_NAME_S, service_name_s, SERVICES_DESCRIPTION_S, service_description_s, SERVICE_STATUS_S, status);
	const char *info_uri_s = GetServiceInformationURI (service_p);


	if (info_uri_s)
		{
			if (json_object_set_new (json_p, OPERATION_INFORMATION_URI_S, json_string (info_uri_s)) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to add operation info uri \"%s\" to service response for \"%s\"", info_uri_s, service_name_s);
				}
		}


	#if SERVICE_DEBUG >= STM_LEVEL_FINE
	char *dump_s = json_dumps (result_json_p, JSON_INDENT (2));
	PrintLog (STM_LEVEL_FINE, "result json:\n%s", dump_s);
	free (dump_s);

	dump_s = json_dumps (json_p, JSON_INDENT (2));
	PrintLog (STM_LEVEL_FINE, "resp json:\n%s", dump_s);
	free (dump_s);
	#endif


	if (json_p)
		{
			if (result_json_p)
				{
					if (!json_object_set_new (json_p, SERVICE_RESULTS_S, result_json_p) == 0)
						{
							char *dump_s = json_dumps (result_json_p, JSON_INDENT (2));
							PrintErrors (STM_LEVEL_WARNING, "Failed to add %s to service response for %s\n", dump_s, service_name_s);

							free (dump_s);
							json_decref (json_p);
							json_p = NULL;
						}
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to create json service response for %s\n", service_name_s);			
		}
	
	#if SERVICE_DEBUG >= STM_LEVEL_FINE
	dump_s = json_dumps (json_p, JSON_INDENT (2));
	PrintLog (STM_LEVEL_FINE, "final resp json:\n%s", dump_s);
	free (dump_s);
	#endif

	
	return json_p;
}


ServicesArray *GetReferenceServicesFromJSON (json_t *config_p, const char *plugin_name_s, Service *(*get_service_fn) (json_t *config_p, size_t i))
{
	if (config_p)
		{
			/* Make sure that the config refers to our service */
			json_t *value_p = json_object_get (config_p, PLUGIN_NAME_S);

			if (value_p)
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);

							if (strcmp (value_s, plugin_name_s) == 0)
								{
									json_t *ops_p = json_object_get (config_p, SERVER_OPERATIONS_S);

									if (ops_p)
										{
											size_t num_ops = json_is_array (ops_p) ? json_array_size (ops_p) : 1;
											ServicesArray *services_p = AllocateServicesArray (num_ops);

											if (services_p)
												{
													size_t i = 0;
													Service **service_pp = services_p -> sa_services_pp;

													while (i < num_ops)
														{
															json_t *op_p =  json_array_get (ops_p, i);
															Service *service_p = get_service_fn (op_p, i);

															if (service_p)
																{
																	*service_pp = service_p;
																}
															else
																{
																	char *dump_s = json_dumps (op_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																	if (dump_s)
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to create service %lu from:\n%s\n", i, dump_s);
																			free (dump_s);
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to create service %lu\n", i);
																		}

																}

															++ i;
															++ service_pp;
														}

														return services_p;
												}

										}

								}		/* if (strcmp (value_s, plugin_name_s) == 0) */

						}		/* if (json_is_string (value_p)) */

				}		/* if (value_p) */

		}		/* if (config_p) */

	return NULL;
}

