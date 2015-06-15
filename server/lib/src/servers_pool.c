#include "servers_pool.h"


void InitServersManager (ServersManager *manager_p,
                      bool (*add_server_fn) (ServersManager *manager_p, uuid_t key, ExternalServer *job_p),
											ExternalServer *(*get_server_fn)  (ServersManager *manager_p, const uuid_t key),
											ExternalServer *(*remove_server_fn) (ServersManager *manager_p, const uuid_t key))
{
	manager_p -> sm_add_server_fn = add_server_fn;
	manager_p -> sm_get_server_fn = get_server_fn;
	manager_p -> sm_remove_server_fn = remove_server_fn;
}


bool AddExternalServerToServersManager (ServersManager *manager_p, uuid_t key, ExternalServer *server_p)
{
	return manager_p -> sm_add_server_fn (manager_p, key, server_p);
}


ExternalServer *GetExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key)
{
	return manager_p -> sm_get_server_fn (manager_p, key);
}


ExternalServer *RemoveExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key)
{
	return manager_p -> sm_remove_server_fn (manager_p, key);
}

ExternalServer *AllocateExternalServer (char *uri_s, ConnectionType ct)
{
}

void FreeExternalServer (ExternalServer *server_p)
{
}

json_t *MakeRemoteJSONCallToExternalServer (ExternalServer *server_p, json_t *request_p)
{
}

