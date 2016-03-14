/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include "json_tools.h"
#include "json_util.h"
#include "streams.h"
#include "service_job.h"
#include "grassroots_config.h"
#include "paired_service.h"
#include "servers_pool.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define SERVICE_DEBUG	(STM_LEVEL_INFO)
#else
	#define SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


static bool AddServiceNameToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceDescriptionToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceParameterSetToJSON (Service * const service_p, json_t *root_p, const bool full_definition_flag, Resource *resource_p, const json_t *json_p);

static bool AddOperationInformationURIToJSON (Service * const service_p, json_t *root_p);

static bool AddServiceUUIDToJSON (Service * const service_p, json_t *root_p);

static uint32 GetMatchingServices (const char * const services_path_s, ServiceMatcher *matcher_p, const json_t *config_p, LinkedList *services_list_p, bool multiple_match_flag);

static const char *GetPluginNameFromJSON (const json_t * const root_p);

static uint32 AddMatchingServicesFromServicesArray (ServicesArray *services_p, LinkedList *matching_services_p, ServiceMatcher *matcher_p, bool multiple_match_flag);

static void GenerateServiceUUID (Service *service_p);


void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (Service *service_p),
	const char *(*get_service_description_fn) (Service *service_p),
	const char *(*get_service_info_uri_fn) (struct Service *service_p),
	ServiceJobSet *(*run_fn) (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p),
	ParameterSet *(*match_fn) (Service *service_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (Service *service_p, Resource *resource_p, const json_t *json_p),
	void (*release_parameters_fn) (Service *service_p, ParameterSet *params_p),
	bool (*close_fn) (struct Service *service_p),
	json_t *(*get_results_fn) (struct Service *service_p, const uuid_t service_id),
	OperationStatus (*get_status_fn) (Service *service_p, const uuid_t service_id),
	bool specific_flag,
	bool synchronous_flag,
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
	service_p -> se_get_status_fn = get_status_fn;
	service_p -> se_get_results_fn = get_results_fn;
	service_p -> se_data_p = data_p;
	
	service_p -> se_is_specific_service_flag = specific_flag;
	service_p -> se_synchronous_flag = synchronous_flag;

	if (service_p -> se_data_p)
		{
			const char *service_name_s = service_p -> se_get_service_name_fn (service_p);

			service_p -> se_data_p -> sd_service_p = service_p;

			if (service_name_s)
				{
					service_p -> se_data_p -> sd_config_p = GetGlobalServiceConfig (service_name_s);
				}
		}

	uuid_clear (& (service_p -> se_id));

	service_p -> se_plugin_p = NULL;
	service_p -> se_has_permissions_fn = NULL;

	service_p -> se_jobs_p = NULL;

	InitLinkedList (& (service_p -> se_paired_services));
	SetLinkedListFreeNodeFunction (& (service_p -> se_paired_services), FreePairedServiceNode);
}


void FreeService (Service *service_p)
{
	Plugin *plugin_p = service_p -> se_plugin_p;

	if (service_p -> se_jobs_p)
		{
			FreeServiceJobSet (service_p -> se_jobs_p);
			service_p -> se_jobs_p = NULL;
		}

	ClearLinkedList (& (service_p -> se_paired_services));

	CloseService (service_p);

	if (plugin_p)
		{
			DecrementPluginOpenCount (plugin_p);
		}

	FreeMemory (service_p);
}


bool CloseService (Service *service_p)
{
	#if SERVICE_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Closing %s at %.16X", GetServiceName (service_p), service_p);
	#endif

	return service_p -> se_close_fn (service_p);
}


bool IsServiceLive (Service *service_p)
{
	bool is_live_flag = false;

	if (service_p -> se_jobs_p)
		{
			is_live_flag = AreAnyJobsLive (service_p -> se_jobs_p);
		}

	return is_live_flag;
}


json_t *GetServiceResults (Service *service_p, const uuid_t service_id)
{
	json_t *results_p = NULL;

	if (service_p -> se_get_results_fn)
		{
			results_p = service_p -> se_get_results_fn (service_p, service_id);
		}

	return results_p;
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
	Service *service_p = service_node_p -> sn_service_p;

	if (service_p)
		{
			FreeService (service_p);
		}

	FreeMemory (service_node_p);
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
											json_t *reference_config_p = LoadJSONFile (reference_file_node_p -> sln_string_s);

											if (reference_config_p)
												{
													uint32 num_added_services = 0;
													char *json_s = json_dumps (reference_config_p, JSON_INDENT (2));

													json_t *services_json_p = json_object_get (reference_config_p, SERVICES_NAME_S);
													
													if (services_json_p)
														{
															const char * const plugin_name_s = GetPluginNameFromJSON (services_json_p);
															
															if (plugin_name_s)
																{
																	SetPluginNameForServiceMatcher (matcher_p, plugin_name_s);		
																	
																	num_added_services = GetMatchingServices (services_path_s, (ServiceMatcher *) matcher_p, services_json_p, services_p, true);
																}
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get service name from", reference_file_node_p -> sln_string_s);
																}
															
														}		/* if (services_json_p) */

													#if SERVICE_DEBUG >= STM_LEVEL_FINEST
													PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Added " UINT32_FMT " reference services for %s and refcount is %d", num_added_services, reference_file_node_p -> sln_string_s, config_p -> refcount);
													#endif

													if (json_s)
														{
															free (json_s);
														}

													json_decref (reference_config_p);
												}		/* if (config_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load reference file %s", reference_file_node_p -> sln_string_s);
												}

											reference_file_node_p = (StringListNode *) (reference_file_node_p -> sln_node.ln_next_p);
										}		/* while (reference_file_node_p) */
									
									FreeServiceMatcher ((ServiceMatcher *) matcher_p);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for references service matcher\n");
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
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get service from plugin with name \"%s\"", plugin_name_s);
						}
						
				}		/* if (OpenPlugin (plugin_p)) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to open plugin with name \"%s\"", plugin_name_s);
				}
				
		}		/* if (plugin_p) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate plugin with name \"%s\"", plugin_name_s);
		}

		
	return success_flag;
}



Service *GetServiceByName (const char * const service_name_s)
{
	Service *service_p = NULL;
	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);

	if (services_p)
		{
			LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, NULL);

			if (services_p -> ll_size == 1)
				{
					ServiceNode *service_node_p = (ServiceNode *) LinkedListRemHead (services_p);

					/* Detach service from node and free the node */
					service_p = service_node_p -> sn_service_p;
					service_node_p -> sn_service_p = NULL;

					FreeServiceNode ((ListItem *) service_node_p);
				}		/* if (services_p -> ll_size == 1) */

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return service_p;
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
					Service *service_p = *service_pp;
					const char *service_name_s = GetServiceName (service_p);

					if (IsServiceEnabled (service_name_s))
						{
							bool using_service_flag = RunServiceMatcher (matcher_p, service_p);
							
							if (using_service_flag)
								{
									ServiceNode *service_node_p = AllocateServiceNode (service_p);
									
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

						}		/* if (!IsServiceDisabled (service_name_s)) */

				}		/* if (*service_pp) */
				
			if (loop_flag)
				{
					-- i;
					++ service_pp;
					
					loop_flag = (i > 0);
				}
			
		}		/* while (loop_flag) */
	
	return num_matched_services;
}


uint32 GetMatchingServices (const char * const services_path_s, ServiceMatcher *matcher_p, const json_t *config_p, LinkedList *services_list_p, bool multiple_match_flag)
{
	uint32 num_matched_services = 0;
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
																	uint32 i = AddMatchingServicesFromServicesArray (services_p, services_list_p, matcher_p, multiple_match_flag);

																	#if SERVICE_DEBUG >= STM_LEVEL_FINEST
																	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Got " UINT32_FMT " services from %s with open count of " INT32_FMT, i, node_p -> sln_string_s, plugin_p -> pl_open_count);
																	#endif

																	if (i > 0)
																		{
																			using_plugin_flag = true;
																			num_matched_services += i;
																		}

																	/*
																	 * If the open count is 0, then the plugin
																	 * should get freed by the call to FreeServicesArray
																	 */
																	if (i == 0)
																		{
																			plugin_p = NULL;
																		}

																	FreeServicesArray (services_p);
																}
															else
																{
																	/* failed to get service from plugin */
																}
																
														}		/* if (OpenPlugin (plugin_p)) */


													if (plugin_p && (!using_plugin_flag))
														{
															FreePlugin (plugin_p);
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

	return num_matched_services;
}


void LoadMatchingServicesByName (LinkedList *services_p, const char * const services_path_s, const char *service_name_s, const json_t *json_config_p)
{
	NameServiceMatcher matcher;
	
	InitOperationNameServiceMatcher (&matcher, service_name_s);
	
	GetMatchingServices (services_path_s, & (matcher.nsm_base_matcher), json_config_p, services_p, true);
	
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

	AddReferenceServices (services_p, REFERENCES_PATH_S, services_path_s, NULL, json_config_p);
}


ServiceJobSet *RunService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	GenerateServiceUUID (service_p);

	return service_p -> se_run_fn (service_p, param_set_p, credentials_p);
}


ParameterSet *IsServiceMatch (Service *service_p, Resource *resource_p, Handler *handler_p)
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


static const char *GetPluginNameFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, PLUGIN_NAME_S);
}


static void GenerateServiceUUID (Service *service_p)
{
	uuid_generate (& (service_p -> se_id));
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


json_t *GetServiceRunRequest (const char * const service_name_s, const ParameterSet *params_p, const bool run_flag)
{
	json_t *service_json_p = NULL;
	json_error_t err;

	service_json_p = json_pack_ex (&err, 0, "{s:s,s:b}", SERVICE_NAME_S, service_name_s, SERVICE_RUN_S, run_flag);

	if (service_json_p)
		{
			if (run_flag)
				{
					json_t *param_set_json_p = GetParameterSetAsJSON (params_p, false);

					if (param_set_json_p)
						{
							if (json_object_set_new (service_json_p, PARAM_SET_KEY_S, param_set_json_p) == 0)
								{
									return service_json_p;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add ParameterSet JSON");
									json_decref (param_set_json_p);
								}		/* if (json_object_set_new (service_json_p, PARAM_SET_KEY_S, param_set_json_p) != 0) */

						}		/* if (param_set_json_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get ParameterSet as JSON");
						}

				}		/* if (run_flag) */
			else
				{
					return service_json_p;
				}

			json_decref (service_json_p);
		}		/* if (service_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service json, error at line %d, col %d, \"%s\"", err.line, err.column, err.text);
		}

	return NULL;
}




json_t *GetServiceAsJSON (Service * const service_p, Resource *resource_p, const json_t *json_p, const bool add_id_flag)
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
					const json_t *provider_p = GetProviderAsJSON ();

					if (provider_p)
						{
							json_t *copied_provider_p = json_deep_copy (provider_p);

							if (copied_provider_p)
								{
									/* Add any paired services details */
									if (service_p -> se_paired_services.ll_size > 0)
										{
											json_t *providers_array_p = json_array ();

											if (providers_array_p)
												{
													if (json_array_append_new (providers_array_p, copied_provider_p) == 0)
														{
															ServersManager *servers_manager_p = GetServersManager ();
															PairedServiceNode *node_p = (PairedServiceNode *) (service_p -> se_paired_services.ll_head_p);

															while (node_p)
																{
																	PairedService *paired_service_p = node_p -> psn_paired_service_p;
																	ExternalServer *external_server_p = GetExternalServerFromServersManager (servers_manager_p, paired_service_p -> ps_server_uri_s, NULL);

																	if (external_server_p)
																		{
																			json_error_t error;
																			json_t *external_provider_p = json_pack_ex (&error, 0, "{s:s,s:s,s:s}", PROVIDER_NAME_S, external_server_p -> es_name_s, PROVIDER_DESCRIPTION_S, external_server_p -> es_name_s, PROVIDER_URI_S, external_server_p ->  es_uri_s);

																			if (external_provider_p)
																				{
																					if (json_array_append_new (providers_array_p, external_provider_p) != 0)
																						{
																							json_decref (external_provider_p);
																						}
																				}

																			FreeExternalServer (external_server_p);
																		}		/* if (external_server_p) */

																	node_p = (PairedServiceNode *) (node_p -> psn_node.ln_next_p);
																}		/* while (node_p) */


															if (json_object_set_new (root_p, SERVER_PROVIDER_S, providers_array_p) != 0)
																{
																	json_decref (providers_array_p);
																}

														}		/* if (json_array_append_new (providers_array_p, copied_provider_p) == 0) */
													else
														{
															json_decref (copied_provider_p);
															json_decref (providers_array_p);
														}


												}		/* if (providers_array_p) */

										}		/* if (service_p -> se_paired_services -> ll_size > 0) */
									else
										{
											if (json_object_set_new (root_p, SERVER_PROVIDER_S, copied_provider_p) != 0)
												{
													WipeJSON (copied_provider_p);
													success_flag = false;
												}
										}
								}
							else
								{
									success_flag = false;
								}
						}
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
															if (json_object_set_new (operation_p, OPERATION_SYNCHRONOUS_S, service_p -> se_synchronous_flag ? json_true () : json_false ()) == 0)
																{
																	bool b = true;

																	if (add_id_flag)
																		{
																			if (!IsUUIDSet (service_p -> se_id))
																				{
																					GenerateServiceUUID (service_p);
																				}

																			b = AddServiceUUIDToJSON (service_p, operation_p);
																		}

																	if (b)
																		{
																			AddOperationInformationURIToJSON (service_p, operation_p);

																			success_flag = true;
																		}		/* if (b) */

																}		/* if (json_object_set_new (operation_p, OPERATION_SYNCHRONOUS_S, service_p -> se_synchronous_flag ? json_true () : json_false ()) == 0) */

														}		/* if (AddServiceParameterSetToJSON (service_p, operation_p, true, resource_p, json_p)) */

												}		/* if (AddServiceDescriptionToJSON (service_p, operation_p)) */	
																						
										}		/* if (AddServiceNameToJSON (service_p, operation_p)) */
										
									if (success_flag)
										{
											if (json_object_set_new (root_p, SERVER_OPERATIONS_S, operation_p) == 0)
												{
													success_flag = true;
												}
											else
												{
													json_decref (operation_p);
												}
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

			#if SERVICE_DEBUG >= STM_LEVEL_FINER
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
	const char *name_s = GetJSONString (root_p, SERVICES_NAME_S);

	if (!name_s)
		{
			name_s = GetJSONString (root_p, SERVICE_NAME_S);
		}

	return name_s;
}


const char *GetOperationDescriptionFromJSON (const json_t * const root_p)
{
	return GetJSONString (root_p, OPERATION_DESCRIPTION_S);
}


const char *GetOperationNameFromJSON (const json_t * const root_p)
{
	const char *result_s = GetJSONString (root_p, OPERATION_ID_S);

	if (!result_s)
		{
			result_s = GetJSONString (root_p, OPERATION_ID_OLD_S);
		}

	return result_s;
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
			success_flag = (json_object_set_new (root_p, OPERATION_ID_S, json_string (name_s)) == 0);
		}

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddServiceNameToJSON :: operationId -> ");
	#endif

	return success_flag;
}


static bool AddServiceDescriptionToJSON (Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *description_s = GetServiceDescription (service_p);

	if (description_s)
		{
			success_flag = (json_object_set_new (root_p, OPERATION_DESCRIPTION_S, json_string (description_s)) == 0);
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
					if (json_object_set_new (root_p, PARAM_SET_KEY_S, param_set_json_p) == 0)
						{
							success_flag = true;
						}
					else
						{
							json_decref (param_set_json_p);
						}
				}
				
			/* could set this to be cached ... */
			ReleaseServiceParameters (service_p, param_set_p);
		}

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddServiceParameterSetToJSON :: parameters -> ");
	#endif
	
	return success_flag;
}



bool CreateAndAddPairedService (Service *service_p, struct ExternalServer *external_server_p, const char *remote_service_name_s, const json_t *op_p)
{
	PairedService *paired_service_p = AllocatePairedService (external_server_p -> es_id, remote_service_name_s, external_server_p -> es_uri_s, external_server_p -> es_name_s, op_p);

	if (paired_service_p)
		{
			if (AddPairedService (service_p, paired_service_p))
				{
					return true;
				}

			FreePairedService (paired_service_p);
		}

	return false;
}


bool AddPairedService (Service *service_p, PairedService *paired_service_p)
{
	bool success_flag = false;
	PairedServiceNode *node_p = AllocatePairedServiceNode (paired_service_p);

	if (node_p)
		{
			LinkedListAddTail (& (service_p -> se_paired_services), (ListItem *) node_p);
			success_flag = true;
		}

	return success_flag;
}


json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p, const bool add_service_ids_flag)
{
	json_t *services_list_json_p = json_array ();
			

	if (services_list_json_p)
		{
			if (services_list_p)
				{
					ServiceNode *service_node_p = (ServiceNode *) (services_list_p -> ll_head_p);

					while (service_node_p)
						{
							json_t *service_json_p = GetServiceAsJSON (service_node_p -> sn_service_p, resource_p, json_p, add_service_ids_flag);

							#if SERVICE_DEBUG >= STM_LEVEL_FINER
							PrintJSONToLog (service_json_p, "service:\n", STM_LEVEL_FINER, __FILE__, __LINE__);
							#endif

							if (service_json_p)
								{
									if (json_array_append_new (services_list_json_p, service_json_p) != 0)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add JSON for service \"%s\" to list\n", GetServiceName (service_node_p -> sn_service_p));
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON for service \"%s\"\n", GetServiceName (service_node_p -> sn_service_p));
								}

							service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
						}

				}		/* if (services_list_p) */

			#if SERVICE_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (services_list_json_p, "services list:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
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
					IncrementPluginOpenCount (plugin_p);
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
bool AddServiceResponseHeader (Service *service_p, json_t *response_p)
{
	bool success_flag = false;
	const char *service_name_s = GetServiceName (service_p);


	if (service_name_s)
		{
			if (json_object_set_new (response_p, SERVICE_NAME_S, json_string (service_name_s)) == 0)
				{
					const char *service_description_s = GetServiceDescription (service_p);

					if (service_description_s)
						{
							if (json_object_set_new (response_p, SERVICES_DESCRIPTION_S, json_string (service_description_s)) == 0)
								{
									const char *info_uri_s = GetServiceInformationURI (service_p);

									if (info_uri_s)
										{
											if (json_object_set_new (response_p, OPERATION_INFORMATION_URI_S, json_string (info_uri_s)) != 0)
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add operation info uri \"%s\" to service response for \"%s\"", info_uri_s, service_name_s);
												}
										}

									success_flag = true;
								}
						}
				}
		}

	
	#if SERVICE_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (response_p, __FILE__, __LINE__, "service response header", SERVICE_DEBUG);
	#endif

	return success_flag;
}


ServicesArray *GetReferenceServicesFromJSON (json_t *config_p, const char *plugin_name_s, Service *(*get_service_fn) (json_t *config_p, size_t i))
{
	ServicesArray *services_p = NULL;

	#if SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (config_p, "GetReferenceServicesFromJSON: config", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

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
											if (json_is_array (ops_p))
												{
													size_t num_ops = json_array_size (ops_p);

													services_p = AllocateServicesArray (num_ops);

													if (services_p)
														{
															size_t i = 0;
															Service **service_pp = services_p -> sa_services_pp;

															while (i < num_ops)
																{
																	json_t *op_p =  json_array_get (ops_p, i);
																	json_t *copied_op_p = json_deep_copy (op_p);

																	if (copied_op_p)
																		{
																			Service *service_p = get_service_fn (copied_op_p, i);

																			if (service_p)
																				{
																					*service_pp = service_p;
																				}
																			else
																				{
																					char *dump_s = json_dumps (op_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																					if (dump_s)
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service %lu from:\n%s\n", i, dump_s);
																							free (dump_s);
																						}
																					else
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service %lu\n", i);
																						}

																					json_decref (copied_op_p);
																				}

																		}		/* if (copied_op_p) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy refererence service %lu\n", i);
																		}


																	++ i;
																	++ service_pp;
																}		/* while (i < num_ops) */

														}		/* if (services_p) */

												}		/* if (json_is_array (ops_p)) */
											else
												{
													services_p = AllocateServicesArray (1);

													if (services_p)
														{
															Service **service_pp = services_p -> sa_services_pp;
															json_t *copied_op_p = json_deep_copy (ops_p);

															if (copied_op_p)
																{
																	Service *service_p = get_service_fn (copied_op_p, 0);

																	if (service_p)
																		{
																			*service_pp = service_p;
																		}
																	else
																		{
																			char *dump_s = json_dumps (copied_op_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																			if (dump_s)
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service 0 from:\n%s\n", dump_s);
																					free (dump_s);
																				}
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service 0\n");
																				}

																			FreeServicesArray (services_p);
																			services_p = NULL;
																			json_decref (copied_op_p);
																		}

																}
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy single refererence service\n");
																}


														}		/* if (services_p)  */
												}

										}

								}		/* if (strcmp (value_s, plugin_name_s) == 0) */

						}		/* if (json_is_string (value_p)) */

				}		/* if (value_p) */

		}		/* if (config_p) */

	return services_p;
}


const json_t *GetProviderFromServiceJSON (const json_t *service_json_p)
{
	return json_object_get (service_json_p, SERVER_PROVIDER_S);
}


json_t *GetInterestedServiceJSON (const char *service_name_s, const char *keyword_s, const ParameterSet * const params_p)
{
	json_t *res_p = NULL;
	json_error_t json_err;

	res_p = json_pack_ex (&json_err, 0, "{s:s,s:b}", JOB_SERVICE_S, service_name_s, SERVICE_RUN_S, true);

	if (res_p)
		{
			json_t *params_json_p = GetParameterSetAsJSON (params_p, true);

			if (params_json_p)
				{
					if (json_object_set_new (res_p, PARAM_SET_KEY_S, params_json_p) == 0)
						{

						}
					else
						{
							json_decref (params_json_p);
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add parameters JSON to InterestedServiceJSON");
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetParameterSetAsJSON failed");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInterestedServiceJSON failed, %s", json_err.text);
		}

	return res_p;
}

