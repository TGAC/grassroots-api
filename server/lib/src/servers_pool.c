#include <string.h>

#include "servers_pool.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "streams.h"
#include "json_util.h"


void InitServersManager (ServersManager *manager_p,
                      bool (*add_server_fn) (ServersManager *manager_p, ExternalServer *server_p),
											ExternalServer *(*get_server_fn)  (ServersManager *manager_p, const uuid_t key),
											ExternalServer *(*remove_server_fn) (ServersManager *manager_p, const uuid_t key))
{
	manager_p -> sm_add_server_fn = add_server_fn;
	manager_p -> sm_get_server_fn = get_server_fn;
	manager_p -> sm_remove_server_fn = remove_server_fn;
}


bool AddExternalServerToServersManager (ServersManager *manager_p, ExternalServer *server_p)
{
	return manager_p -> sm_add_server_fn (manager_p, server_p);
}


ExternalServer *GetExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key)
{
	return manager_p -> sm_get_server_fn (manager_p, key);
}


ExternalServer *RemoveExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key)
{
	return manager_p -> sm_remove_server_fn (manager_p, key);
}



bool AddExternalServerFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	ServersManager *manager_p = GetServersManager ();

	if (manager_p)
		{
			const char *name_s = GetJSONString (json_p, SERVER_NAME_S);

			if (name_s)
				{
					const char *uri_s = GetJSONString (json_p, SERVER_URI_S);

					if (uri_s)
						{
							ExternalServer *server_p = NULL;
							ConnectionType ct = CT_WEB;
							const char *type_s = GetJSONString (json_p, SERVER_CONNECTION_TYPE_S);

							if (type_s)
								{
									if (strcmp (type_s, CONNECTION_RAW_S) == 0)
										{
											ct = CT_RAW;
										}
								}		/* if (type_s) */

							server_p = AllocateExternalServer (name_s, uri_s, ct);

							if (server_p)
								{
									if (AddExternalServerToServersManager (manager_p, server_p))
										{
											PrintErrors (STM_LEVEL_INFO, "Added external server %s on %s to manager with id %s", name_s, uri_s, server_p -> es_id);
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, "Failed to add external server %s on %s to manager", name_s, uri_s);
											FreeExternalServer (server_p);
										}

								}		/* if (server_p) */
							else
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to allocate external server %s on %s", name_s, uri_s);
								}

						}		/* if (uri_s) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to find uri for external server on %s", name_s);
						}

				}		/* if (name_s) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to find name for external server");
				}

		}		/* if (manager_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to get external servers manager");
		}

	return success_flag;
}


ExternalServer *AllocateExternalServer (const char *name_s, const char *uri_s, ConnectionType ct)
{
	Connection *connection_p = NULL;

	if (ct == CT_WEB)
		{
			connection_p = AllocateWebServerConnection (uri_s);
		}

	if (connection_p)
		{
			char *copied_name_s = CopyToNewString (name_s, 0, false);

			if (copied_name_s)
				{
					char *copied_uri_s = CopyToNewString (uri_s, 0, false);

					if (copied_uri_s)
						{
							ExternalServer *server_p = (ExternalServer *) AllocMemory (sizeof (ExternalServer));

							if (server_p)
								{
									server_p -> es_connection_p = connection_p;
									server_p -> es_name_s = copied_name_s;
									server_p -> es_uri_s = copied_uri_s;
									uuid_generate (server_p -> es_id);

									return server_p;
								}

							FreeCopiedString (copied_uri_s);
						}		/* if (copied_uri_s) */

					FreeCopiedString (copied_name_s);
				}		/* if (copied_name_s) */

			FreeConnection (connection_p);
		}		/* if (connection_p) */

	return NULL;
}


void FreeExternalServer (ExternalServer *server_p)
{
	FreeCopiedString (server_p -> es_uri_s);
	FreeCopiedString (server_p -> es_name_s);
	FreeConnection (server_p -> es_connection_p);

	FreeMemory (server_p);
}


json_t *MakeRemoteJSONCallToExternalServer (ExternalServer *server_p, json_t *request_p)
{
	json_t *response_p = NULL;
	const char *result_s = MakeRemoteJsonCallViaConnection (server_p -> es_connection_p, request_p);

	if (result_s)
		{
			json_error_t error;

			response_p = json_loads (result_s, 0, &error);

			if (!response_p)
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to make call to external server %s, error at %d, %d %s\n", server_p -> es_uri_s, error.line, error.column, error.source);
				}
		}

	return response_p;
}

