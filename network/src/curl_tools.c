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

#include "curl_tools.h"


typedef struct CURLParam
{
	CURLOption cp_opt;
	const char *cp_value_s;
} CURLParam;


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
					break;
				}
		}
	else
		{	                     
			PrintErrors (STM_LEVEL_SEVERE, "can't set crypto engine %s\n", cryptograph_engine_name_s);
		}

	return success_flag;
}


bool CallUrl (const char *url_s, const char *header_data_s, const char *cryptograph_engine_name_s, const bool verify_certs)
{
	bool success_flag = false;
	CURL *curl_p = curl_easy_init ();
	
	if (curl_p)
		{
			bool continue_flag = true;
			CURLcode res;
			const char *pass_phrase_s = NULL;

			static const char *cert_file_s = "testcert.pem";
			static const char *ca_cert_file_s = "cacert.pem";

			const char *key_name_s = NULL;
			const char *key_type_s = NULL;

		#ifdef USE_ENGINE
			key_name_s  = "rsa_test";
			key_type_s  = "ENG";
			cryptograph_engine_name_s   = "chil";            /* for nChiper HSM... */
		#else
			key_name_s  = "testkey.pem";
			key_type_s  = "PEM";
			cryptograph_engine_name_s   = NULL;
		#endif

			
      if (cryptograph_engine_name_s)             /* use crypto engine */
				{
					continue_flag = SetSSLEngine (curl_p, cryptograph_engine_name_s);
				}
			
			if (continue_flag)
				{
					const CURLParam params [] = 
						{
							/* what call to write: */
							{ CURLOPT_URL, url_s },
							{ CURLOPT_HEADERDATA, header_data_s },

							/* cert is stored PEM coded in file... */
							/* since PEM is default, we needn't set it for PEM */
							{ CURLOPT_SSLCERTTYPE, "PEM" },
							
							/* set the cert for client authentication */
							{ CURLOPT_SSLCERT, cert_file_s },
							
							{ CURLOPT_SSLKEYTYPE, key_type_s },
							{ CURLOPT_SSLKEY, key_name_s },
							{ CURLOPT_CAINFO, ca_cert_file_s },
							{ CURLOPT_SSL_VERIFYPEER, verify_certs ? 1L : 0L },
							NULL
						};
					const CURLParam *param_p = params;
					
					
					while (continue_flag && param_p)
						{
							if (curl_easy_setopt (curl_p, param_p -> cp_opt, param_p -> cp_value_s) == CURLE_OK)
								{
									++ param_p;
								}
							else
								{
									continue_flag = false;
									PrintErrors (STM_LEVEL_SEVERE, "Failed to to set CURL option \"%s\" to \"%s\"\n", param_p -> cp_opt, param_p -> cp_value_s);
								}
								
						}		/* while (continue_flag && param_p) */
					
					if (continue_flag)
						{
							/* sorry, for engine we must set the passphrase
								 (if the key has one...) */
							if (pass_phrase_s)
								{
									continue_flag = (curl_easy_setopt (curl_p, CURLOPT_KEYPASSWD, pass_phrase_s) == CURLE_OK);
								}

							if (continue_flag)
								{
									/* Perform the request, res will get the return code */
									res = curl_easy_perform (curl_p);

									/* Check for errors */
									if (res == CURLE_OK)
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to access %s with curl error: %s\n", url_s, curl_easy_strerror (res));
										}																							
														
								}		/* if (continue_flag) */
							
						}		/* if (continue_flag) */
					
				}		/* if (continue_flag) */
			
			curl_easy_cleanup (curl_p);
		}		/* if (curl_p) */
	
	return success_flag;
}

