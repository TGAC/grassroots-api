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

static LinkedList s_running_services;


static UUIDJobNode *AllocateUUIDJobNode (uuid_t id, ServiceJob *service_p);

static void FreeUUIDJobNode (ListItem * const node_p);

static UUIDJobNode *GetServiceJobNode (const uuid_t job_key);



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



/**
 * Create a HashTable where both the keys are strings and the values are services
 *
 * @param initital_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or NULL is there was an error.
 */
bool InitJobsManager (void *data_p)
{
	InitLinkedList (&s_running_services);

	return true;
}


bool DestroyJobsManager (void *data_p)
{
	ClearLinkedList (&s_running_services);

	return true;
}


bool AddServiceJobToJobsManager (uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;
	UUIDJobNode *node_p = GetServiceJobNode (job_key);

	if (node_p)
		{
			success_flag = (node_p -> ujn_job_p == job_p);
		}
	else
		{
			UUIDJobNode *node_p = AllocateUUIDJobNode (job_key, job_p);

			if (node_p)
				{
					LinkedListAddTail (&s_running_services, (ListItem * const) node_p);
					success_flag = true;
				}
		}

	return success_flag;
}


static UUIDJobNode *GetServiceJobNode (const uuid_t job_key)
{
	UUIDJobNode *node_p = (UUIDJobNode *) s_running_services.ll_head_p;

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


ServiceJob *GetServiceJobFromJobsManager (const uuid_t job_key)
{
	UUIDJobNode *node_p = GetServiceJobNode (job_key);

	return (node_p ? node_p -> ujn_job_p : NULL);
}


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	UUIDJobNode *node_p = (UUIDJobNode *) s_running_services.ll_head_p;

	while (node_p)
		{
			if (uuid_compare (node_p -> ujn_id, job_key) == 0)
				{
					job_p = node_p -> ujn_job_p;

					LinkedListRemove (&s_running_services, (ListItem * const) node_p);

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



void ServiceJobFinished (uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromJobsManager (job_key);

	if (job_p)
		{
		}
}



