
#include <stdio.h>
#include <curl/curl.h>

#include "curl_tools.h"
#include "selector.hpp"
 
CURLcode SimpleTest (CurlTool *tool_p, const char * const url_s);
CURLcode ComplexTest (CurlTool *tool_p, const char * const url_s, const char * const ca_certificate_s, const char * const certificate_s, const char *key_name_s);
CURLcode Test1 (CurlTool *tool_p);
HtmlLinkArray *GetLinks (CurlTool *tool_p, const char * const uri_s, const char * const selector_s);

//#define SKIP_PEER_VERIFICATION (1)

int main (int argc, char *argv [])
{
	if (argc == 5)
		{
			CurlTool *tool_p = AllocateCurlTool ();

			if (tool_p)
				{
					HtmlLinkArray *links_p = GetLinks (tool_p, "http://agris.fao.org/agris-search/searchIndex.do?query=wheat", "li.result-item h3 a");

					if (links_p)
						{
							size_t i = links_p -> hla_num_entries;
							HtmlLink *link_p = links_p -> hla_data_p;

							for ( ; i > 0; -- i, ++ link_p)
								{
									printf ("link -> uri_s \"%s\" data \"%s\" title \"%s\"\n", link_p -> hl_uri_s, link_p -> hl_data_s, link_p -> hl_title_s);
								}

							FreeHtmlLinkArray (links_p);
						}

					FreeCurlTool (tool_p);
				}
		}
		
	return 0;
}



CURLcode ComplexTest (CurlTool *tool_p, const char * const url_s, const char * const ca_certificate_s, const char * const certificate_s, const char *key_name_s)
{
	CURL *curl_p = tool_p -> ct_curl_p;

	CURLcode res = CURLE_OK;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	printf ("ComplexTest init\n");
	
	if (buffer_p)
		{
			char *header_data_s = "";
			bool verify_certs = false;		
			bool success_flag = CallSecureUrl (url_s, header_data_s, ca_certificate_s, certificate_s, key_name_s, verify_certs, buffer_p);

			printf ("%d\n%s\n", (int) success_flag, buffer_p -> bb_data_p);



			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */
	else
		{
			res = CURLE_OUT_OF_MEMORY;
		}

	printf ("ComplexTest exit\n");

	return res;
}


CURLcode SimpleTest (CurlTool *tool_p, const char * const url_s)
{
	CURL *curl_p = tool_p -> ct_curl_p;
	CURLcode res;
	curl_easy_setopt (curl_p, CURLOPT_URL, url_s);
 
	printf ("SimpleTest init\n");
 
#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
 
#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt (curl_p, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform (curl_p);
    /* Check for errors */ 

    if (res == CURLE_OK)
			{
				printf ("curl_easy_perform () on %s succeeded\n", url_s);
			}
		else
			{
				printf ("curl_easy_perform () on %s failed: %s\n", url_s, curl_easy_strerror (res));
			}

	printf ("SimpleTest exit\n");
			 
  return res;
}


CURLcode Test1 (CurlTool *tool_p)
{
	CURL *curl_p = tool_p -> ct_curl_p;
  CURLcode ret;

	printf ("Test1 init\n");

  curl_easy_setopt (curl_p, CURLOPT_URL, "https://n79610.nbi.ac.uk:8843");
  curl_easy_setopt (curl_p, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt (curl_p, CURLOPT_USERAGENT, "curl/7.22.0 (x86_64-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3");
  curl_easy_setopt (curl_p, CURLOPT_CAINFO, "/usr/local/apache2/conf/cacert.pem");
  curl_easy_setopt (curl_p, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt (curl_p, CURLOPT_SSH_KNOWNHOSTS, "/usr/users/ga002/tyrrells/.ssh/known_hosts");
  curl_easy_setopt (curl_p, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt (curl_p, CURLOPT_VERBOSE, 1L);
  
  ret = curl_easy_perform (curl_p);

  /* Here is a list of options the curl code used that cannot get generated
     as source easily. You may select to either not use them or implement
     them yourself.

  CURLOPT_WRITEDATA set to a objectpointer
  CURLOPT_WRITEFUNCTION set to a functionpointer
  CURLOPT_READDATA set to a objectpointer
  CURLOPT_READFUNCTION set to a functionpointer
  CURLOPT_SEEKDATA set to a objectpointer
  CURLOPT_SEEKFUNCTION set to a functionpointer
  CURLOPT_ERRORBUFFER set to a objectpointer
  CURLOPT_SSLCERT set to a objectpointer
  CURLOPT_SSLKEY set to a objectpointer
  CURLOPT_STDERR set to a objectpointer
  CURLOPT_DEBUGFUNCTION set to a functionpointer
  CURLOPT_DEBUGDATA set to a objectpointer
  CURLOPT_SOCKOPTFUNCTION set to a functionpointer
  CURLOPT_SOCKOPTDATA set to a objectpointer

  */
  
	printf ("Test1 exit\n");  
  return ret;
}


