#include "running_services_table.h"

#include "linked_list.h"



/* TODO move this to a hash table for production */
typedef struct
{
	ListItem usn_node;
	uuid_t usn_id;
	Service *usn_service_p;
} UUIDServiceNode;

static LinkedList s_running_services;


static UUIDServiceNode *AllocateUUIDServiceNode (uuid_t id, Service *service_p);

static void FreeUUIDServiceNode (ListItem * const node_p);


static UUIDServiceNode *AllocateUUIDServiceNode (uuid_t id, Service *service_p)
{
	UUIDServiceNode *node_p = (UUIDServiceNode *) AllocMemory (sizeof (UUIDServiceNode));

	if (node_p)
		{
			node_p -> usn_node.ln_prev_p = NULL;
			node_p -> usn_node.ln_next_p = NULL;

			uuid_copy (node_p -> usn_id, id);
			node_p -> usn_service_p = service_p;
		}

	return node_p;
}


static void FreeUUIDServiceNode (ListItem * const node_p)
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
bool InitServicesStatusTable (void)
{
	InitLinkedList (&s_running_services);

	return true;
}


bool DestroyServicesStatusTable (void)
{
	ClearLinkedList (&s_running_services);

	return true;
}


bool AddServiceToStatusTable (uuid_t service_key, Service *service_p)
{
	UUIDServiceNode *node_p = AllocateUUIDServiceNode (service_key, service_p);

	if (node_p)
		{
			LinkedListAddTail (&s_running_services, (ListItem * const) node_p);
		}

	return (node_p != NULL);
}


Service *GetServiceFromStatusTable (const uuid_t service_key)
{
	UUIDServiceNode *node_p = (UUIDServiceNode *) s_running_services.ll_head_p;

	while (node_p)
		{
			if (uuid_compare (node_p -> usn_id, service_key) == 0)
				{
					return (node_p -> usn_service_p);
				}

			node_p = (UUIDServiceNode *) (node_p -> usn_node.ln_next_p);
		}

	return NULL;
}


Service *RemoveServiceFromStatusTable (const uuid_t service_key)
{
	Service *service_p = NULL;
	UUIDServiceNode *node_p = (UUIDServiceNode *) s_running_services.ll_head_p;

	while (node_p)
		{
			if (uuid_compare (node_p -> usn_id, service_key) == 0)
				{
					service_p = node_p -> usn_service_p;

					LinkedListRemove (&s_running_services, (ListItem * const) node_p);

					node_p -> usn_service_p = NULL;
					FreeUUIDServiceNode ((ListItem * const) node_p);

					node_p = NULL;
				}
			else
				{
					node_p = (UUIDServiceNode *) (node_p -> usn_node.ln_next_p);
				}
		}


	return service_p;
}



void ServiceFinished (uuid_t service_key, const OperationStatus status)
{
	Service *service_p = RemoveServiceFromStatusTable (service_key);

	if (service_p)
		{
			SetCurrentServiceStatus (service_p, service_key, status);
		}
}



