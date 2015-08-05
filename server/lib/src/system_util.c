#include "system_util.h"

#include "curl/curl.h"

#include "handler_utils.h"
#include "streams.h"
#include "jobs_manager.h"
#include "wheatis_config.h"
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
							const char *root_path_s = GetServerRootDirectory ();
							char *full_services_path_s = MakeFilename (root_path_s, "wheatis.config");

							if (full_services_path_s)
								{
									if (InitConfig (full_services_path_s))
										{
											if (InitMongoDB ())
												{
													res_flag = true;
												}

											ConnectToExternalServers ();
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, "Failed to load config file from %s", full_services_path_s);
										}
									FreeCopiedString (full_services_path_s);
								}		/* if (full_services_path_s) */

							#ifdef DRMAA_ENABLED
							if (res_flag)
								{
									res_flag = InitDrmaa ();
								}
							#endif

						}
				}
		}

	return false;
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

