/**
 * @addtogroup Network
 * @{
 */

#ifndef WHEATIS_CURL_TOOLS_H
#define WHEATIS_CURL_TOOLS_H

#include <curl/curl.h>


#include "typedefs.h"
#include "network_library.h"
#include "byte_buffer.h"
#include "selector.hpp"

/**
 * @brief A tool for making http(s) requests and responses.
 *
 * A datatype that uses CURL to perform http(s) requests
 * and responses.
 */
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


/**
 * Allocate a CurlTool
 *
 * @return A newly-allocated CurlTool or <code>NULL</code> upon error.
 * @memberof CurlTool.
 * @see FreeCurlTool
 */
WHEATIS_NETWORK_API CurlTool *AllocateCurlTool (void);


/**
 * Free a CurlTool
 *
 * @param curl_p The CurlTool to free.
 * @memberof CurlTool.
 */
WHEATIS_NETWORK_API void FreeCurlTool (CurlTool *curl_p);


/**
 * Allocate a new CURL object and add optionally add a
 * write to buffer callback.
 *
 * @param buffer_p If this is not NULL, then set the curl object
 * to write its response to this buffer. This can be NULL <code>NULL</code>.
 * @see AddCurlCallback
 * @return The CURL object or <code>NULL</code> upon error.
 */
WHEATIS_NETWORK_API CURL *GetCurl (ByteBuffer *buffer_p);


/**
 * Free a CURL object.
 *
 * @param curl_p The CURL object to free.
 */
WHEATIS_NETWORK_API void FreeCurl (CURL *curl_p);

/**
 * @brief Set the cryptographic engine to use.
 *
 * @param curl_p The CURL instance to set the SSL engine for.
 * @param cryptograph_engine_name_s The name of the cryptographic engine
 * to use. For the valid names see the CURL documentation.
 * @return <code>true</code> if the SSL engine name was set successfully,
 * <code>false</code> otherwise.
 */
WHEATIS_NETWORK_API bool SetSSLEngine (CURL *curl_p, const char *cryptograph_engine_name_s);


/**
 * Call a secure web address
 *
 * @param url_s The address to get the data from.
 * @param header_data_s The value to use for CURLOPT_CAINFO. See the Curl documentation.
 * @param ca_cert_name_s The value to use for CURLOPT_CAINFO. See the Curl documentation.
 * @param cert_name_s The value to use for CURLOPT_SSLCERT. See the Curl documentation.
 * @param key_name_s The value to use for CURLOPT_SSLKEY. See the Curl documentation.
 * @param verify_certs If this is true then CURLOPT_SSL_VERIFYPEER will be set. See the Curl documentation.
 * @param buffer_p Where the data from the address will be written to
 * @return <code>true</code> if the call was successful, <code>false</code> otherwise.
 */
WHEATIS_NETWORK_API bool CallSecureUrl (const char *url_s, const char *header_data_s, const char *ca_cert_name_s, const char * const cert_name_s, const char *key_name_s, const bool verify_certs, ByteBuffer *buffer_p);


/**
 * Add a callback to write the HTTP response for this CURL object to
 * the given buffer.
 *
 * @return The CURL object to add the callback for.
 * @param buffer_p The ByteBuffer which will store the CURL object's response.
 */
WHEATIS_NETWORK_API bool AddCurlCallback (CURL *curl_p, ByteBuffer *buffer_p);


/**
 * Set the URI that the CurlTool will try to get the html data from.
 *
 * @param tool_p The CurlTool to update.
 * @param uri_s The URI to set the CurlTool for.
 * @return <code>true</code> if the CurlTool was updated successfully,
 * <code>false</code> otherwise.
 * @memberof CurlTool
 */
WHEATIS_NETWORK_API bool SetUriForCurlTool (CurlTool *tool_p, const char * const uri_s);


/**
 * @brief Run a CurlTool.
 * This will get the CurlTool to get all of the data from its given URI.
 * @return CURLE_OK if successful or an error code upon failure.
 */
WHEATIS_NETWORK_API CURLcode RunCurlTool (CurlTool *tool_p);


/**
 * @brief Set up a CurlTool to do a JSON post when it is run.
 *
 * @param tool_p The CurlTool to update.
 * @return <code>true</code> if the CurlTool was updated successfully,
 * <code>false</code> otherwise.
 * @memberof CurlTool
 */
WHEATIS_NETWORK_API bool SetCurlToolForJSONPost (CurlTool *tool_p);


/**
 * @brief Get the downloaded data from a CurlTool.
 * If the CurlTool has been run successfully, this will get a read-only
 * version of the downloaded data. <code>RunCurlTool</code> must have been
 * run prior to this.
 * @return The downloaded data or <code>NULL</code> upon error.
 * @see RunCurlTool
 * @memberof CurlTool
 */
WHEATIS_NETWORK_API const char *GetCurlToolData (const CurlTool * const tool_p);


/**
 * @brief Run a CurlTool and select a subset of the data.
 * This will set up a CurlTool to a given URI and download the data. If successful, it
 * will then extract a set of HtmlLinks using the given selector.
 * @param tool_p The CurlTool to use.
 * @param uri_s The URI for the CurlTool to download from.
 * @param selector_s The CSS selector to use to extract the HtmlLinkArray with.
 * @return A newly-allocated HtmlLinkArray if successful or <code>NULL</code>
 * upon error.
 * @memberof CurlTool
 * @see GetMatchingLinks
 */
WHEATIS_NETWORK_API HtmlLinkArray *GetLinks (CurlTool *tool_p, const char * const uri_s, const char * const link_selector_s, const char * const title_selector_s);

/**
 * @brief Send a JSON request using a CurlTool.
 * @param tool_p The CurlTool to use.
 * @param req_p The json data to send.
 * @return <code>true</code> if the CurlTool made the request and received a response
 * successfully. The resultant data can be retrieved using <code>GetCurlToolData</code>.
 * If the call was unsuccessful, then <code>false</code> will be returned.
 * @see GetCurlToolData
 * @memberof CurlTool
 */
WHEATIS_NETWORK_API bool MakeRemoteJSONCallFromCurlTool (CurlTool *tool_p, json_t *req_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_CURL_TOOLS_H */

/* @} */
