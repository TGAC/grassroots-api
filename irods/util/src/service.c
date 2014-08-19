
#include "service.h"
#include "plugin.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"




static bool AddServiceNameToJSON (const Service * const service_p, json_t *root_p);

static bool AddServiceDescriptionToJSON (const Service * const service_p, json_t *root_p);

static bool AddServiceParameterSetToJSON (const Service * const service_p, json_t *root_p);




void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (void),
	const char *(*get_service_description_fn) (void),
	int (*run_fn) (const char * const filename_s, ParameterSet *param_set_p),
	bool (*match_fn) (const char * const filename_s),
	ParameterSet *(*get_parameters_fn) (void),
	ServiceData *data_p)
{
	service_p -> se_get_service_name_fn = get_service_name_fn;
	service_p -> se_get_service_description_fn = get_service_description_fn;
	service_p -> se_run_fn = run_fn;
	service_p -> se_match_fn = match_fn;
	service_p -> se_get_params_fn = get_parameters_fn;
	service_p -> se_data_p = data_p;
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
															bool using_service_flag = true;
															
															if (filename_s)
																{
																	using_service_flag = DoesFileMatchService (service_p, filename_s);
																}
															
															if (using_service_flag)
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
const char *GetServiceName (const Service *service_p)
{
	return service_p -> se_get_service_name_fn ();
}


/** Get the user-friendly description of the service. */
const char *GetServiceDescription (const Service *service_p)
{
	return service_p -> se_get_service_description_fn ();	
}


ParameterSet *GetServiceParameters (const Service *service_p)
{
	return service_p -> se_get_params_fn ();
}


json_t *GetServiceAsJSON (const Service * const service_p)
{
	json_t *root_p = json_array ();
	
	if (root_p)
		{
			const char *value_s = GetServiceName (service_p);
			bool success_flag = true;
			
			/* Add the key-value pair */
			if (value_s)
				{
					success_flag = (json_object_set_new (root_p, "path", json_string (value_s)) == 0);
				}

			if (success_flag)
				{
					value_s = GetServiceDescription (service_p);
					
					if (value_s)
						{
							success_flag = (json_object_set_new (root_p, "description", json_string (value_s)) == 0);
						}
					
					if (success_flag)
						{
							/* Add the operations for this service */
							json_t *operation_p = json_array ();
							
							if (operation_p)
								{
									success_flag = false;

									if (AddServiceNameToJSON (service_p, operation_p))
										{
											if (AddServiceDescriptionToJSON (service_p, operation_p))
												{
													if (AddServiceParameterSetToJSON (service_p, operation_p))
														{
															success_flag = true;
														}
												}		/* if (AddServiceDescriptionToJSON (service_p, operation_p)) */	
																						
										}		/* if (AddServiceNameToJSON (service_p, operation_p)) */
										
									if (success_flag)
										{
											json_array_append (root_p, operation_p);
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



static bool AddServiceNameToJSON (const Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	const char *name_s = GetServiceName (service_p);

	if (name_s)
		{
			success_flag = (json_object_set_new (root_p, "nickname", json_string (name_s)) == 0);
		}

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

	return success_flag;
}


static bool AddServiceParameterSetToJSON (const Service * const service_p, json_t *root_p)
{
	bool success_flag = false;
	ParameterSet *param_set_p = GetServiceParameters (service_p);
	
	if (param_set_p)
		{
			json_t *param_set_json_p = GetParameterSetAsJSON (param_set_p);
			
			if (param_set_json_p)
				{
					json_array_append (root_p, param_set_json_p);
					json_decref (param_set_json_p);

					success_flag = true;
				}
				
			/* could set this to be cached ... */
			FreeParameterSet (param_set_p);
		}
	
	
	return success_flag;
}







