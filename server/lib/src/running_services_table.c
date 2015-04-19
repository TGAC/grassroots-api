#include "running_services_table.h"


typedef struct UUIDServiceNode
{
	ServiceNode usn_node;
	uuid_t usn_id;
} UUIDServiceNode;



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


static UUIDServiceNode *AllocateUUIDServiceNode (Service *service_p);

static void FreeUUIDServiceNode (ListItem * const node_p);


static UUIDServiceNode *AllocateUUIDServiceNode (Service *service_p)
{
	UUIDServiceNode *node_p = (UUIDServiceNode *) AllocMemory (sizeof (UUIDServiceNode));

	if (node_p)
		{
			node_p -> usn_node.sn_node.ln_prev_p = NULL;
			node_p -> usn_node.sn_node.ln_next_p = NULL;

			node_p -> usn_node.sn_service_p = service_p;

			uuid_generate (node_p -> usn_id);
		}		/* if (node_p) */

	return node_p;
}


static void FreeUUIDServiceNode (ListItem * const node_p)
{
	FreeServiceNode (node_p);
}


HashTable *GetHashTableOfServiceStatuses (const uint32 initial_capacity, const uint8 load_percentage)
{
	HashTable *services_p = NULL;

	return services_p;
}


bool AddServiceToStatusTable (uuid_t user_key, Service *service_p)
{
	bool success_flag = false;
	LinkedList *services_p = NULL;

	/* Lock access to avoid race conditions */
	pthread_mutex_lock (&s_services_mutex);

	services_p = GetFromHashTable (s_running_services_p, user_key);

	if (!services_p)
		{
			services_p = AllocateLinkedList (FreeUUIDServiceNode);

			if (services_p)
				{
					if (!PutInHashTable (s_running_services_p, user_key, services_p))
						{
							FreeLinkedList (services_p);
							services_p = NULL;
						}
				}
		}

	if (services_p)
		{
			UUIDServiceNode *node_p = AllocateUUIDServiceNode (service_p);

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


void ServiceFinished (uuid_t user_key, Service *service_p, const OperationStatus status)
{
	LinkedList *services_p = NULL;

	/* Lock access to avoid race conditions */
	pthread_mutex_lock (&s_services_mutex);

	services_p = GetFromHashTable (s_running_services_p, user_key);

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


Service *GetServiceFromStatusTable (const uuid_t user_key, const uuid_t service_key)
{
	LinkedList *services_p = NULL;
	Service *service_p = NULL;

	/* Lock access to avoid race conditions */
	pthread_mutex_lock (&s_services_mutex);

	services_p = GetFromHashTable (s_running_services_p, user_key);

	if (services_p)
		{
			UUIDServiceNode *node_p = (UUIDServiceNode *) (services_p -> ll_head_p);
			bool searching_flag = (node_p != NULL);

			while (searching_flag)
				{
					if (uuid_compare (service_key, node_p -> usn_id) == 0)
						{
							service_p = node_p -> usn_node.sn_service_p;
							searching_flag = false;
						}
					else
						{
							node_p = (UUIDServiceNode *) (node_p -> usn_node.sn_node.ln_next_p);
						}
				}		/* while (searching_flag) */

		}		/* if (services_p) */

	pthread_mutex_unlock (&s_services_mutex);

	return service_p;
}

