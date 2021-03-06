/*
** Copyright 2014-2016 The Earlham Institute
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

#include "jobs_manager.h"

#include "linked_list.h"

#include "servers_pool.h"


typedef struct ExternalServersManager
{
	ServersManager esm_base_manager;
	LinkedList esm_servers;
} ExternalServersManager;



static ExternalServersManager *s_manager_p = NULL;


static ExternalServerNode *GetExternalServerNode (ExternalServersManager *manager_p, const char * const server_uri_s);

static ExternalServer *GetExternalServerFromExternalServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);

static ExternalServer *RemoveExternalServerFromExternalServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);

static bool AddExternalServerToExternalServersManager (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn);

static LinkedList *GetAllExternalServersFromExternalServersManager (ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn);

static bool FreeExternalServersManager (ServersManager *manager_p);


ServersManager *GetServersManager (void)
{
	if (!s_manager_p)
		{
			s_manager_p = (ExternalServersManager *) AllocMemory (sizeof (ExternalServersManager));

			InitServersManager (& (s_manager_p -> esm_base_manager), AddExternalServerToExternalServersManager, GetExternalServerFromExternalServersManager, RemoveExternalServerFromExternalServersManager, GetAllExternalServersFromExternalServersManager, FreeExternalServersManager);
			InitLinkedList (& (s_manager_p -> esm_servers));
		}

	return (& (s_manager_p -> esm_base_manager));
}


bool DestroyServersManager ()
{
	ClearLinkedList (& (s_manager_p -> esm_servers));
	FreeMemory (s_manager_p);

	return true;
}




static bool AddExternalServerToExternalServersManager (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser UNUSED_PARAM (serialise_fn))
{
	bool success_flag = false;
	ExternalServersManager *external_servers_manager_p = (ExternalServersManager *) manager_p;
	ExternalServerNode *node_p = GetExternalServerNode (external_servers_manager_p, server_p -> es_uri_s);

	if (node_p)
		{
			success_flag = (node_p -> esn_server_p == server_p);
		}
	else
		{
			node_p = AllocateExternalServerNode (server_p, MF_SHADOW_USE);

			if (node_p)
				{
					LinkedListAddTail (& (external_servers_manager_p -> esm_servers), (ListItem * const) node_p);
					success_flag = true;
				}
		}

	return success_flag;
}


static ExternalServerNode *GetExternalServerNode (ExternalServersManager *manager_p, const char * const server_uri_s)
{
	ExternalServerNode *node_p = (ExternalServerNode *) manager_p -> esm_servers.ll_head_p;

	while (node_p)
		{
			if (strcmp (node_p -> esn_server_p -> es_uri_s, server_uri_s) == 0)
				{
					return node_p;
				}

			node_p = (ExternalServerNode *) (node_p -> esn_node.ln_next_p);
		}

	return NULL;
}


static ExternalServer *GetExternalServerFromExternalServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser UNUSED_PARAM (deserialise_fn))
{
	ExternalServersManager *external_servers_manager_p = (ExternalServersManager *) manager_p;
	ExternalServerNode *node_p = GetExternalServerNode (external_servers_manager_p, server_uri_s);

	return (node_p ? node_p -> esn_server_p : NULL);
}


static ExternalServer *RemoveExternalServerFromExternalServersManager (ServersManager *manager_p, const char * const server_uri_s,  ExternalServerDeserialiser UNUSED_PARAM (deserialise_fn))
{
	ExternalServersManager *external_servers_manager_p = (ExternalServersManager *) manager_p;
	ExternalServer *server_p = NULL;
	ExternalServerNode *node_p = GetExternalServerNode (external_servers_manager_p, server_uri_s);

	if (node_p)
		{
			server_p = node_p -> esn_server_p;

			LinkedListRemove (& (external_servers_manager_p -> esm_servers), (ListItem * const) node_p);

			node_p -> esn_server_p = NULL;
			FreeExternalServerNode ((ListItem * const) node_p);
		}

	return server_p;
}



static LinkedList *GetAllExternalServersFromExternalServersManager (ServersManager *manager_p, ExternalServerDeserialiser UNUSED_PARAM (deserialise_fn))
{
	ExternalServersManager *external_servers_manager_p = (ExternalServersManager *) manager_p;
	return & (external_servers_manager_p -> esm_servers);
}


static bool FreeExternalServersManager (ServersManager *manager_p)
{
	ExternalServersManager *external_servers_manager_p = (ExternalServersManager *) manager_p;

	FreeLinkedList (& (external_servers_manager_p -> esm_servers));
	FreeMemory (external_servers_manager_p);

	return true;
}

