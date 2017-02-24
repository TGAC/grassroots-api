/*
** Copyright 2014-2016 The Earlham Institute
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
/**
 * paired_samtools_service.h
 *
 *  Created on: 13 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SERVER_SRC_SERVICES_SAMTOOLS_INCLUDE_PAIRED_SAMTOOLS_SERVICE_H_
#define SERVER_SRC_SERVICES_SAMTOOLS_INCLUDE_PAIRED_SAMTOOLS_SERVICE_H_

#include "samtools_service.h"
#include "remote_service_job.h"


#ifdef __cplusplus
extern "C"
{
#endif


SAMTOOLS_SERVICE_LOCAL bool SaveRemoteSamtoolsJobDetails (RemoteServiceJob *job_p, const ServiceData *service_data_p);


SAMTOOLS_SERVICE_LOCAL bool AddPairedIndexParameters (Service *service_p, Parameter *internal_dbs_param_p, ParameterSet *internal_params_p);


SAMTOOLS_SERVICE_LOCAL char *CreateDatabaseName (const char *database_s, const char *server_s);


#ifdef __cplusplus
}
#endif



#endif /* SERVER_SRC_SERVICES_SAMTOOLS_INCLUDE_PAIRED_SAMTOOLS_SERVICE_H_ */
