#ifndef WHEATIS_CURL_TOOLS_H
#define WHEATIS_CURL_TOOLS_H

#include <curl/curl.h>

#include "typedefs.h"
#include "network_library.h"
#include "byte_buffer.h"


typedef struct CurlTool
{
	CURL *ct_curl_p;
	ByteBuffer *ct_buffer_p;
	struct curl_httppost *ct_form_p;
	struct curl_httppost *ct_last_field_p;
	struct curl_slist *ct_headers_list_p;
} CurlTool;




#ifdef __cplusplus
	extern "C" {
#endif


WHEATIS_NETWORK_API CurlTool *AllocateCurlTool (void);

WHEATIS_NETWORK_API void FreeCurlTool (CurlTool *curl_p);


/**
 * Allocate a new CURL object and add optionally add a 
 * write to buffer callback.
 * 
 * @param buffer_p If this is not NULL, then set the curl object 
 * to write its reponse to this buffer. This can be NULL <code>NULL<code>.
 * @see AddCurlCallback
 * @return The CURL object or <code>NULL<code> upon error.
 */
WHEATIS_NETWORK_API CURL *GetCurl (ByteBuffer *buffer_p);


/**
 * Free a CURL object.
 * 
 * @param curl_p The CURL object to free.
 */
WHEATIS_NETWORK_API void FreeCurl (CURL *curl_p);


WHEATIS_NETWORK_API bool SetSSLEngine (CURL *curl_p, const char *cryptograph_engine_name_s);

WHEATIS_NETWORK_API bool CallSecureUrl (const char *url_s, const char *header_data_s, const char *ca_cert_name_s, const char * const cert_name_s, const char *key_name_s, const bool verify_certs, ByteBuffer *buffer_p);

/**
 * Add a callback to write the HTTP response for this CURL object to 
 * the given buffer.
 * 
 * @return The CURL object to add the callback for.
 * @param buffer_p The ByteBuffer which will store the CURL object's repsonse.
 */
WHEATIS_NETWORK_API bool AddCurlCallback (CURL *curl_p, ByteBuffer *buffer_p);


WHEATIS_NETWORK_API bool SetUriForCurlTool (CurlTool *tool_p, const char * const uri_s);


WHEATIS_NETWORK_API CURLcode RunCurlTool (CurlTool *tool_p);


WHEATIS_NETWORK_API const char *GetCurlToolData (const CurlTool * const tool_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_CURL_TOOLS_H */
