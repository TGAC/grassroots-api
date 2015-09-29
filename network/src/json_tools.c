
#include "json_tools.h"
#include "request_tools.h"
#include "server.h"
#include "json_util.h"
#include "streams.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define JSON_TOOLS_DEBUG	(STM_LEVEL_FINER)
#else
	#define JSON_TOOLS_DEBUG	(STM_LEVEL_NONE)
#endif


json_t *LoadConfig (const char *path_s);


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s);

static json_t *GetServicesInfoRequest (const uuid_t **ids_pp, const uint32 num_ids, OperationStatus status, Connection *connection_p);


void WipeJSON (json_t *json_p)
{
	if (json_p)
		{
			if (json_is_array (json_p))
				{
					json_array_clear (json_p);
				}
			else if (json_is_object (json_p))
				{
					json_object_clear (json_p);
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
			PrintLog (STM_LEVEL_FINER, "MakeRemoteJsonCall >\n%s\n", data_s);
			FlushLog ();
			#endif

			response_p = json_loads (data_s, 0, &err);

			if (!response_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "error decoding response: \"%s\"\n\"%s\"\n%d %d %d\n%s\n", err.text, err.source, err.line, err.column, err.position, data_s);
				}
		}

	return response_p;
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

bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s)
{
	bool success_flag = false;
	json_error_t error;
	json_t *credentials_p = json_pack_ex (&error, 0, "{s:s,s:s}", CREDENTIALS_USERNAME_S, username_s, CREDENTIALS_PASSWORD_S, password_s);

	if (credentials_p)
		{
			json_t *config_p = LoadConfig (".wheatis");
			
			if (config_p)
				{
					json_t *loaded_credentials_p = json_object_get (config_p, CREDENTIALS_S);
					
					if (loaded_credentials_p)
						{
							const char *key_s;
							json_t *value_p;

							json_object_foreach (loaded_credentials_p, key_s, value_p) 
								{
									json_object_set (credentials_p, key_s, value_p);
									json_object_del (config_p, key_s);
								}
						}		
						
					json_decref (config_p);
				}
			
			
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
	PrintJSONToLog (credentials_p, "credentials: ", JSON_TOOLS_DEBUG);
	#endif

	return success_flag;
}


json_t *LoadConfig (const char *path_s)
{
	bool success_flag = true;
	json_error_t error;
	json_t *config_json_p = json_load_file (path_s, 0, &error);	

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	char *value_s = json_dumps (config_json_p, JSON_INDENT (2));
	#endif


	if (!config_json_p)
		{
			success_flag = false;
		}		/* if (config_json_p) */

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	free (value_s);
	#endif


	return config_json_p;
}



/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	bool success_flag = false;
	json_t *root_p = GetOperationAsJSON (OP_IRODS_MODIFIED_DATA);

	if (root_p)
		{
			if (AddCredentialsToJson (root_p, username_s, password_s))
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




/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s)
{	
	json_t *json_p = GetOperationAsJSON (OP_LIST_ALL_SERVICES);
	
	return json_p;
}


json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, const char * const protocol_s, const char * const filename_s)
{

	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s, s:s}", KEY_PROTOCOL, protocol_s, KEY_FILENAME, filename_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_LIST_INTERESTED_SERVICES, KEY_FILE_DATA, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetKeywordServicesRequest (const char * const username_s, const char * const password_s, const char * const keyword_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s}", KEY_QUERY, keyword_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_RUN_KEYWORD_SERVICES, KEY_QUERY, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetCheckServicesRequest (const char * const username_s, const char * const password_s, const char * const service_uuid_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "[s]", service_uuid_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_CHECK_SERVICE_STATUS, SERVICES_NAME_S, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetNamedServicesRequest (const char * const username_s, const char * const password_s, const char * const service_name_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "[s]", service_name_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_GET_NAMED_SERVICES, SERVICES_NAME_S, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetServicesRequest (const char * const username_s, const char * const password_s, const Operation op, const char * const op_key_s, json_t * const op_data_p)
{
	json_t *root_p = GetOperationAsJSON (op);
	bool success_flag = false;

	if (root_p)
		{
			if (AddCredentialsToJson (root_p, username_s, password_s))
				{
					if (json_object_set (root_p, op_key_s, op_data_p) == 0)
						{
							success_flag = true;
						}
				}
		}

	if (!success_flag)
		{
			json_object_clear (root_p);
			json_decref (root_p);
			root_p = NULL;
		}

	return root_p;
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
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:i}}", SERVER_OPERATIONS_S, OPERATION_ID_S, op);

	if (!json_p)
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to get Operation %d as JSON, %s\n", op, error.text);
		}
	
	return json_p;
}




json_t *CallServices (json_t *client_params_json_p, const char * const username_s, const char * const password_s, Connection *connection_p)
{
	json_t *services_json_p = NULL;

	if (client_params_json_p)
		{
			char *client_results_s = json_dumps (client_params_json_p, JSON_INDENT (2));
			json_t *new_req_p = json_object ();

			if (new_req_p)
				{
					if (!AddCredentialsToJson (new_req_p, username_s, password_s))
						{
							printf ("failed to add credentials to request\n");
						}

					if (json_object_set_new (new_req_p, SERVICES_NAME_S, client_params_json_p) == 0)
						{
							char *new_req_s  = json_dumps (new_req_p, JSON_INDENT (2));

							printf ("client sending:\n%s\n", new_req_s);

							services_json_p = MakeRemoteJsonCall (new_req_p, connection_p);

							if (services_json_p)
								{
									char *response_s = json_dumps (services_json_p, JSON_INDENT (2));

									if (response_s)
										{
											printf ("%s\n", response_s);
											free (response_s);
										}
								}
							else
								{
									printf ("no response\n");
								}

							if (new_req_s)
								{
									free (new_req_s);
								}

						}		/* if (json_object_set_new (new_req_p, SERVICES_S, client_results_p) */

					json_decref (new_req_p);

				}		/* if (new_req_p) */

			if (client_results_s)
				{
					printf ("%s\n", client_results_s);
					free (client_results_s);
				}

		}
	else
		{
			printf ("no results from client\n");
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



static json_t *GetServicesInfoRequest (const uuid_t **ids_pp, const uint32 num_ids, OperationStatus status, Connection *conection_p)
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

