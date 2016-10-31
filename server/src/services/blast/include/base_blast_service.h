/*
 * base_blast_service.h
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_BASE_BLAST_SERVICE_H_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_BASE_BLAST_SERVICE_H_


#include "blast_app_parameters.h"
#include "blast_service_api.h"
#include "blast_service.h"
#include "blast_service_job.h"
#include "parameter_set.h"
#include "temp_file.hpp"



#ifdef __cplusplus
extern "C"
{
#endif

BLAST_SERVICE_LOCAL ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p, BlastAppParameters *app_params_p);

BLAST_SERVICE_LOCAL bool GetBlastServiceConfig (BlastServiceData *data_p);

BLAST_SERVICE_LOCAL BlastServiceData *AllocateBlastServiceData (Service *blast_service_p, DatabaseType database_type);

BLAST_SERVICE_LOCAL void FreeBlastServiceData (BlastServiceData *data_p);

BLAST_SERVICE_LOCAL bool CloseBlastService (Service *service_p);

BLAST_SERVICE_LOCAL ParameterSet *IsResourceForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

BLAST_SERVICE_LOCAL bool AddBaseBlastServiceParameters (Service *blast_service_p, ParameterSet *param_set_p, const DatabaseType db_type, bool (*add_additional_params_fn) (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p));

BLAST_SERVICE_LOCAL void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);


BLAST_SERVICE_LOCAL bool DetermineBlastResult (Service *service_p, BlastServiceJob *job_p);

BLAST_SERVICE_LOCAL OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id);

BLAST_SERVICE_LOCAL TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id);

BLAST_SERVICE_LOCAL char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code);

BLAST_SERVICE_LOCAL char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p);

BLAST_SERVICE_LOCAL void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, BlastServiceData *data_p);


BLAST_SERVICE_LOCAL ServiceJob *BuildBlastServiceJob (struct Service *service_p, const json_t *service_job_json_p);

BLAST_SERVICE_LOCAL json_t *BuildBlastServiceJobJSON (Service *service_p, const ServiceJob *service_job_p);


BLAST_SERVICE_LOCAL void CustomiseBlastServiceJob (Service *service_p, ServiceJob *job_p);

BLAST_SERVICE_LOCAL char *GetValueFromBlastServiceJobOutput (Service *service_p, ServiceJob *job_p, const char * const input_s);


#ifdef __cplusplus
}
#endif




#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_BASE_BLAST_SERVICE_H_ */
