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
 * audit.c
 *
 *  Created on: 21 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#include "audit.h"

#include "connection.h"
#include "service_config.h"
#include "json_tools.h"
#include "grassroots_config.h"



bool LogServiceJob (ServiceJob *job_p)
{
	bool success_flag = false;
	const char *uri_s = GetJobLoggingURI ();

	if (uri_s)
		{
			Connection *connection_p = AllocateWebServerConnection (uri_s);

			if (connection_p)
				{
					const SchemaVersion *sv_p = GetSchemaVersion ();
					json_t *req_p = GetInitialisedMessage (sv_p);

					if (req_p)
						{
							json_t *job_json_p = GetServiceJobAsJSON (job_p);

							if (job_json_p)
								{
									if (json_object_set_new (req_p, "job", job_json_p) == 0)
										{
											const char *response_s = MakeRemoteJsonCallViaConnection (connection_p, req_p);

											success_flag = true;
										}		/* if (json_object_set_new (req_p, "job", job_json_p) == 0) */
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add service job JSON to logging object");
											json_decref (job_json_p);
										}

								}		/* if (job_json_p) */
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "GetServiceJobAsJSON failed for logging service job");
								}

							json_decref (req_p);
						}		/* if (req_p) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "GetInitialisedMessage failed for logging service job");
						}

					FreeConnection (connection_p);
				}		/* if (connection_p) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate Web Connection for logging service job");
				}

		}		/* if (uri_s) */
	else
		{
			/* no logging uri, so nothing to do */
			success_flag = true;
		}

	return success_flag;
}
