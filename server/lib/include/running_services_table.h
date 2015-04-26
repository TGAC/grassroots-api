/*
 * running_services_table.h
 *
 *  Created on: 17 Apr 2015
 *      Author: tyrrells
 */

#ifndef SERVICES_STATUSES_TABLE_H_
#define SERVICES_STATUSES_TABLE_H_

#include "hash_table.h"
#include "wheatis_service_manager_library.h"
#include "service.h"
#include "service_job.h"

#include "uuid/uuid.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Create a HashTable where both the keys are strings and the values are services
 *
 * @param initital_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or NULL is there was an error.
 */
WHEATIS_SERVICE_MANAGER_API bool InitServicesStatusTable (void);


WHEATIS_SERVICE_MANAGER_API bool DestroyServicesStatusTable (void);


WHEATIS_SERVICE_MANAGER_API bool AddServiceJobToStatusTable (uuid_t job_key, ServiceJob *job_p);


WHEATIS_SERVICE_MANAGER_API ServiceJob *GetServiceJobFromStatusTable (const uuid_t key);


WHEATIS_SERVICE_MANAGER_API ServiceJob *RemoveServiceJobFromStatusTable (const uuid_t key);



/**
 *
 */
WHEATIS_SERVICE_MANAGER_LOCAL void ServiceJobFinished (uuid_t job_key);






#ifdef __cplusplus
}
#endif

#endif /* SERVICES_STATUSES_TABLE_H_ */
