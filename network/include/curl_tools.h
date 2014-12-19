#ifndef WHEATIS_CURL_TOOLS_H
#define WHEATIS_CURL_TOOLS_H

#include <curl/curl.h>

#include "typedefs.h"
#include "network_library.h"


#ifdef __cplusplus
	extern "C" {
#endif


WHEATIS_NETWORK_API bool SetSSLEngine (CURL *curl_p, const char *cryptograph_engine_name_s);

WHEATIS_NETWORK_API bool CallUrl (const char *url_s, const char *header_data_s, const char *cryptograph_engine_name_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_CURL_TOOLS_H */
