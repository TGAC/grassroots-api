#include "running_services_table.h"
#include "string_utils.h"


static uint32 HashUUID (const void * const key_p);

static bool CompareUUIDBuckets (const void * const bucket_key_p, const void * const key_p);

static bool SaveUUIDBucket (const HashBucket * const bucket_p, FILE *out_f);

static void PrintUUIDBucket (const HashBucket * const bucket_p, OutputStream * const stream_p);

static bool FillUUIDBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p);


/*
static HashBucket *create_buckets_fn (const uint32 num_buckets);

static void (*free_bucket_fn) (HashBucket * const bucket_p);
*/


/*
 * The keys are uuid_t and the values are Service *
 */
//static HashTable *s_running_services_p = NULL;



static uint32 HashUUID (const void * const key_p)
{
	uint32 res = 0;
	const uuid_t id = (const uuid_t) key_p;
	const uint8 *c_p = id;

	/* uuid_t is a 16 character array */
	for (i = 16; i > 0; -- i, ++ c_p)
		{
			res = (res << 1) ^ *c_p;
		}

	return res;
}


static bool FillUUIDBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p)
{
	bool success_flag = false;
	const uuid_t id = (const uuid_t) (bucket_p -> hb_key_p);
	const Service *service_p = (const Service *) (bucket_p -> hb_value_p);

	char *dest_p = NULL;

	FreeHashBucket (bucket_p);

	GenerateUUID()

	switch (bucket_p -> hb_owns_key)
		{
			case MF_DEEP_COPY:
				const uuid_t key_
				dest_p = CopyToNewString ((const char * const) key_p, 0, FALSE);

				if (dest_p)
					{
						bucket_p -> hb_key_p = dest_p;
					}
				else
					{
						success = FALSE;
					}
				break;

			case MF_SHALLOW_COPY:
			case MF_SHADOW_USE:
			case MF_ALREADY_FREED:
				bucket_p -> hb_key_p = key_p;
				break;
		}

	if (success)
		{
			switch (bucket_p -> hb_owns_value)
				{
					case MF_DEEP_COPY:
						dest_p = CopyToNewString ((const char * const) value_p, 0, FALSE);

						if (dest_p)
							{
								bucket_p -> hb_value_p = dest_p;
							}
						else
							{
								success = FALSE;
							}
						break;

					case MF_SHALLOW_COPY:
					case MF_SHADOW_USE:
					case MF_ALREADY_FREED:
						bucket_p -> hb_value_p = value_p;
						break;
				}
		}

	return success;




	return success_flag;
}


static bool CompareUUIDBuckets (const void * const bucket_0_p, const void * const bucket_1_p)
{
	bool equal_flag = false;

	if (bucket_0_p && bucket_1_p)
		{
			const uuid_t id0 = (const uuid_t) bucket_0_p;
			const uuid_t id1 = (const uuid_t) bucket_1_p;

			equal_flag = (uuid_compare (id0, id1) == 0);
		}

	return FALSE;


	return equal_flag;
}


static bool SaveUUIDBucket (const HashBucket * const bucket_p, FILE *out_f)
{
	bool success_flag = false;
	const uuid_t id = (const uuid_t) (bucket_p -> hb_key_p);
	const Service *service_p = (const Service *) (bucket_p -> hb_value_p);
	char *uuid_s = GetUUIDAsString (id);

	if (uuid_s)
		{
			char *service_name_s = GetServiceName (service_p);

			success_flag = (fprintf (out_f, "%s = %s\n", uuid_s, service_name_s) > 0);

			FreeUUIDString (uuid_s);
		}

	return success_flag;
}


static void PrintUUIDBucket (const HashBucket * const bucket_p, OutputStream * const stream_p)
{
	const uuid_t id = (const uuid_t) (bucket_p -> hb_key_p);
	const Service *service_p = (const Service *) (bucket_p -> hb_value_p);
	char *uuid_s = GetUUIDAsString (id);

	if (uuid_s)
		{
			char *service_name_s = GetServiceName (service_p);

			PrintToOutputStream (stream_p, "%s = %s\n", uuid_s, service_name_s);

			FreeUUIDString (uuid_s);
		}
}


bool InitJobsManager (void)
{
	if (!s_running_services_p)
		{
			s_running_services_p = GetHashTableOfServiceStatuses ();
		}

	return (s_running_services_p != NULL);
}


bool DestroyJobsManager (void)
{
	if (s_running_services_p)
		{
			FreeHashTable (s_running_services_p);
			s_running_services_p = NULL;
		}

	return true;
}



HashTable *GetHashTableOfServiceStatuses (const uint32 initial_capacity, const uint8 load_percentage)
{
	HashTable *services_p = AllocateHashTable (1024, 75,
		uint32 (*hash_fn) (const void * const key_p),
		HashBucket *(*create_buckets_fn) (const uint32 num_buckets),
		void (*free_bucket_fn) (HashBucket * const bucket_p),
		bool (*fill_bucket_fn) (HashBucket * const bucket_p, const void * const key_p, const void * const value_p),
		bool (*compare_keys_fn) (const void * const bucket_key_p, const void * const key_p),
		void (*print_bucket_fn) (const HashBucket * const bucket_p, OutputStream * const stream_p),
		bool (*save_bucket_fn) (const HashBucket * const bucket_p, FILE *out_f));

	return services_p;
}





bool AddServiceJobToStatusTable (uuid_t service_key, Service *service_p)
{
	bool success_flag = false;
	Service *existing_service_p = GetServiceJobFromJobsManager (service_key);

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


void ServiceJobFinished (uuid_t service_key, const OperationStatus status)
{
	Service *service_p = (Service *) GetFromHashTable (s_running_services_p, service_key);

	if (service_p)
		{

		}
}



Service *RemoveServiceJobFromJobsManager (const uuid_t service_key)
{
	Service *service_p = GetServiceJobFromJobsManager (service_key);

	if (service_p)
		{
			RemoveFromHashTable (s_running_services_p, &service_key);
		}

	return service_p;
}


Service *GetServiceJobFromJobsManager (const uuid_t service_key)
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

