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
#include "jobs_manager.h"

#include "linked_list.h"

#include "service_job.h"

/* TODO move this to a hash table for production */
typedef struct
{
	ListItem ujn_node;
	uuid_t ujn_id;
	ServiceJob *ujn_job_p;
} UUIDJobNode;


typedef struct ServicesListManager
{
	JobsManager slm_base_manager;
	LinkedList slm_running_services;
} ServicesListManager;



static ServicesListManager *s_manager_p = NULL;


static UUIDJobNode *AllocateUUIDJobNode (uuid_t id, ServiceJob *service_p);

static void FreeUUIDJobNode (ListItem * const node_p);

static UUIDJobNode *GetServiceJobNode (const uuid_t job_key);


static ServiceJob *GetServiceJobFromServicesListManager (JobsManager *manager_p, const uuid_t job_key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));

static ServiceJob *RemoveServiceJobFromServicesListManager (JobsManager *manager_p, const uuid_t job_key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));

static bool AddServiceJobToServicesListManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p, unsigned char *(*serialise_fn) (ServiceJob *job_p, uint32 *length_p));


static UUIDJobNode *AllocateUUIDJobNode (uuid_t id, ServiceJob *job_p)
{
	UUIDJobNode *node_p = (UUIDJobNode *) AllocMemory (sizeof (UUIDJobNode));

	if (node_p)
		{
			node_p -> ujn_node.ln_prev_p = NULL;
			node_p -> ujn_node.ln_next_p = NULL;

			uuid_copy (node_p -> ujn_id, id);
			node_p -> ujn_job_p = job_p;
		}

	return node_p;
}


static void FreeUUIDJobNode (ListItem * const node_p)
{
	FreeMemory (node_p);
}


JobsManager *GetJobsManager (void)
{
	if (!s_manager_p)
		{
			s_manager_p = (ServicesListManager *) AllocMemory (sizeof (ServicesListManager));

			InitJobsManager (& (s_manager_p -> slm_base_manager), AddServiceJobToServicesListManager, GetServiceJobFromServicesListManager, RemoveServiceJobFromServicesListManager);
			InitLinkedList (& (s_manager_p -> slm_running_services));
		}

	return (& (s_manager_p -> slm_base_manager));
}


bool DestroyJobsManager ()
{
	ClearLinkedList (& (s_manager_p -> slm_running_services));
	FreeMemory (s_manager_p);

	return true;
}


static bool AddServiceJobToServicesListManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p, unsigned char *(*serialise_fn) (ServiceJob *job_p, uint32 *length_p))
{
	bool success_flag = false;
	UUIDJobNode *node_p = GetServiceJobNode (job_key);

	if (node_p)
		{
			success_flag = (node_p -> ujn_job_p == job_p);
		}
	else
		{
			node_p = AllocateUUIDJobNode (job_key, job_p);

			if (node_p)
				{
					LinkedListAddTail (& (s_manager_p -> slm_running_services), (ListItem * const) node_p);
					success_flag = true;
				}
		}

	return success_flag;
}


static UUIDJobNode *GetServiceJobNode (const uuid_t job_key)
{
	UUIDJobNode *node_p = (UUIDJobNode *) s_manager_p -> slm_running_services.ll_head_p;

	while (node_p)
		{
			if (uuid_compare (node_p -> ujn_id, job_key) == 0)
				{
					return node_p;
				}

			node_p = (UUIDJobNode *) (node_p -> ujn_node.ln_next_p);
		}

	return NULL;
}


static ServiceJob *GetServiceJobFromServicesListManager (JobsManager *manager_p, const uuid_t job_key, ServiceJob *(*deserialise_fn) (unsigned char *data_p))
{
	UUIDJobNode *node_p = GetServiceJobNode (job_key);

	return (node_p ? node_p -> ujn_job_p : NULL);
}


static ServiceJob *RemoveServiceJobFromServicesListManager (JobsManager *manager_p, const uuid_t job_key, ServiceJob *(*deserialise_fn) (unsigned char *data_p))
{
	ServiceJob *job_p = NULL;
	UUIDJobNode *node_p = (UUIDJobNode *) s_manager_p -> slm_running_services.ll_head_p;

	while (node_p)
		{
			if (uuid_compare (node_p -> ujn_id, job_key) == 0)
				{
					job_p = node_p -> ujn_job_p;

					LinkedListRemove (& (s_manager_p -> slm_running_services), (ListItem * const) node_p);

					node_p -> ujn_job_p = NULL;
					FreeUUIDJobNode ((ListItem * const) node_p);

					node_p = NULL;
				}
			else
				{
					node_p = (UUIDJobNode *) (node_p -> ujn_node.ln_next_p);
				}
		}


	return job_p;
}



void ServiceJobFinished (JobsManager *manager_p, uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromServicesListManager (& (s_manager_p -> slm_base_manager), job_key, NULL);

	if (job_p)
		{
		}
}



