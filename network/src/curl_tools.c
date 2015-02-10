/*
 * This code is based on the simplessl.c example code from http://curl.haxx.se/libcurl/c/simplessl.html
 */

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 */
/* some requirements for this to work:
   1.   set pCertFile to the file with the client certificate
   2.   if the key is passphrase protected, set pPassphrase to the
        passphrase you use
   3.   if you are using a crypto engine:
   3.1. set a #define USE_ENGINE
   3.2. set pEngine to the name of the crypto engine you use
   3.3. set pKeyName to the key identifier you want to use
   4.   if you don't use a crypto engine:
   4.1. set pKeyName to the file name of your client key
   4.2. if the format of the key file is DER, set pKeyType to "DER"

   !! verify of the server certificate is not implemented here !!

   **** This example only works with libcurl 7.9.3 and later! ****

*/
 
/***************************************************************************/

#include <curl/curl.h>
#include <curl/easy.h>

#include "curl_tools.h"
#include "streams.h"
#include "memory_allocations.h"

typedef struct CURLParam
{
	CURLoption cp_opt;
	const char *cp_value_s;
} CURLParam;


static size_t WriteMemoryCallback (void *response_data_p, size_t block_size, size_t num_blocks, void *store_p);



CurlTool *AllocateCurlTool (void)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);
	
	if (buffer_p)
		{
			CURL *curl_p = curl_easy_init ();
			
			if (curl_p)
				{
					if (AddCurlCallback (curl_p, buffer_p))
						{
							CurlTool *curl_tool_p = (CurlTool *) AllocMemory (sizeof (CurlTool));
						
							if (curl_tool_p)
								{
									curl_tool_p -> ct_curl_p = GetCurl (buffer_p);

									if (curl_tool_p -> ct_curl_p)
										{
											curl_tool_p -> ct_buffer_p = buffer_p;
											curl_tool_p -> ct_form_p = NULL;
											curl_tool_p -> ct_last_field_p = NULL;			
											curl_tool_p -> ct_headers_list_p = NULL;					
											
											return curl_tool_p;
										}
								
									FreeMemory (curl_tool_p);
								}		/* if (curl_tool_p) */			
																
						}		/* if (AddCurlCallback (curl_p, buffer_p)) */
					
					curl_easy_cleanup (curl_p);
				}		/* if (curl_p) */
				
			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */
		
	return NULL;
}


void FreeCurlTool (CurlTool *curl_tool_p)
{
	FreeCurl (curl_tool_p -> ct_curl_p);
	FreeByteBuffer (curl_tool_p -> ct_buffer_p);
	FreeMemory (curl_tool_p);
}


CURL *GetCurl (ByteBuffer *buffer_p)
{
	CURL *curl_p = curl_easy_init ();
	
	if (curl_p)
		{
			if (buffer_p)
				{
					if (!AddCurlCallback (curl_p, buffer_p))
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to add buffer callback for curl object\n");
							FreeCurl (curl_p);
							curl_p = NULL;
						}
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to create curl object\n");			
		}

	return curl_p;
}


void FreeCurl (CURL *curl_p)
{
	curl_easy_cleanup (curl_p);
}


bool SetSSLEngine (CURL *curl_p, const char *cryptograph_engine_name_s)
{
	bool success_flag = false;
	
	/* load the crypto engine */
	if (curl_easy_setopt (curl_p, CURLOPT_SSLENGINE, cryptograph_engine_name_s) == CURLE_OK)
		{
			/* set the crypto engine as default */
			/* only needed for the first time you load a engine in a curl object... */
			if (curl_easy_setopt (curl_p, CURLOPT_SSLENGINE_DEFAULT, 1L) == CURLE_OK)
				{
					success_flag = true;
				}
			else
				{ 
					PrintErrors (STM_LEVEL_SEVERE, "can't set crypto engine as default\n");
				}
		}
	else
		{	                     
			PrintErrors (STM_LEVEL_SEVERE, "can't set crypto engine %s\n", cryptograph_engine_name_s);
		}

	return success_flag;
}


bool CallSecureUrl (const char *url_s, const char *header_data_s, const char *ca_cert_name_s, const char * const cert_name_s, const char *key_name_s, const bool verify_certs, ByteBuffer *buffer_p)
{
	bool success_flag = false;
	CURL *curl_p = curl_easy_init ();
	
	if (curl_p)
		{
			CURLcode res;

			if (AddCurlCallback (curl_p, buffer_p))
				{
					const CURLParam params [] = 
						{
//							{ CURLOPT_CAINFO, ca_cert_name_s },

							/* what call to write: */
							{ CURLOPT_URL, url_s },

					//		{ CURLOPT_POSTFIELDS, post_data_s },
							/* cert is stored PEM coded in file... */
							/* since PEM is default, we needn't set it for PEM */
							
							/* set the cert for client authentication */
//							{ CURLOPT_SSLCERT, cert_name_s },							
//							{ CURLOPT_SSLCERTTYPE, "PEM" },

//							{ CURLOPT_SSLKEY, key_name_s },
//							{ CURLOPT_SSLKEYTYPE, "PEM" },
 
//							{ CURLOPT_SSL_VERIFYPEER, (const char *) (verify_certs ? 1L : 0L) },
							{ CURLOPT_VERBOSE,  (const char *) 1L},
//							{ CURLOPT_CERTINFO,  1L},
							
							{ CURLOPT_LASTENTRY, (const char *) NULL }
						};
					const CURLParam *param_p = params;
					
					success_flag = true;
					
					while (success_flag && (param_p -> cp_value_s))
						{
							CURLcode ret = curl_easy_setopt (curl_p, param_p -> cp_opt, param_p -> cp_value_s);
							
							if (ret == CURLE_OK)
								{
									++ param_p;
								}
							else
								{
									success_flag = false;
									PrintErrors (STM_LEVEL_SEVERE, "Failed to to set CURL option \"%s\" to \"%s\"\n", param_p -> cp_opt, param_p -> cp_value_s);
								}
								
						}		/* while (continue_flag && param_p) */
					
					if (success_flag)
						{
							struct curl_certinfo cert_info;
							
							/* Perform the request, res will get the return code */
							res = curl_easy_perform (curl_p);

							/* Check for errors */
							if (res != CURLE_OK)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to access %s with curl error: %s\n", url_s, curl_easy_strerror (res));
									success_flag = false;
								}																							
																					
							res = curl_easy_getinfo (curl_p, CURLINFO_CERTINFO, &cert_info);													
																				
							printf ("here\n");
																					
						}		/* if (continue_flag) */
					
				}		/* if (AddCurlCallback (curl_p, buffer_p)) */
						
			curl_easy_cleanup (curl_p);
		}		/* if (curl_p) */
	
	return success_flag;
}




bool AddCurlCallback (CURL *curl_p, ByteBuffer *buffer_p)
{
	bool success_flag = true;
	const CURLParam params [] = 
		{
			{ CURLOPT_WRITEFUNCTION, (const char *)  WriteMemoryCallback },
			{ CURLOPT_WRITEDATA, (const char *) buffer_p },
			{ CURLOPT_LASTENTRY, (const char *) NULL }
		};
	const CURLParam *param_p = params;
					
					
	while (success_flag && (param_p -> cp_value_s))
		{
			CURLcode ret = curl_easy_setopt (curl_p, param_p -> cp_opt, param_p -> cp_value_s);
			
			if (ret == CURLE_OK)
				{
					++ param_p;
				}
			else
				{
					success_flag = false;
					PrintErrors (STM_LEVEL_SEVERE, "Failed to to set CURL option \"%s\" to \"%s\"\n", param_p -> cp_opt, param_p -> cp_value_s);
				}
				
		}		/* while (continue_flag && param_p) */
				
	
	return success_flag;
}



static size_t WriteMemoryCallback (void *response_data_p, size_t block_size, size_t num_blocks, void *store_p)
{
	size_t total_size = block_size * num_blocks;
	size_t result = CURLE_OK;
	ByteBuffer *buffer_p = (ByteBuffer *) store_p;

	size_t remaining_space = GetRemainingSpaceInByteBuffer (buffer_p);

	if (remaining_space < total_size)
		{
			if (!ExtendByteBuffer (buffer_p, total_size - remaining_space + 1))
				{
					result = CURLE_OUT_OF_MEMORY;
				}
		}

	if (result == CURLE_OK)
		{
			if (AppendToByteBuffer (buffer_p, response_data_p, total_size))
				{
					result = total_size;
				}
		}
	
	return result;
}
