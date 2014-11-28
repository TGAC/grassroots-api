#include "system_util.h"

#include "curl/curl.h"

#include "handler_utils.h"
#include "streams.h"



bool InitInformationSystem ()
{
	if (InitHandlerUtil ())
		{
			if (InitDefaultOutputStream ())
				{					
					CURLcode c = curl_global_init (CURL_GLOBAL_DEFAULT);
					
					if (c == 0)
						{
							return true;
						}
				}
		}

	return false;
}


bool DestroyInformationSystem ()
{
	bool res_flag = true;
	
	FreeDefaultOutputStream ();
	DestroyHandlerUtil ();

	curl_global_cleanup ();

	return res_flag;
}
