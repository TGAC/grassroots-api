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
#include <string.h>

#include "service_job.h"
#include "service.h"

#include "string_utils.h"
#include "json_tools.h"

#include "jobs_manager.h"

#ifdef _DEBUG
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_FINER)
#else
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_NONE)
#endif



static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p, bool set_as_new_flag);

static bool AddStatusToServiceJobJSON (ServiceJob *job_p, json_t *value_p);

static bool GetOperationStatusFromServiceJobJSON (const json_t *value_p, OperationStatus *status_p);

static bool AddMappedParameterDetails (ServiceJob *job_p, const MappedParameter * const mapped_param_p, json_t *parent_p);

static bool AddLinkedServicesToServiceJobJSON (ServiceJob *job_p, json_t *value_p);


ServiceJob *AllocateEmptyServiceJob (void)
{
	ServiceJob *job_p = (ServiceJob *) AllocMemory (sizeof (ServiceJob));

	if (job_p)
		{
			memset (job_p, 0, sizeof (ServiceJob));
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE,  __FILE__, __LINE__, "Failed to allocate empty ServiceJob");
		}

	return job_p;
}



ServiceJobSet *AllocateSimpleServiceJobSet (struct Service *service_p, const char *job_name_s, const char *job_description_s)
{
	ServiceJobSet *job_set_p = AllocateServiceJobSet (service_p);

	if (job_set_p)
		{
			ServiceJob *job_p = CreateAndAddServiceJobToServiceJobSet (job_set_p, job_name_s, job_description_s, NULL, NULL);

			if (job_p)
				{
					return job_set_p;
				}

			FreeServiceJobSet (job_set_p);
		}		/* if (job_set_p) */

	return NULL;
}



ServiceJob *AllocateServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, bool (*update_fn) (struct ServiceJob *job_p), void (*free_job_fn) (struct ServiceJob *job_p))
{
	ServiceJob *job_p = AllocateEmptyServiceJob ();

	if (job_p)
		{
			if (InitServiceJob (job_p, service_p, job_name_s, job_description_s, update_fn, free_job_fn))
				{
					return job_p;
				}

			FreeServiceJob (job_p);
		}		/* if (job_p) */

	return NULL;
}


ServiceJob *CreateAndAddServiceJobToServiceJobSet (ServiceJobSet *job_set_p, const char *job_name_s, const char *job_description_s, bool (*update_fn) (struct ServiceJob *job_p), void (*free_job_fn) (struct ServiceJob *job_p))
{
	ServiceJob *job_p = AllocateServiceJob (job_set_p -> sjs_service_p, job_name_s, job_description_s, update_fn, free_job_fn);

	if (job_p)
		{
			if (AddServiceJobToServiceJobSet (job_set_p, job_p))
				{
					return job_p;
				}

			FreeServiceJob (job_p);
		}		/* if (job_p) */

	return NULL;
}


bool InitServiceJob (ServiceJob *job_p, Service *service_p, const char *job_name_s, const char *job_description_s, bool (*update_fn) (struct ServiceJob *job_p), void (*free_job_fn) (struct ServiceJob *job_p))
{
	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Initialising Job: %.16x\n", job_p);
	#endif

	memset (job_p, 0, sizeof (ServiceJob));


	job_p -> sj_service_p = service_p;
	uuid_generate (job_p -> sj_id);
	job_p -> sj_status = OS_IDLE;

	job_p -> sj_errors_p = json_object ();

	if (job_p -> sj_errors_p)
		{
			job_p -> sj_linked_services_p = json_array ();

			if (job_p -> sj_linked_services_p)
				{
					/* Set the name if valid */
						if (job_name_s)
							{
								job_p -> sj_name_s = CopyToNewString (job_name_s, 0, false);

								if (! (job_p -> sj_name_s))
									{
										PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy job name \"%s\"", job_name_s);
										return false;
									}
							}
						else
							{
								job_p -> sj_name_s = NULL;
							}

						/* set the description if valid */
						if (job_description_s)
							{
								job_p -> sj_description_s = CopyToNewString (job_description_s, 0, false);

								if (! (job_p -> sj_description_s))
									{
										PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy job description \"%s\"", job_description_s);

										if (job_p -> sj_name_s)
											{
												FreeCopiedString (job_p -> sj_name_s);
												job_p -> sj_name_s = NULL;
											}

										return false;
									}
							}
						else
							{
								job_p -> sj_description_s = NULL;
							}



					job_p -> sj_result_p = NULL;
					job_p -> sj_metadata_p = NULL;

					job_p -> sj_update_fn = update_fn;
					job_p -> sj_free_fn = free_job_fn;

					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
						{
							char uuid_s [UUID_STRING_BUFFER_SIZE];

							ConvertUUIDToString (job_p -> sj_id, uuid_s);
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Job: %s\n", uuid_s);
						}
					#endif

					return  true;
				}		/* if (job_p -> sj_linked_services_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create job linked services array");
				}

			json_decref (job_p -> sj_errors_p);
			job_p -> sj_errors_p = NULL;
		}		/* if (job_p -> sj_errors_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create job errors");
		}

	return false;


}


void ClearServiceJob (ServiceJob *job_p)
{
	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Clearing Job: %.16x\n", job_p);
	#endif

	if (job_p -> sj_name_s)
		{
			FreeCopiedString (job_p -> sj_name_s);
		}

	if (job_p -> sj_description_s)
		{
			FreeCopiedString (job_p -> sj_description_s);
		}


	if (job_p -> sj_result_p)
		{
			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (STM_LEVEL_FINER, __FILE__, __LINE__, job_p -> sj_result_p, "pre decref res_p");
			#endif

			json_decref (job_p -> sj_result_p);

			job_p -> sj_result_p = NULL;
		}

	if (job_p -> sj_metadata_p)
		{
			json_decref (job_p -> sj_metadata_p);
			job_p -> sj_metadata_p = NULL;
		}

	if (job_p -> sj_errors_p)
		{
			json_decref (job_p -> sj_errors_p);
			job_p -> sj_errors_p = NULL;
		}


	if (job_p -> sj_linked_services_p)
		{
			json_decref (job_p -> sj_linked_services_p);
			job_p -> sj_linked_services_p = NULL;
		}

	job_p -> sj_status = OS_CLEANED_UP;
}


bool SetServiceJobName (ServiceJob *job_p, const char * const name_s)
{
	bool success_flag = ReplaceStringValue (& (job_p -> sj_name_s), name_s);

	return success_flag;
}


bool SetServiceJobDescription (ServiceJob *job_p, const char * const description_s)
{
	bool success_flag = ReplaceStringValue (& (job_p -> sj_description_s), description_s);

	return success_flag;
}


void SetServiceJobStatus (ServiceJob *job_p, OperationStatus status)
{
	job_p -> sj_status = status;
}



uint32 GetNumberOfServiceJobResults (const ServiceJob *job_p)
{
	uint32 size = 0;

	if (job_p -> sj_result_p)
		{
			size = json_array_size (job_p -> sj_result_p);
		}		/* if (job_p -> sj_result_p) */

	return size;
}


void FreeServiceJob (ServiceJob *job_p)
{
	if (job_p -> sj_free_fn)
		{
			job_p -> sj_free_fn (job_p);
		}
	else
		{
			ClearServiceJob (job_p);
			FreeMemory (job_p);
		}
}


ServiceJobSet *AllocateServiceJobSet (Service *service_p)
{
	ServiceJobSet *job_set_p = (ServiceJobSet *) AllocMemory (sizeof (ServiceJobSet));

	if (job_set_p)
		{
			job_set_p -> sjs_jobs_p = AllocateLinkedList (FreeServiceJobNode);

			if (job_set_p -> sjs_jobs_p)
				{
					job_set_p -> sjs_service_p = service_p;

					if (service_p -> se_jobs_p)
						{
							FreeServiceJobSet (service_p -> se_jobs_p);
						}
					service_p -> se_jobs_p = job_set_p;

					return job_set_p;
				}

			FreeMemory (job_set_p);
		}		/* if (job_set_p) */

	return NULL;
}


void FreeServiceJobSet (ServiceJobSet *jobs_p)
{
	FreeLinkedList (jobs_p -> sjs_jobs_p);
	FreeMemory (jobs_p);
}


uint32 GetServiceJobSetSize (const ServiceJobSet * const jobs_p)
{
	return (jobs_p -> sjs_jobs_p -> ll_size);
}



ServiceJobNode *AllocateServiceJobNode (ServiceJob *job_p)
{
	ServiceJobNode *node_p = (ServiceJobNode *) AllocMemory (sizeof (ServiceJobNode));

	if (node_p)
		{
			node_p -> sjn_job_p = job_p;
			node_p -> sjn_node.ln_prev_p = NULL;
			node_p -> sjn_node.ln_next_p = NULL;
		}

	return node_p;
}


void FreeServiceJobNode (ListItem *node_p)
{
	ServiceJobNode *service_job_node_p = (ServiceJobNode *) node_p;

	FreeServiceJob (service_job_node_p -> sjn_job_p);
	FreeMemory (service_job_node_p);
}


bool AddServiceJobToServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p)
{
	bool added_flag = false;
	ServiceJobNode *node_p = AllocateServiceJobNode (job_p);

	if (node_p)
		{
			LinkedListAddTail (job_set_p -> sjs_jobs_p, (ListItem *) node_p);
			added_flag = true;
		}

	return added_flag;
}


ServiceJobNode *FindServiceJobNodeByUUIDInServiceJobSet (const ServiceJobSet *job_set_p, const uuid_t job_id)
{
	if (job_set_p)
		{
			ServiceJobNode *node_p = (ServiceJobNode *) (job_set_p -> sjs_jobs_p -> ll_head_p);

			while (node_p)
				{
					if (uuid_compare (node_p -> sjn_job_p -> sj_id, job_id) == 0)
						{
							return node_p;
						}
				}
		}

	return NULL;
}


ServiceJob *GetServiceJobFromServiceJobSetById (const ServiceJobSet *jobs_p, const uuid_t job_id)
{
	ServiceJobNode *node_p = FindServiceJobNodeByUUIDInServiceJobSet (jobs_p, job_id);
	ServiceJob *job_p = NULL;

	if (node_p)
		{
			job_p = node_p -> sjn_job_p;
		}

	return job_p;
}



ServiceJobNode *FindServiceJobNodeInServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p)
{
	if (job_set_p && job_p)
		{
			ServiceJobNode *node_p = (ServiceJobNode *) (job_set_p -> sjs_jobs_p -> ll_head_p);

			while (node_p)
				{
					if (node_p -> sjn_job_p == job_p)
						{
							return node_p;
						}
				}
		}

	return NULL;
}


bool RemoveServiceJobFromServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p)
{
	bool removed_flag = false;
	ServiceJobNode *node_p = FindServiceJobNodeInServiceJobSet (job_set_p, job_p);

	if (node_p)
		{
			LinkedListRemove (job_set_p -> sjs_jobs_p, (ListItem *) node_p);
			removed_flag = true;
		}

	return removed_flag;
}


bool CloseServiceJob (ServiceJob *job_p)
{
	bool success_flag = true;

	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Closing ServiceJob %.8X for %s", job_p, job_p -> sj_service_p);
	#endif

	if (job_p -> sj_result_p)
		{
			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (STM_LEVEL_FINER, __FILE__, __LINE__, job_p -> sj_result_p, "pre wipe results");
			#endif

			WipeJSON (job_p -> sj_result_p);

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (STM_LEVEL_FINER, __FILE__, __LINE__, job_p -> sj_result_p, "post wipe results");
			#endif

			job_p -> sj_result_p = NULL;
		}

	return success_flag;
}


void ClearServiceJobResults (ServiceJob *job_p, bool free_memory_flag)
{
	if (job_p -> sj_result_p)
		{
			if (free_memory_flag)
				{
					json_decref (job_p -> sj_result_p);
				}

			job_p -> sj_result_p = NULL;
		}
}



static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p, bool set_as_new_flag)
{
	bool success_flag = true;

	if (child_p)
		{
			if (set_as_new_flag)
				{
					if (json_object_set_new (parent_p, key_s, child_p) != 0)
						{
							success_flag = false;
						}
				}
			else
				{
					if (json_object_set (parent_p, key_s, child_p) != 0)
						{
							success_flag = false;
						}
				}
		}

	if (!success_flag)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add child json for \"%s\" set_as_new_flag: %d", key_s, set_as_new_flag);
		}

	return success_flag;
}


static bool AddStatusToServiceJobJSON (ServiceJob *job_p, json_t *value_p)
{
	bool success_flag = false;

	if (json_is_object (value_p))
		{
			if (json_object_set_new (value_p, SERVICE_STATUS_VALUE_S, json_integer (job_p -> sj_status)) == 0)
				{
					const char *status_text_s = GetOperationStatusAsString (job_p -> sj_status);

					if (status_text_s)
						{
							if (json_object_set_new (value_p, SERVICE_STATUS_S, json_string (status_text_s)) == 0)
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status text \"%s\" to json", status_text_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get status text for ", job_p -> sj_status);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status value \"%d\" to json", job_p -> sj_status);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Job status JSON can only be added to an object, value is of type %d", json_typeof (value_p));
		}

	return success_flag;
}


static bool AddLinkedServicesToServiceJobJSON (ServiceJob *job_p, json_t *value_p)
{
	bool success_flag = true;

	if ((job_p -> sj_linked_services_p) && (json_array_size (job_p -> sj_linked_services_p) > 0))
		{

		}

	return success_flag;
}


static bool GetOperationStatusFromServiceJobJSON (const json_t *value_p, OperationStatus *status_p)
{
	bool success_flag = false;

	if (json_is_object (value_p))
		{
			int i;

			if (GetJSONInteger (value_p, SERVICE_STATUS_VALUE_S, &i))
				{
					OperationStatus s = (OperationStatus) i;

					if ((s > OS_LOWER_LIMIT) && (s < OS_UPPER_LIMIT))
						{
							*status_p = s;
							success_flag = true;
						}
				}
			else
				{
					const char *status_s = GetJSONString (value_p, SERVICE_STATUS_S);

					if (status_s)
						{
							OperationStatus s = GetOperationStatusFromString (status_s);

							if (s != OS_NUM_STATUSES)
								{
									*status_p = s;
									success_flag = true;
								}
						}
				}		/* if (!GetJSONInteger (value_p, SERVICE_STATUS_VALUE_S, &i)) */
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Job status JSON can only be got from an object, value is of type %d", json_typeof (value_p));
		}

	return success_flag;
}



bool InitServiceJobFromJSON (ServiceJob *job_p, const json_t *job_json_p)
{
	bool success_flag = false;

	if (job_json_p)
		{
			char *job_json_s = json_dumps (job_json_p, JSON_INDENT (2));
			const char *service_name_s = GetJSONString (job_json_p, JOB_SERVICE_S);

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Starting InitServiceJobFromJSON for \"%s\"  at %16X", job_json_s, job_json_p);
			#endif


			if (service_name_s)
				{
					const char *uuid_s = GetJSONString (job_json_p, JOB_UUID_S);

					if (uuid_s)
						{
							const char *job_name_s = GetJSONString (job_json_p, JOB_NAME_S);
							const char *job_description_s  = GetJSONString (job_json_p, JOB_DESCRIPTION_S);
							json_t *job_results_p = json_object_get (job_json_p, JOB_RESULTS_S);
							json_t *job_metadata_p = json_object_get (job_json_p, JOB_METADATA_S);
							json_t *job_errors_p = json_object_get (job_json_p, JOB_ERRORS_S);
							OperationStatus status;

							if (GetOperationStatusFromServiceJobJSON (job_json_p, &status))
								{
									Service *service_p = GetServiceByName (service_name_s);

									if (service_p)
										{
											if (uuid_parse (uuid_s, job_p -> sj_id) == 0)
												{
													if (job_name_s)
														{
															job_p -> sj_name_s = CopyToNewString (job_name_s, 0, false);

															if (! (job_p -> sj_name_s))
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to copy name \"%s\" to job", job_name_s);
																}
														}
													else
														{
															job_p -> sj_name_s = NULL;
														}

													if (job_description_s)
														{
															job_p -> sj_description_s = CopyToNewString (job_description_s, 0, false);

															if (! (job_p -> sj_description_s))
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to copy description \"%s\" to job", job_description_s);
																}
														}
													else
														{
															job_p -> sj_description_s = NULL;
														}

													job_p -> sj_errors_p = job_errors_p;
													job_p -> sj_metadata_p = job_metadata_p;
													job_p -> sj_result_p = job_results_p;
													job_p -> sj_service_p = service_p;
													job_p -> sj_status = status;

													SetServiceJobCustomFunctions (service_p, job_p);

													success_flag = true;

												}		/* if (uuid_parse (uuid_s, job_p -> sj_id) == 0) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't parse uuid \"%s\" as valid uuid", uuid_s);
												}


										}		/* if (service_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get service \"%s\"", service_name_s);
										}

								}		/* if (GetOperationStatusFromServiceJobJSON (job_json_p, &status)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get status from JSON \"%s\"", job_json_s ? job_json_s : "");
								}

						}		/* if (uuid_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get uuid from JSON \"%s\"", job_json_s ? job_json_s : "");
						}

				}		/* if (service_name_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get service name from JSON \"%s\"", job_json_s ? job_json_s : "");
				}

			if (job_json_s)
				{
					FreeCopiedString (job_json_s);
				}
		}		/* if (job_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Job JSON is NULL");
		}


	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Ending InitServiceJobFromJSON, success = %d", success_flag);
	#endif

	return success_flag;
}


ServiceJob *CreateServiceJobFromJSON (const json_t *job_json_p)
{
	ServiceJob *job_p = NULL;
	const char *service_name_s = GetJSONString (job_json_p, JOB_SERVICE_S);

	/* Check whether the job needs a custom deserialiser */
	if (service_name_s)
		{
			Service *service_p = GetServiceByName (service_name_s);

			if (DoesServiceHaveCustomServiceJobSerialisation (service_p))
				{
					job_p = CreateSerialisedServiceJobFromService (service_p, job_json_p);

					if (job_p)
						{
							return job_p;
						}		/* if (job_p) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_json_p, "Failed to create ServiceJob");
						}

				}		/* if (DoesServiceHaveCustomServiceJobSerialisation (service_p)) */
			else
				{
					job_p = AllocateEmptyServiceJob ();

					if (job_p)
						{
							if (InitServiceJobFromJSON (job_p, job_json_p))
								{
									return job_p;
								}
							else
								{
									FreeServiceJob (job_p);
								}
						}
				}
		}		/* if (service_name_s) */


	return NULL;
}


json_t *GetServiceJobAsJSON (ServiceJob *job_p)
{
	json_t *job_json_p = json_object ();

	if (job_json_p)
		{
			bool success_flag = false;
			const char *service_name_s = GetServiceName (job_p -> sj_service_p);

			if (json_object_set_new (job_json_p, JOB_SERVICE_S, json_string (service_name_s)) == 0)
				{
					if (AddValidJSON (job_json_p, JOB_RESULTS_S, job_p -> sj_result_p, false))
						{
							if (AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p, false))
								{
									if (AddValidJSON (job_json_p, JOB_METADATA_S, job_p -> sj_metadata_p, false))
										{
											if (AddStatusToServiceJobJSON (job_p, job_json_p))
												{
													char buffer_s [UUID_STRING_BUFFER_SIZE];

													ConvertUUIDToString (job_p -> sj_id, buffer_s);

													if (json_object_set_new (job_json_p, JOB_UUID_S, json_string (buffer_s)) == 0)
														{
															if (AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s))
																{
																	if (AddValidJSONString (job_json_p, JOB_DESCRIPTION_S, job_p -> sj_description_s))
																		{
																			if (!AddLinkedServicesToServiceJobJSON (job_p, job_json_p))
																				{
																					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to Linked Services for %s to json", job_p -> sj_name_s ? job_p -> sj_name_s : "unnamed job");
																				}

																			success_flag = true;
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add description \"%s\" %s to json", job_p -> sj_description_s ? job_p -> sj_description_s : "");
																		}

																}		/* if (AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s)) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add name \"%s\" to json", job_p -> sj_name_s ? job_p -> sj_name_s : "");
																}

														}		/* if (json_object_set_new (job_json_p, JOB_UUID_S, json_string (buffer_s)) == 0) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add uuid %s to json", buffer_s);
														}

												}		/* if (AddStatusToServiceJobJSON (job_p, job_json_p)) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status to service job json");
												}

										}		/* if (AddValidJSON (job_json_p, JOB_METADATA_S, job_p -> sj_metadata_p, false)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add metadata to service job json");
										}

								}		/* if (AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p, false)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add errors to service job json");
								}

						}		/* if (AddValidJSON (job_json_p, JOB_RESULTS_S, results_json_p, false)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add results to service job json");
						}

				}		/* if (json_object_set_new (SERVICE_NAME_S, json_string (service_name_s)) == 0) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service name \"%s\" to json", service_name_s);
				}


			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "service job: ");
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "success_flag %d", success_flag);
			#endif

			if (!success_flag)
				{
					json_decref (job_json_p);
					job_json_p = NULL;
				}
		}		/* if (job_json_p) */


	return job_json_p;
}


bool ProcessServiceJobSet (ServiceJobSet *jobs_p, json_t *res_p)
{
	bool success_flag = true;
	uint32 i = 0;
	ServiceJobNode *node_p = (ServiceJobNode *) (jobs_p -> sjs_jobs_p -> ll_head_p);

	while (node_p)
		{
			ServiceJob *job_p = node_p -> sjn_job_p;

			json_t *job_json_p = NULL;
			const OperationStatus job_status = GetServiceJobStatus (job_p);

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Job " UINT32_FMT ": status: %d", i, job_status);
			#endif

			if ((job_status == OS_SUCCEEDED) || (job_status == OS_PARTIALLY_SUCCEEDED))
				{
					job_json_p = GetServiceJobAsJSON (job_p);

					/*
					 * If this service has any linked services, fill in the data here
					 */
					ProcessLinkedServices (job_p);
				}
			else
				{
					job_json_p = GetServiceJobStatusAsJSON (job_p);
				}

			if (job_json_p)
				{
					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
					PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, job_json_p, "Job JSON");
					#endif

					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
					PrintJSONRefCounts (STM_LEVEL_FINE, __FILE__, __LINE__, job_json_p, "Job JSON");
					#endif

					if (json_array_append_new (res_p, job_json_p) == 0)
						{
							#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
							PrintJSONRefCounts (STM_LEVEL_FINE, __FILE__, __LINE__, job_json_p, "after array adding, job json ");
							#endif
						}
					else
						{
							char *uuid_s = GetUUIDAsString (job_p -> sj_id);

							if (uuid_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job results %s to json response", uuid_s);
									FreeCopiedString (uuid_s);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job results %s to json response", job_p -> sj_name_s);
								}

							WipeJSON (job_json_p);
						}
				}
			else
				{
					char *uuid_s = GetUUIDAsString (job_p -> sj_id);

					if (uuid_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to json response for json %s", uuid_s);
							FreeCopiedString (uuid_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to json response for json %s", job_p -> sj_name_s);
						}
				}

			node_p = (ServiceJobNode *) (node_p -> sjn_node.ln_next_p);
			++ i;
		}		/* while (node_p) */


	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, res_p, "service json: ");
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "success_flag %s", success_flag ? "true" : "false");
	#endif

	return success_flag;
}


OperationStatus GetCachedServiceJobStatus (const ServiceJob *job_p)
{
	return job_p -> sj_status;
}


void ProcessLinkedServices (ServiceJob *job_p)
{
	Service *service_p = job_p -> sj_service_p;

	if (service_p -> se_linked_services.ll_size)
		{
			LinkedServiceNode *linked_service_node_p = (LinkedServiceNode *) (service_p -> se_linked_services.ll_head_p);

			while (linked_service_node_p)
				{
					LinkedService *linked_service_p = linked_service_node_p -> lsn_linked_service_p;

					if (!AddLinkedServiceToServiceJob (job_p, linked_service_p))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add Linked Service for \"%s\" to service job", linked_service_p -> ls_input_service_s);
						}

					linked_service_node_p = (LinkedServiceNode *) (linked_service_node_p -> lsn_node.ln_next_p);
				}		/* while (linked_service_node_p) */

		}		/* if (service_p -> se_linked_services.ll_size) */
}

OperationStatus GetServiceJobStatus (ServiceJob *job_p)
{
	/* If job has been started or is waiting to run, check its status */
	/*
	OS_LOWER_LIMIT = -4,
	OS_FAILED = -3,
	OS_FAILED_TO_START = -2,
	OS_ERROR = -1,
	OS_IDLE = 0,
	OS_PENDING,
	OS_STARTED,
	OS_FINISHED,
	OS_PARTIALLY_SUCCEEDED,
	OS_SUCCEEDED,
	OS_CLEANED_UP,
	OS_UPPER_LIMIT,
	OS_NUM_STATUSES = OS_UPPER_LIMIT - OS_LOWER_LIMIT + 1
	 */
	switch (job_p -> sj_status)
		{
			case OS_IDLE:
			case OS_PENDING:
			case OS_STARTED:
				if (!UpdateServiceJob (job_p))
					{
						char uuid_s [UUID_STRING_BUFFER_SIZE];
						const char *service_name_s = GetServiceName (job_p -> sj_service_p);

						ConvertUUIDToString (job_p -> sj_id, uuid_s);

						PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to update ServiceJob %s for service %s", uuid_s, service_name_s);
					}
				break;

			default:
				break;
		}

	return job_p -> sj_status;
}



json_t *GetServiceJobStatusAsJSON (ServiceJob *job_p)
{
	json_t *job_json_p = json_object ();

	if (job_json_p)
		{
			char *uuid_s = GetUUIDAsString (job_p -> sj_id);

			/* Make sure that the job's status is up to date */
			GetServiceJobStatus (job_p);

			if (uuid_s)
				{
					if (json_object_set_new (job_json_p, JOB_UUID_S, json_string (uuid_s)) == 0)
						{
							#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
							if (job_json_p)
								{
									PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding uuid:");
								}
							#endif

							if (AddStatusToServiceJobJSON (job_p, job_json_p))
								{
									const char *service_name_s = GetServiceName (job_p -> sj_service_p);

									#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
									if (job_json_p)
										{
											PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding status:");
										}
									#endif

									if (!AddValidJSONString (job_json_p, JOB_SERVICE_S, service_name_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add service %s to job status json", service_name_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding service_name_s:");
	}
#endif

									if (!AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add job name %s to job status json", job_p -> sj_name_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding job_p -> sj_name_s:");
	}
#endif
									if (!AddValidJSONString (job_json_p, JOB_DESCRIPTION_S, job_p -> sj_description_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add job description %s to job status json", job_p -> sj_description_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding job_p -> sj_description_s:");
	}
#endif
									if (!AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p, false))
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job errors to job %s, %s", job_p -> sj_name_s, uuid_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, job_json_p, "Job after adding JOB_ERRORS_S:");
	}
#endif
								}		/* if (AddStatusToServiceJobJSON (job_p, job_json_p)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to  add status for job uuid \"%s\" to json", uuid_s);
								}

						}		/* if (json_object_set_new (job_json_p, JOB_UUID_S, uuid_s) == 0) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job uuid \"%s\" as json  for job %s", uuid_s, job_p -> sj_name_s ? job_p -> sj_name_s : "");
						}

					FreeUUIDString (uuid_s);
				}		/* if (uuid_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job uuid as string for job %s", job_p -> sj_name_s ? job_p -> sj_name_s : "");
				}

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
			if (job_json_p)
				{
					PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, job_json_p, "Job:");
				}
			#endif

		}		/* if (json_p) */

	return job_json_p;
}



bool SetServiceJobFromJSON (ServiceJob *job_p, const json_t *json_p)
{
	bool success_flag = false;

	return success_flag;
}


json_t *GetServiceJobSetAsJSON (const ServiceJobSet *jobs_p)
{
	json_t *jobs_json_p = json_array ();

	if (jobs_json_p)
		{
			ServiceJobNode *node_p = (ServiceJobNode *) (jobs_p -> sjs_jobs_p -> ll_head_p);

			while (node_p)
				{
					ServiceJob *job_p = node_p -> sjn_job_p;
					json_t *job_json_p = GetServiceJobAsJSON (job_p);

					if (job_json_p)
						{
							if (json_array_append_new (jobs_json_p, job_json_p) != 0)
								{
									char *uuid_s = GetUUIDAsString (job_p -> sj_id);

									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to append job %s to results", uuid_s ? uuid_s : "");

									if (uuid_s)
										{
											FreeUUIDString (uuid_s);
										}
								}
						}
					else
						{
							char *uuid_s = GetUUIDAsString (job_p -> sj_id);

							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job %s as json", uuid_s ? uuid_s : "");

							if (uuid_s)
								{
									FreeUUIDString (uuid_s);
								}
						}

					node_p = (ServiceJobNode *) (node_p -> sjn_node.ln_next_p);
				}		/* while (node_p) */
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate json jobset array");
		}

	return jobs_json_p;
}



bool AreAnyJobsLive (const ServiceJobSet *jobs_p)
{
	ServiceJobNode *node_p = (ServiceJobNode *) (jobs_p -> sjs_jobs_p -> ll_head_p);

	while (node_p)
		{
			ServiceJob *job_p = node_p -> sjn_job_p;

			switch (job_p -> sj_status)
				{
					case OS_IDLE:
					case OS_PENDING:
					case OS_STARTED:
					case OS_SUCCEEDED:
					case OS_FINISHED:
						return true;

					default:
						break;
				}

			node_p = (ServiceJobNode *) (node_p -> sjn_node.ln_next_p);
		}		/* while (node_p) */

	return false;
}


char *SerialiseServiceJobToJSON (ServiceJob * const job_p)
{
	char *serialised_data_p = NULL;
	json_t *job_json_p = GetServiceJobAsJSON (job_p);

	if (job_json_p)
		{
			serialised_data_p = json_dumps (job_json_p, JSON_INDENT (2));
			json_decref (job_json_p);
		}		/* if (job_json_p) */

	return serialised_data_p;
}


ServiceJob *DeserialiseServiceJobFromJSON (char *raw_json_data_s)
{
	ServiceJob *job_p = NULL;
	json_error_t err;
	json_t *job_json_p = json_loads (raw_json_data_s, 0, &err);

	if (job_json_p)
		{

		}		/* if (job_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load service json from \"%s\"", raw_json_data_s);
		}

	return job_p;
}


ServiceJob *GetServiceJobFromServiceJobSet (const ServiceJobSet *jobs_p, const uint32 job_index)
{
	ServiceJob *job_p = NULL;

	if (job_index < (jobs_p -> sjs_jobs_p -> ll_size))
		{
			ListItem *node_p = jobs_p -> sjs_jobs_p -> ll_head_p;
			uint32 i;

			for (i = job_index; i > 0; -- i)
				{
					node_p = node_p -> ln_next_p;
				}

			job_p = ((ServiceJobNode *) node_p) -> sjn_job_p;
		}

	return job_p;
}


bool InitServiceJobFromResultsJSON (ServiceJob *job_p, const json_t *results_p, Service *service_p, const char *name_s, const char *description_s, OperationStatus status)
{
	bool success_flag = true;
	OperationStatus (*update_status_fn) (ServiceJob *job_p) = NULL;

	memset (job_p, 0, sizeof (*job_p));

	if (name_s)
		{
			job_p -> sj_name_s = CopyToNewString (name_s, 0, false);

			if (job_p -> sj_name_s)
				{
					if (description_s)
						{
							job_p -> sj_description_s = CopyToNewString (description_s, 0, false);

							if (! (job_p -> sj_description_s))
								{
									success_flag = false;
								}		/* if (! (job_p -> sj_description_s)) */
						}

					if (success_flag)
						{
							const char *value_s = GetJSONString (results_p, RESOURCE_PROTOCOL_S);

							job_p -> sj_status = status;
							job_p -> sj_service_p = service_p;
							job_p -> sj_status = status;

							job_p -> sj_update_fn = NULL;
							job_p -> sj_free_fn = NULL;

							uuid_generate (job_p -> sj_id);

							if (value_s)
								{
									if (strcmp (value_s, PROTOCOL_INLINE_S) == 0)
										{
											json_t *data_p = json_object_get (results_p, RESOURCE_DATA_S);

											if (data_p)
												{
													json_t *results_array_p = json_array ();

													if (results_array_p)
														{
															char uuid_s [UUID_STRING_BUFFER_SIZE];
															json_t *resource_p = NULL;

															ConvertUUIDToString (job_p -> sj_id, uuid_s);
															resource_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, uuid_s, data_p);

															if (resource_p)
																{
																	if (json_array_append_new (results_array_p, resource_p) == 0)
																		{
																			job_p -> sj_result_p = results_array_p;
																		}		/* if (json_array_append_new (results_array_p, resource_p) == 0) */
																	else
																		{
																			json_decref (resource_p);
																		}
																}

															if (! (job_p -> sj_result_p))
																{
																	json_decref (results_array_p);
																}
														}

												}		/* if (data_p) */

										}
								}

						}

				}		/* if (job_p -> sj_name_s) */
			else
				{
					success_flag = false;
				}
		}


	return success_flag;
}


ServiceJob *CreateServiceJobFromResultsJSON (const json_t *results_p, Service *service_p, const char *name_s, const char *description_s, OperationStatus status)
{
	ServiceJob *job_p = (ServiceJob *) AllocMemory (sizeof (ServiceJob));

	if (job_p)
		{
			if (InitServiceJobFromResultsJSON (job_p, results_p, service_p, name_s, description_s, status))
				{
					return job_p;
				}

			FreeMemory (job_p);
		}		/* if (job_p) */

	return NULL;
}


bool AddErrorToServiceJob (ServiceJob *job_p, const char * const key_s, const char * const value_s)
{
	if (json_object_set_new (job_p -> sj_errors_p, key_s, json_string (value_s)) == 0)
		{
			return true;
		}

	return false;
}


bool AddCompoundErrorToServiceJob (ServiceJob *job_p, const char * const key_s, json_t *values_p, const bool claim_flag)
{
	bool success_flag = false;

	if (claim_flag)
		{
			success_flag = (json_object_set_new (job_p -> sj_errors_p, key_s, values_p) == 0);
		}
	else
		{
			success_flag = (json_object_set (job_p -> sj_errors_p, key_s, values_p) == 0);
		}

	return success_flag;
}



bool UpdateServiceJob (ServiceJob *job_p)
{
	return ((job_p -> sj_update_fn) ? (job_p -> sj_update_fn (job_p)) : true);
}



void SetServiceJobUpdateFunction (ServiceJob *job_p, bool (*update_fn) (ServiceJob *job_p))
{
	job_p -> sj_update_fn = update_fn;
}



bool AddResultToServiceJob (ServiceJob *job_p, json_t *result_p)
{
	bool success_flag = false;

	if (! (job_p -> sj_result_p))
		{
			job_p -> sj_result_p = json_array ();

			if (! (job_p -> sj_result_p))
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (job_p -> sj_id, uuid_s);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate results array for %s", uuid_s);
				}

		}		/* if (! (job_p -> sj_result_p)) */

	if (job_p -> sj_result_p)
		{
			if (json_array_append_new (job_p -> sj_result_p, result_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (job_p -> sj_id, uuid_s);
					PrintJSONToErrors(STM_LEVEL_SEVERE, __FILE__, __LINE__, result_p, "Failed to add result to %s", uuid_s);
				}

		}		/* if (job_p -> sj_result_p) */

	return success_flag;
}


bool ReplaceServiceJobResults (ServiceJob *job_p, json_t *results_p)
{
	bool success_flag = false;

	if (results_p)
		{
			if (json_is_array (results_p))
				{
					if (job_p -> sj_result_p)
						{
							json_decref (job_p -> sj_result_p);
						}

					job_p -> sj_result_p = results_p;
					success_flag = true;

				}		/* if (json_is_array (results_p)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "New ServiceJob results is not an array %d", json_typeof (results_p));
				}

		}		/* if (results_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "New ServiceJob results is NULL");
		}

	return success_flag;
}



bool AddLinkedServiceToServiceJob (ServiceJob *job_p, LinkedService *linked_service_p)
{
	bool success_flag = false;
	json_t *linked_service_json_p = GetLinkedServiceAsJSON (linked_service_p);

	if (linked_service_json_p)
		{
			if (json_array_append_new (job_p -> sj_linked_services_p, linked_service_json_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add linked service json object to job \"%s\"", job_p -> sj_name_s);
				}
		}		/* if (linked_service_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create linked service json objectjob \"%s\"", job_p -> sj_name_s);
		}

	return success_flag;
}



static bool AddMappedParameterDetails (ServiceJob *job_p, const MappedParameter * const mapped_param_p, json_t *parent_p)
{
	char *value_s = GetValueFromJobOutput (job_p -> sj_service_p, job_p, mapped_param_p -> mp_input_param_s);

	if (value_s)
		{
			json_t *param_json_p = json_object ();

			if (param_json_p)
				{
					if (json_object_set_new (param_json_p, PARAM_NAME_S, json_string (mapped_param_p -> mp_output_param_s)) == 0)
						{
							if (json_object_set_new (param_json_p, PARAM_CURRENT_VALUE_S, json_string (value_s)) == 0)
								{
									if (json_array_append_new (parent_p, param_json_p) == 0)
										{
											return true;
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to append params json to parent object");
										}

								}		/* if (json_object_set_new (linked_service_json_p, PARAM_CURRENT_VALUE_S, json_string (value_s)) == 0) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\": \"%s\" to param json", PARAM_CURRENT_VALUE_S, value_s);
								}

						}		/* if (json_object_set_new (linked_service_json_p, PARAM_NAME_S, json_string (mapped_param_p -> mp_output_param_s)) == 0) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\": \"%s\" to param json", PARAM_NAME_S, mapped_param_p -> mp_output_param_s);
						}

					json_decref (param_json_p);
				}		/* if (param_json_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate JSON for params object");
				}

			FreeCopiedString (value_s);
		}		/* if (value_s) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_p -> sj_result_p, "Failed to get \"%s\"", mapped_param_p -> mp_input_param_s);
		}

	return false;
}

