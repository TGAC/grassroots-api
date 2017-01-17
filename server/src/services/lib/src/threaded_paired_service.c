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
/*
 * threaded_paired_service.c
 *
 *  Created on: 20 Jul 2016
 *      Author: tyrrells
 */


#include "threaded_paired_service.h"


void *RunRemoteServiceInThread (void *data_p)
{
	RemoteServiceData *remote_data_p = (RemoteServiceData *) data_p;

	json_t *res_p = MakeRemotePairedServiceCall (remote_data_p -> rsd_service_name_s, remote_data_p -> rsd_params_p, remote_data_p -> rsd_paired_service_p -> ps_server_uri_s, remote_data_p -> rsd_providers_p);
	return res_p;
}



int32 RunThreads (LinkedList *threads_p)
{
	return 0;
}
