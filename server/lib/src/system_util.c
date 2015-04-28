#include "system_util.h"

#include "curl/curl.h"

#include "handler_utils.h"
#include "streams.h"
#include "running_services_table.h"

#ifdef DRMAA_ENABLED
#include "drmaa_util.h"
#endif

bool InitInformationSystem ()
{
	if (InitHandlerUtil ())
		{
			if (InitDefaultOutputStream ())
				{
					if (InitServicesStatusTable ())
						{
							CURLcode c = curl_global_init (CURL_GLOBAL_DEFAULT);

							if (c == 0)
								{
									bool res_flag = true;

									#ifdef DRMAA_ENABLED
									res_flag = InitDrmaa ();
									#endif

									return res_flag;
								}
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
	DestroyServicesStatusTable ();
	curl_global_cleanup ();

	return res_flag;
}

