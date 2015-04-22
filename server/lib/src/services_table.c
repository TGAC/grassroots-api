#include "running_services_table.h"
#include "string_utils.h"

/**
 * This hashtable will exist in a separate thread
 */

static HashTable *s_running_services_p = NULL;




HashTable *GetHashTableOfServiceStatuses (const uint32 initial_capacity, const uint8 load_percentage)
{
	HashTable *services_p = NULL;

	return services_p;
}


bool AddServiceToStatusTable (uuid_t service_key, Service *service_p)
{
	bool success_flag = false;
	Service *existing_service_p = GetServiceFromStatusTable (service_key);

	if (existing_service_p)
		{
			char buffer [UUID_BUFFER_SIZE];

			ConvertUUIDToString (service_key, buffer);
			PrintErrors (STM_LEVEL_SEVERE, "Service key %s for %s already exists", buffer, GetServiceName (service_p));
		}
	else
		{
			if (PutInHashTable (s_running_services_p, service_key, service_p))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to add %s to services table", GetServiceName (service_p));
				}
		}

	return success_flag;
}


void ServiceFinished (uuid_t service_key, const OperationStatus status)
{
	Service *service_p = (Service *) GetFromHashTable (s_running_services_p, service_key);

	if (service_p)
		{

		}
}



Service *RemoveServiceFromStatusTable (const uuid_t service_key)
{
	Service *service_p = GetServiceFromStatusTable (service_key);

	if (service_p)
		{
			RemoveFromHashTable (s_running_services_p, &service_key);
		}

	return service_p;
}


Service *GetServiceFromStatusTable (const uuid_t service_key)
{
	Service *service_p = (Service *) GetFromHashTable (s_running_services_p, &service_key);

	if (!service_p)
		{
			char buffer [UUID_BUFFER_SIZE];

			ConvertUUIDToString (service_key, buffer);
			PrintErrors (STM_LEVEL_SEVERE, "No service for key %s", buffer);
		}

	return service_p;
}

