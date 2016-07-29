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

#include "json_tools.h"
#include "request_tools.h"
#include "server.h"
#include "json_util.h"
#include "streams.h"
#include "string_utils.h"
#include "schema_version.h"

#include "providers_state_table.h"



#ifdef _DEBUG
#define JSON_TOOLS_DEBUG	(STM_LEVEL_FINER)
#else
#define JSON_TOOLS_DEBUG	(STM_LEVEL_NONE)
#endif


static json_t *LoadConfig (const char *path_s);


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s);

static json_t *GetServicesInfoRequest (const uuid_t **ids_pp, const uint32 num_ids, OperationStatus status, Connection *connection_p);


void WipeJSON (json_t *json_p)
{
	if (json_p)
		{
			if (json_p -> refcount == 1)
				{
					if (json_is_array (json_p))
						{
							json_array_clear (json_p);
						}
					else if (json_is_object (json_p))
						{
							json_object_clear (json_p);
						}
				}

			json_decref (json_p);
		}
}


json_t *MakeRemoteJsonCall (json_t *req_p, Connection *connection_p)
{
	json_t *response_p = NULL;
	const char *data_s = MakeRemoteJsonCallViaConnection (connection_p, req_p);

	if (data_s)
		{
			json_error_t err;

#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "MakeRemoteJsonCall >\n%s\n", data_s);
			FlushLog ();
#endif

			response_p = json_loads (data_s, 0, &err);

			if (!response_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "error decoding response: \"%s\"\n\"%s\"\n%d %d %d\n%s\n", err.text, err.source, err.line, err.column, err.position, data_s);
				}
		}

	return response_p;
}


json_t *GetInitialisedMessage (void)
{
	json_error_t err;
	json_t *res_p = json_pack_ex (&err, 0, "{s:{s:{s:i,s:i}}}", HEADER_S, SCHEMA_S, VERSION_MAJOR_S, GetSchemaMajorVersion (), VERSION_MINOR_S, GetSchemaMinorVersion ());

	if (!res_p)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create schema json: \"%s\"\n\"%s\"\n%d %d %d\n", err.text, err.source, err.line, err.column, err.position);
		}

	return res_p;
}


json_t *GetInitialisedResponse (const json_t *req_p, const char *key_s, json_t *value_p)
{
	json_t *res_p = GetInitialisedMessage ();

	if (res_p)
		{
			if (req_p)
				{
					bool verbose_flag = false;

					GetJSONBoolean (req_p, REQUEST_VERBOSE_S, &verbose_flag);

					if (verbose_flag)
						{
							json_t *copied_req_p = json_deep_copy (req_p);

							if (copied_req_p)
								{
									if (json_object_set_new (res_p, REQUEST_S, copied_req_p) != 0)
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add request to response header");
											json_decref (copied_req_p);
										}

								}		/* if (copied_req_p) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, req_p, "Failed to add request to response header");
								}

						}		/* if (verbose_flag) */

				}		/* if (req_p) */

			if (key_s && value_p)
				{
					if (json_object_set_new (res_p, key_s, value_p) != 0)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s to the response header", key_s);
							json_decref (res_p);
						}
				}		/* if (key_s && value_p) */

		}		/* if (res_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get initialised message");
		}

	return res_p;
}



/*
 {
	credentials:
		{
			dropbox:
				{
					token_key:    onr78fxbbne0gzfy,
					token_secret: bnr6bfxwgy995su
				}
		}
}
 * */

bool AddCredentialsToJson (json_t *root_p, const UserDetails *user_p)
{
	bool success_flag = false;
	json_t *credentials_p = json_object ();

	if (credentials_p)
		{
			json_t *config_p = LoadConfig (".grassroots");

			if (config_p)
				{
					json_t *loaded_credentials_p = json_object_get (config_p, CREDENTIALS_S);

					if (loaded_credentials_p)
						{
							if (json_object_set (credentials_p, CREDENTIALS_S, loaded_credentials_p) == 0)
								{
									#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
									PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, credentials_p, "credentials: ");
									#endif

									json_object_del (config_p, CREDENTIALS_S);

									#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
									PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, credentials_p, "credentials: ");
									#endif
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add loaded credentials  to credentials json");
								}
						}

					json_decref (config_p);
				}		/* if (config_p) */



#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, credentials_p, "credentials: ");
#endif


			if (json_object_set_new (root_p, CREDENTIALS_S, credentials_p) == 0)
				{
					success_flag = true;
				}								
			else
				{
					json_decref (credentials_p);
				}
		}


#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, root_p, "root_p: ");
#endif

	return success_flag;
}


static json_t *LoadConfig (const char *path_s)
{
	json_error_t error;
	json_t *config_json_p = json_load_file (path_s, 0, &error);	

#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	char *value_s = json_dumps (config_json_p, JSON_INDENT (2));
#endif

#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	free (value_s);
#endif

	return config_json_p;
}



/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFilesRequest (const UserDetails *user_p, const char * const from_s, const char * const to_s)
{
	bool success_flag = false;
	json_t *root_p = GetOperationAsJSON (OP_IRODS_MODIFIED_DATA);

	if (root_p)
		{
			if (AddCredentialsToJson (root_p, user_p))
				{
					json_t *interval_p = json_object ();

					if (interval_p)
						{
							json_t *irods_p = json_object_get (root_p, KEY_IRODS);

							if (json_object_set_new (irods_p, "interval", interval_p) == 0)
								{
									success_flag = true;

									if (from_s)
										{
											success_flag = AddKeyAndStringValue (interval_p, "from", from_s);
										}

									if (success_flag && to_s)
										{
											success_flag = AddKeyAndStringValue (interval_p, "to", to_s);
										}

								}	
							else
								{
									json_decref (interval_p);
								}
						}
				}

			if (!success_flag)
				{
					json_object_clear (root_p);
					json_decref (root_p);
					root_p = NULL;
				}
		}		/* if (root_p) */

	return root_p;
}


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s)
{
	bool success_flag = false;
	json_t *value_p = json_string (value_s);

	if (value_p)
		{
			if (json_object_set_new (json_p, key_s, value_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					json_decref (value_p);
				}
		}

	return success_flag;
}


json_t *GetAvailableServicesRequest (const UserDetails *user_p)
{	
	json_t *op_p = GetOperationAsJSON (OP_LIST_ALL_SERVICES);

	if (op_p)
		{
			return op_p;
		}		/* if (op_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON for OP_LIST_ALL_SERVICES");
		}

	return NULL;
}


json_t *GetInterestedServicesRequest (const UserDetails *user_p, const char * const protocol_s, const char * const filename_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s, s:s}", KEY_PROTOCOL, protocol_s, KEY_FILENAME, filename_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (user_p, OP_LIST_INTERESTED_SERVICES, KEY_FILE_DATA, op_data_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInterestedServicesRequest failed for %s:%s", protocol_s, filename_s);
		}

	return res_p;
}



json_t *GetKeywordServicesRequest (const UserDetails *user_p, const char * const keyword_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s}", KEYWORDS_QUERY_S, keyword_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (user_p, OP_RUN_KEYWORD_SERVICES, KEYWORDS_QUERY_S, op_data_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetKeywordServicesRequest failed for %s", keyword_s);
		}

	return res_p;
}


json_t *GetCheckServicesRequest (const UserDetails *user_p, const char * const service_uuid_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "[s]", service_uuid_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (user_p, OP_CHECK_SERVICE_STATUS, SERVICES_NAME_S, op_data_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetCheckServicesRequest failed for %s", service_uuid_s);
		}

	return res_p;
}


json_t *GetNamedServicesRequest (const UserDetails *user_p, const char * const service_name_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "[s]", service_name_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (user_p, OP_GET_NAMED_SERVICES, SERVICES_NAME_S, op_data_p);

			if (!res_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetServicesRequest failed for %s", service_name_s);
				}

			json_decref (op_data_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create services array for %s", service_name_s);
		}

	return res_p;
}


json_t *GetServicesRequest (const UserDetails *user_p, const Operation op, const char * const op_key_s, json_t * const op_data_p)
{
	json_t *root_p = GetOperationAsJSON (op);

	if (root_p)
		{
			bool success_flag = true;

			if (user_p)
				{
					if (!AddCredentialsToJson (root_p, user_p))
						{
							char *dump_s = json_dumps (root_p, 0);

							if (dump_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add user details for %s to %s", user_p -> ud_username_s, dump_s);
									free (dump_s);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add user details for %s", user_p -> ud_username_s);
								}

						}		/* if (!AddCredentialsToJson (root_p, user_p)) */

				}		/* if (user_p) */

			if (success_flag)
				{
					if (json_object_set (root_p, op_key_s, op_data_p) == 0)
						{
							return root_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set %s", op_key_s);
						}
				}

			json_decref (root_p);
		}

	return NULL;
}


bool GetUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss)
{
	bool success_flag = false;
	char *dump_s = json_dumps (root_p, 0);

	/* 
	 * Take care of whether we have been passed the credentials group
	 * or its parent.
	 */
	const json_t *group_p = json_object_get (root_p, CREDENTIALS_S);
	if (!group_p)
		{
			group_p = root_p;
		}

	if ((*username_ss = GetJSONString (group_p, CREDENTIALS_USERNAME_S)) != NULL)
		{
			if ((*password_ss = GetJSONString (group_p, CREDENTIALS_PASSWORD_S)) != NULL)
				{
					success_flag = true;
				}
		}	

	free (dump_s);

	return success_flag;
}


json_t *GetOperationAsJSON (Operation op)
{
	json_t *msg_p = GetInitialisedMessage ();

	if (msg_p)
		{
			json_t *op_p = json_object ();

			if (op_p)
				{
					if (json_object_set_new (op_p, OPERATION_ID_S, json_integer (op)) == 0)
						{
							if (json_object_set_new (msg_p, SERVER_OPERATIONS_S, op_p) == 0)
								{
									return msg_p;
								}		/* if (json_object_set_new (msg_p, SERVER_OPERATIONS_S, op_p) == 0) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, op_p, "Failed to add ops");
								}

						}		/* if (json_object_set (op_p, OPERATION_ID_S, json_integer (op)) == 0) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, op_p, "Failed to add op id");
						}

					json_decref (op_p);
				}		/* if (op_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create op for %d", op);
				}

			json_decref (msg_p);
		}		/* if (msg_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create message for %d", op);
		}

	return NULL;
}


json_t *CallServices (json_t *client_params_json_p, const UserDetails *user_p, Connection *connection_p)
{
	json_t *services_json_p = NULL;

	if (client_params_json_p)
		{
			json_t *new_req_p = json_object ();

			if (new_req_p)
				{
					if (!AddCredentialsToJson (new_req_p, user_p))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "failed to add credentials to request");
						}

					if (json_object_set (new_req_p, SERVICES_NAME_S, client_params_json_p) == 0)
						{
							#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINER
							PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, new_req_p, "Client sending: ");
							#endif

							services_json_p = MakeRemoteJsonCall (new_req_p, connection_p);

							if (services_json_p)
								{
									#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINER
									PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, services_json_p, "Client received: ");
									#endif
								}
							else
								{
									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, new_req_p, "Empty response after client sent: ");
								}

						}		/* if (json_object_set (new_req_p, SERVICES_S, client_results_p) */

					json_decref (new_req_p);
				}		/* if (new_req_p) */
		}
	else
		{
			PrintErrors (STM_LEVEL_INFO, __FILE__, __LINE__, "no results from client");
		}

	return services_json_p;
}


const char *GetUserUUIDStringFromJSON (const json_t *credentials_p)
{
	return GetJSONString (credentials_p, CREDENTIALS_UUID_S);
}



json_t *GetServicesStatusRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p)
{
	return GetServicesInfoRequest (ids_pp, num_ids, OP_CHECK_SERVICE_STATUS, connection_p);
}


json_t *GetServicesResultsRequest (const uuid_t **ids_pp, const uint32 num_ids, Connection *connection_p)
{
	return GetServicesInfoRequest (ids_pp, num_ids, OP_GET_SERVICE_RESULTS, connection_p);
}



static json_t *GetServicesInfoRequest (const uuid_t **ids_pp, const uint32 num_ids, OperationStatus status, Connection * UNUSED_PARAM (connection_p))
{
	json_error_t error;
	json_t *req_p = json_pack_ex (&error, 0, "{s:{s:i}}", SERVER_OPERATIONS_S, OPERATION_ID_S, status);

	if (req_p)
		{
			json_t *services_p = json_array ();

			if (services_p)
				{
					if (json_object_set_new (req_p, SERVICES_NAME_S, services_p) == 0)
						{
							uint32 i = num_ids;
							const uuid_t **id_pp = ids_pp;
							bool success_flag = true;

							while ((i > 0) && success_flag)
								{
									char *uuid_s = GetUUIDAsString (**id_pp);

									if (uuid_s)
										{
											if (json_array_append_new (services_p, json_string (uuid_s)) == 0)
												{
													-- i;
													++ id_pp;
												}
											else
												{
													success_flag = false;
												}

											FreeUUIDString (uuid_s);
										}
									else
										{
											success_flag = false;
										}

								}		/* while ((i > 0) && success_flag) */

							if (success_flag)
								{
									return req_p;
								}		/* if (success_flag) */

						}		/* if (json_object_set_new- (req_p, SERVICES_NAME_S, services_p) == 0) */
					else
						{
							json_decref (services_p);
						}

				}		/* if (services_p) */

			json_object_clear (req_p);
			json_decref (req_p);
		}		/* if (req_p) */

	return NULL;
}



bool GetStatusFromJSON (const json_t *service_json_p, OperationStatus *status_p)
{
	bool success_flag = false;
	json_t *status_json_p = json_object_get (service_json_p, SERVICE_STATUS_VALUE_S);

	if (status_json_p)
		{
			if (json_is_integer (status_json_p))
				{
					int i = json_integer_value (status_json_p);

					if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT))
						{
							*status_p = (OperationStatus) i;

							success_flag = true;
						}		/* if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT)) */

				}		/* if (json_is_integer (status_p)) */

		}		/* if (status_p) */

	return success_flag;
}



bool GetUUIDFromJSON (const json_t *service_json_p, uuid_t uuid)
{
	bool success_flag = false;
	json_t *uuid_json_p = json_object_get (service_json_p, SERVICE_UUID_S);

	if (uuid_json_p)
		{
			if (json_is_string (uuid_json_p))
				{
					const char *uuid_s = json_string_value (uuid_json_p);

					if (uuid_parse (uuid_s, uuid) == 0)
						{
							success_flag = true;
						}
				}
		}

	return success_flag;
}
