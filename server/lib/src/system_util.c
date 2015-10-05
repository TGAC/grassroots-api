/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include "system_util.h"

#include "curl/curl.h"

#include "handler_utils.h"
#include "streams.h"
#include "jobs_manager.h"
#include "grassroots_config.h"
#include "string_utils.h"
#include "service_config.h"
#include "mongodb_util.h"

#ifdef DRMAA_ENABLED
#include "drmaa_util.h"
#endif


bool InitInformationSystem ()
{
	bool res_flag = false;

	if (InitHandlerUtil ())
		{
			if (InitDefaultOutputStream ())
				{
					CURLcode c = curl_global_init (CURL_GLOBAL_DEFAULT);

					if (c == 0)
						{
							if (InitConfig ())
								{
									if (InitMongoDB ())
										{
											res_flag = true;
										}

									ConnectToExternalServers ();
								}

							#ifdef DRMAA_ENABLED
							if (res_flag)
								{
									res_flag = InitDrmaa ();
								}
							#endif

						}
				}
		}

	return res_flag;
}


bool DestroyInformationSystem ()
{
	bool res_flag = true;

	#ifdef DRMAA_ENABLED
	ExitDrmaa ();
	#endif
	
	FreeDefaultOutputStream ();
	DestroyHandlerUtil ();
	ExitMongoDB ();
	curl_global_cleanup ();

	return res_flag;
}

