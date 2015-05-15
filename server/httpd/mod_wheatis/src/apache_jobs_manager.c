#include "jobs_manager.h"

#include "linked_list.h"

#include "service_job.h"
#include "mod_wheatis_config.h"


/* TODO move this to a hash table for production */
typedef struct
{
	ListItem ujn_node;
	uuid_t ujn_id;
	ServiceJob *ujn_job_p;
} UUIDJobNode;

static LinkedList s_running_services;

static WheatISConfig *s_config_p;





static UUIDJobNode *AllocateUUIDJobNode (uuid_t id, ServiceJob *service_p);

static void FreeUUIDJobNode (ListItem * const node_p);

static UUIDJobNode *GetServiceJobNode (const uuid_t job_key);


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
	s_config_p = (WheatISConfig *) data_p;
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
	apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

	if (status == APR_SUCCESS)
		{
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

			status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);
			if (status != APR_SUCCESS)
				{

				}
		}

	return success_flag;
}


ServiceJob *GetServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

	if (status == APR_SUCCESS)
		{
			UUIDJobNode *node_p = GetServiceJobNode (job_key);

			if (node_p)
				{
					job_p = node_p -> ujn_job_p;
				}

			status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);

			if (status != APR_SUCCESS)
				{

				}
		}
	else
		{

		}

	return job_p;
}


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;

	apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

	if (status == APR_SUCCESS)
		{
			UUIDJobNode *node_p = GetServiceJobNode (job_key);

			if (node_p)
				{
					job_p = node_p -> ujn_job_p;

					LinkedListRemove (&s_running_services, (ListItem * const) node_p);

					node_p -> ujn_job_p = NULL;
					FreeUUIDJobNode ((ListItem * const) node_p);

					node_p = NULL;
				}

			status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);

			if (status != APR_SUCCESS)
				{

				}
		}
	else
		{

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

