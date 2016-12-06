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

#include <string.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "jansson.h"

#include "curl_tools.h"
#include "streams.h"
#include "memory_allocations.h"

#include "string_utils.h"
#include "streams.h"


#ifdef _DEBUG
	#define CURL_TOOLS_DEBUG	(STM_LEVEL_FINE)
#else
	#define CURL_TOOLS_DEBUG	(STM_LEVEL_NONE)
#endif


typedef struct CURLParam
{
	CURLoption cp_opt;
	const char *cp_value_s;
} CURLParam;


static size_t WriteMemoryCallback (char *response_data_p, size_t block_size, size_t num_blocks, void *store_p);

static size_t WriteFileCallback (char *response_data_p, size_t block_size, size_t num_blocks, void *store_p);

static bool SetCurlToolJSONRequestData (CurlTool *tool_p, json_t *json_p);


CurlTool *AllocateCurlTool (CurlMode mode)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);
	
	if (buffer_p)
		{
			CurlTool *curl_tool_p = (CurlTool *) AllocMemory (sizeof (CurlTool));

			if (curl_tool_p)
				{
					if (SetupCurl (curl_tool_p, buffer_p))
						{
							curl_tool_p -> ct_buffer_p = buffer_p;
							curl_tool_p -> ct_form_p = NULL;
							curl_tool_p -> ct_last_field_p = NULL;
							curl_tool_p -> ct_headers_list_p = NULL;
							curl_tool_p -> ct_temp_f = NULL;
							curl_tool_p -> ct_mode = mode;

							return curl_tool_p;
						}

					FreeMemory (curl_tool_p);
				}		/* if (curl_tool_p) */

			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */
		
	return NULL;
}


void FreeCurlTool (CurlTool *curl_tool_p)
{
	FreeCurl (curl_tool_p -> ct_curl_p);

	if (curl_tool_p -> ct_headers_list_p)
		{
			curl_slist_free_all (curl_tool_p -> ct_headers_list_p);
		}

	FreeByteBuffer (curl_tool_p -> ct_buffer_p);
	FreeMemory (curl_tool_p);
}


bool SetupCurl (CurlTool *tool_p, ByteBuffer *buffer_p)
{
	tool_p -> ct_curl_p = curl_easy_init ();
	
	if (tool_p -> ct_curl_p)
		{
			if (buffer_p)
				{
					if (AddCurlCallback (tool_p, buffer_p))
						{
							#if CURL_TOOLS_DEBUG >= STM_LEVEL_FINER
							curl_easy_setopt (curl_p, CURLOPT_VERBOSE, 1L);
							#endif

							return true;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add buffer callback for curl object\n");
						}
				}

			FreeCurl (tool_p -> ct_curl_p);
			tool_p -> ct_curl_p = NULL;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create curl object\n");
		}

	return false;
}


void FreeCurl (CURL *curl_p)
{
	curl_easy_cleanup (curl_p);
}


bool SetUriForCurlTool (CurlTool *tool_p, const char * const uri_s)
{
	bool success_flag = false;
	CURLcode res = curl_easy_setopt (tool_p -> ct_curl_p, CURLOPT_URL, uri_s);

	if (res == CURLE_OK)
		{
			success_flag = true;
		}

	return success_flag;
}


bool MakeRemoteJSONCallFromCurlTool (CurlTool *tool_p, json_t *req_p)
{
	bool success_flag = false;
	char *dump_s = json_dumps (req_p, 0);

	if (dump_s)
		{
			if (curl_easy_setopt (tool_p -> ct_curl_p, CURLOPT_POSTFIELDS, dump_s) == CURLE_OK)
				{
					CURLcode res;

					/* if the buffer isn't empty, clear it */
					if (GetByteBufferSize (tool_p -> ct_buffer_p) > 0)
						{
							ResetByteBuffer (tool_p -> ct_buffer_p);
						}

					res = RunCurlTool (tool_p);

					if (res == CURLE_OK)
						{
							success_flag = true;
						}
					else
						{
							const char *error_s = curl_easy_strerror (res);

							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "RunCurlTool failed with code " UINT32_FMT ": %s\n", res, error_s ? error_s : "NULL");
						}
				}

			free (dump_s);
		}		/* if (dump_s) */

	return success_flag;
}


CURLcode RunCurlTool (CurlTool *tool_p)
{
	CURLcode res = CURLE_OK;

	if (tool_p -> ct_headers_list_p)
		{
			res = curl_easy_setopt (tool_p -> ct_curl_p, CURLOPT_HTTPHEADER, tool_p -> ct_headers_list_p);
		}

	if (res == CURLE_OK)
		{
			res = curl_easy_perform (tool_p -> ct_curl_p);
		}

	return res;
}


bool SetSSLEngine (CurlTool *curl_p, const char *cryptograph_engine_name_s)
{
	bool success_flag = false;
	
	/* load the crypto engine */
	if (curl_easy_setopt (curl_p -> ct_curl_p, CURLOPT_SSLENGINE, cryptograph_engine_name_s) == CURLE_OK)
		{
			/* set the crypto engine as default */
			/* only needed for the first time you load a engine in a curl object... */
			if (curl_easy_setopt (curl_p -> ct_curl_p, CURLOPT_SSLENGINE_DEFAULT, 1L) == CURLE_OK)
				{
					success_flag = true;
				}
			else
				{ 
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "can't set crypto engine as default\n");
				}
		}
	else
		{	                     
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "can't set crypto engine %s\n", cryptograph_engine_name_s);
		}

	return success_flag;
}


bool AddCurlCallback (CurlTool *curl_tool_p, ByteBuffer *buffer_p)
{
	bool success_flag = true;

	curl_write_callback callback_fn = WriteMemoryCallback;
	const CURLParam params [] = 
		{
			{ CURLOPT_WRITEFUNCTION, (const char *)  callback_fn },
			{ CURLOPT_WRITEDATA, (const char *) buffer_p },

			/* set default user agent */
	    { CURLOPT_USERAGENT,  "libcurl-agent/1.0" },

	    /* set timeout */
	   // { CURLOPT_TIMEOUT, (const char *) 5 },

	    /* enable location redirects */
	    { CURLOPT_FOLLOWLOCATION, (const char *) 1},

	    /* set maximum allowed redirects */
	    { CURLOPT_MAXREDIRS, (const char *) 1 },

			{ CURLOPT_LASTENTRY, (const char *) NULL }
		};

	const CURLParam *param_p = params;


	while (success_flag && (param_p -> cp_value_s))
		{
			CURLcode ret = curl_easy_setopt (curl_tool_p -> ct_curl_p, param_p -> cp_opt, (void *) param_p -> cp_value_s);

			if (ret == CURLE_OK)
				{
					++ param_p;
				}
			else
				{
					success_flag = false;
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to to set CURL option \"%d\"\n", param_p -> cp_opt);
				}

		}		/* while (continue_flag && param_p) */


	return success_flag;
}


static bool SetCurlToolJSONRequestData (CurlTool *tool_p, json_t *json_p)
{
	bool success_flag = false;
	char *dump_s = json_dumps (json_p, 0);

	if (dump_s)
		{
			if (curl_easy_setopt (tool_p -> ct_curl_p, CURLOPT_COPYPOSTFIELDS, dump_s) == CURLE_OK)
				{
					success_flag = true;
				}

			free (dump_s);
		}		/* if (dump_s) */

	return success_flag;
}



bool SetCurlToolHeader (CurlTool *tool_p, const char *key_s, const char *value_s)
{
	bool success_flag = false;
	char *header_s = ConcatenateVarargsStrings (key_s, ":", value_s, NULL);

	if (header_s)
		{
			tool_p -> ct_headers_list_p = curl_slist_append (tool_p -> ct_headers_list_p, header_s);

			success_flag = true;
			FreeCopiedString (header_s);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to construct header for \"%s\" and \"%s\"", key_s, value_s);
		}

	return success_flag;
}


bool SetCurlToolForJSONPost (CurlTool *tool_p)
{
	bool success_flag = true;

	const CURLParam params [] =
		{
///			{ CURLOPT_POST, 1 },
			{ CURLOPT_HTTPHEADER, (const char *) (tool_p -> ct_headers_list_p)  },
			{ CURLOPT_LASTENTRY, (const char *) NULL }
		};

	const CURLParam *param_p = params;

	tool_p -> ct_headers_list_p = curl_slist_append (tool_p -> ct_headers_list_p, "Accept: application/json");
	tool_p -> ct_headers_list_p = curl_slist_append (tool_p -> ct_headers_list_p, "Content-Type: application/json");

	while (success_flag && (param_p -> cp_value_s))
		{
			CURLcode ret = curl_easy_setopt (tool_p, param_p -> cp_opt, param_p -> cp_value_s);
			
			if (ret == CURLE_OK)
				{
					++ param_p;
				}
			else
				{
					success_flag = false;
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set CURL option \"%s\" to \"%s\"\n", param_p -> cp_opt, param_p -> cp_value_s);
				}
				
		}		/* while (continue_flag && param_p) */
	
	return success_flag;
}


char *GetURLEscapedString (CurlTool *tool_p, const char *src_s)
{
	char *escaped_s = curl_easy_escape (tool_p -> ct_curl_p, src_s, 0);

	if (!escaped_s)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to created URL escaped version of \"%s\"\n", src_s);
		}

	return escaped_s;
}


void FreeURLEscapedString (char *value_s)
{
	curl_free (value_s);
}




const char *GetCurlToolData (const CurlTool * const tool_p)
{
	return (GetByteBufferData (tool_p -> ct_buffer_p));
}



static size_t WriteFileCallback (char *response_data_p, size_t block_size, size_t num_blocks, void *store_p)
{
	size_t result = CURLE_OK;
	FILE *out_f = (FILE *) store_p;
	size_t num_written = fwrite (response_data_p, block_size, num_blocks, out_f);

	if (num_written != num_blocks)
		{
			const int err_no = ferror (out_f);

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Error writing Curl callback data to file, %s", strerror (err_no));
			result= CURLE_WRITE_ERROR;
		}


	return result;
}


static size_t WriteMemoryCallback (char *response_data_p, size_t block_size, size_t num_blocks, void *store_p)
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
