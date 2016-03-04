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
/**
 * @addtogroup Network
 * @{
 */

#ifndef GRASSROOTS_CURL_TOOLS_H
#define GRASSROOTS_CURL_TOOLS_H

#include <curl/curl.h>

#include "jansson.h"

#include "typedefs.h"
#include "network_library.h"
#include "byte_buffer.h"

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
 * Allocate a CurlTool.
 *
 * @return A newly-allocated CurlTool or <code>NULL</code> upon error.
 * @memberof CurlTool.
 * @see FreeCurlTool
 */
GRASSROOTS_NETWORK_API CurlTool *AllocateCurlTool (void);


/**
 * Free a CurlTool
 *
 * @param curl_p The CurlTool to free.
 * @memberof CurlTool.
 */
GRASSROOTS_NETWORK_API void FreeCurlTool (CurlTool *curl_p);


/**
 * Allocate a new CURL object and add optionally add a
 * write to buffer callback.
 *
 * @param buffer_p If this is not NULL, then set the curl object
 * to write its response to this buffer. This can be NULL <code>NULL</code>.
 * @see AddCurlCallback
 * @return The CURL object or <code>NULL</code> upon error.
 */
GRASSROOTS_NETWORK_API CURL *GetCurl (ByteBuffer *buffer_p);


/**
 * Free a CURL object.
 *
 * @param curl_p The CURL object to free.
 */
GRASSROOTS_NETWORK_API void FreeCurl (CURL *curl_p);

/**
 * @brief Set the cryptographic engine to use.
 *
 * @param curl_p The CURL instance to set the SSL engine for.
 * @param cryptograph_engine_name_s The name of the cryptographic engine
 * to use. For the valid names see the CURL documentation.
 * @return <code>true</code> if the SSL engine name was set successfully,
 * <code>false</code> otherwise.
 */
GRASSROOTS_NETWORK_API bool SetSSLEngine (CURL *curl_p, const char *cryptograph_engine_name_s);


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
GRASSROOTS_NETWORK_API bool CallSecureUrl (const char *url_s, const char *header_data_s, const char *ca_cert_name_s, const char * const cert_name_s, const char *key_name_s, const bool verify_certs, ByteBuffer *buffer_p);


/**
 * Add a callback to write the HTTP response for this CURL object to
 * the given buffer.
 *
 * @return The CURL object to add the callback for.
 * @param buffer_p The ByteBuffer which will store the CURL object's response.
 */
GRASSROOTS_NETWORK_API bool AddCurlCallback (CURL *curl_p, ByteBuffer *buffer_p);


/**
 * Set the URI that the CurlTool will try to get the html data from.
 *
 * @param tool_p The CurlTool to update.
 * @param uri_s The URI to set the CurlTool for.
 * @return <code>true</code> if the CurlTool was updated successfully,
 * <code>false</code> otherwise.
 * @memberof CurlTool
 */
GRASSROOTS_NETWORK_API bool SetUriForCurlTool (CurlTool *tool_p, const char * const uri_s);


/**
 * @brief Run a CurlTool.
 * This will get the CurlTool to get all of the data from its given URI.
 *
 * @param tool_p The CurlTool to run.
 * @return CURLE_OK if successful or an error code upon failure.
 * @memberof CurlTool
 */
GRASSROOTS_NETWORK_API CURLcode RunCurlTool (CurlTool *tool_p);


/**
 * @brief Set up a CurlTool to do a JSON post when it is run.
 *
 * @param tool_p The CurlTool to update.
 * @return <code>true</code> if the CurlTool was updated successfully,
 * <code>false</code> otherwise.
 * @memberof CurlTool
 */
GRASSROOTS_NETWORK_API bool SetCurlToolForJSONPost (CurlTool *tool_p);


/**
 * @brief Get the downloaded data from a CurlTool.
 *
 * If the CurlTool has been run successfully, this will get a read-only
 * version of the downloaded data. <code>RunCurlTool</code> must have been
 * run prior to this.
 * @param tool_p The CurlTool to get the data from.
 * @return The downloaded data or <code>NULL</code> upon error.
 * @see RunCurlTool
 * @memberof CurlTool
 */
GRASSROOTS_NETWORK_API const char *GetCurlToolData (const CurlTool * const tool_p);


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
GRASSROOTS_NETWORK_API bool MakeRemoteJSONCallFromCurlTool (CurlTool *tool_p, json_t *req_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_CURL_TOOLS_H */

/* @} */
