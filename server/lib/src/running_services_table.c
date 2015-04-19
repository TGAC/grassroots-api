#include "running_services_table.h"


/**
 * This hashtable will exist in a separate thread
 */

static HashTable *s_running_services_p = NULL;

/**
 * Multiple processes could potentially be modifying our
 * services table so we need to moderate its access by
 * using a mutex.
 */
static pthread_mutex_t s_services_mutex;

HashTable *GetHashTableOfServiceStatuses (const uint32 initial_capacity, const uint8 load_percentage)
{
	HashTable *services_p = NULL;

	return services_p;
}


bool AddServiceToStatusTable (uuid key, Service *service_p)
{
	bool success_flag = false;
	LinkedList *services_p = NULL;

	/* Lock access to avoid race conditions */
	pthread_mutex_lock (&s_services_mutex);

	services_p = GetFromHashTable (s_running_services_p, key);

	if (!services_p)
		{
			services_p = AllocateLinkedList (FreeServiceNode);
		}

	if (services_p)
		{
			ServiceNode *node_p = AllocateServiceNode (service_p);

			if (node_p)
				{
					LinkedListAddTail (services_p, node_p);
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to allocate status node for %s", GetServiceName (service_p));
				}

		}		/* if (services_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to allocate services list for status %s", key);

		}

	pthread_mutex_unlock (&s_services_mutex);
}


void ServiceFinished (uuid key, Service *service_p, const OperationStatus status)
{
	LinkedList *services_p = NULL;

	/* Lock access to avoid race conditions */
	pthread_mutex_lock (&s_services_mutex);

	services_p = GetFromHashTable (s_running_services_p, key);

	if (services_p)
		{
			ServiceNode *node_p = (ServiceNode *) (services_p -> ll_head_p);
			bool searching_flag = (node_p != NULL);

			while (searching_flag)
				{
					Service *stored_service_p = node_p -> sn_service_p;

					if (service_p == node_p -> sn_service_p)
						{
							searching_flag = false;
						}
					else
						{
							node_p = (ServiceNode *) (node_p -> sn_node.ln_next_p);
							searching_flag = (node_p != NULL);
						}

				}		/* while (searching_flag) */

			if (node_p)
				{
					LinkedListRemove (services_p, node_p);

					service_p -> se_data_p -> sd_status = status;
					node_p -> sn_service_p = NULL;
					FreeServiceNode (node_p);
				}

		}		/* if (services_p) */


	pthread_mutex_unlock (&s_services_mutex);
}
