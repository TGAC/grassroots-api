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
/*
 * web_service_util.c
 *
 *  Created on: 17 Feb 2015
 *      Author: tyrrells
 */

#include <string.h>

#include "web_service_util.h"
#include "string_utils.h"
#include "math_utils.h"
#include "service_config.h"


#ifdef _DEBUG
	#define WEB_SERVICE_UTIL_DEBUG	(STM_LEVEL_FINE)
#else
	#define WEB_SERVICE_UTIL_DEBUG	(STM_LEVEL_NONE)
#endif

static const char *S_METHOD_S = "method";
static const char *S_URI_S = "uri";


static const char *S_MATCH_TYPE_VALUES_SS [MT_NUM_MATCH_TYPES] =
{
	"exact", "all", "any"
};


static const char * const S_MATCH_TYPE_S = "Query matching";


static bool AddPostParameter (const Parameter * const param_p, CurlTool *curl_data_p);

static bool AppendParameterValue (ByteBuffer *buffer_p, const Parameter *param_p, CurlTool *curl_tool_p);


SubmissionMethod GetSubmissionMethod (const json_t *op_json_p)
{
	SubmissionMethod sm = SM_UNKNOWN;
	json_t *method_json_p = json_object_get (op_json_p, S_METHOD_S);

	if (method_json_p)
		{
			if (json_is_string (method_json_p))
				{
					const char *method_s = json_string_value (method_json_p);

					if (method_s)
						{
							if (strcmp (method_s, "POST") == 0)
								{
									sm = SM_POST;
								}
							else if (strcmp (method_s, "GET") == 0)
								{
									sm = SM_GET;
								}
							else if (strcmp (method_s, "BODY") == 0)
								{
									sm = SM_BODY;
								}
						}
				}
		}

	return sm;
}



bool InitWebServiceData (WebServiceData * const data_p, json_t *op_json_p)
{
	data_p -> wsd_config_p = op_json_p;
	data_p -> wsd_name_s = GetOperationNameFromJSON (op_json_p);

	if (data_p -> wsd_name_s)
		{
			data_p -> wsd_description_s = GetOperationDescriptionFromJSON (op_json_p);

			if (data_p -> wsd_description_s)
				{
					data_p -> wsd_buffer_p = AllocateByteBuffer (1024);

					if (data_p -> wsd_buffer_p)
						{
							json_t *param_set_p = json_object_get (op_json_p, PARAM_SET_KEY_S);

							if (param_set_p)
								{
									json_t *params_p = json_object_get (param_set_p, PARAM_SET_PARAMS_S);

									if (params_p)
										{
											data_p -> wsd_params_p = CreateParameterSetFromJSON (op_json_p, false);

											if (data_p -> wsd_params_p)
												{
													data_p -> wsd_curl_data_p = AllocateCurlTool (CM_MEMORY);

													if (data_p -> wsd_curl_data_p)
														{
															data_p -> wsd_method = GetSubmissionMethod (op_json_p);

															if (data_p -> wsd_method != SM_UNKNOWN)
																{
																	data_p -> wsd_base_uri_s = GetJSONString (op_json_p, S_URI_S);

																	if (data_p -> wsd_base_uri_s)
																		{
																			data_p -> wsd_info_uri_s = GetOperationInformationURIFromJSON (op_json_p);

																			return true;
																		}
																}

															FreeCurlTool (data_p -> wsd_curl_data_p);
														}

													FreeParameterSet (data_p -> wsd_params_p);
												}

										}		/* if (params_p) */

								}		/* if (param_set_p) */

							FreeByteBuffer (data_p -> wsd_buffer_p);
						}		/* if (data_p -> wsd_buffer_p) */

				}		/* if (data_p -> wsd_description_s) */

		}		/* if (data_p -> wsd_name_s) */

	return false;
}


void ClearWebServiceData (WebServiceData * const data_p)
{
	FreeParameterSet (data_p -> wsd_params_p);
	FreeByteBuffer (data_p -> wsd_buffer_p);

	FreeCurlTool (data_p -> wsd_curl_data_p);

	if (data_p -> wsd_config_p)
		{
			#if WEB_SERVICE_UTIL_DEBUG >= STM_LEVEL_FINE
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "config refcount %d", data_p -> wsd_config_p -> refcount);
			#endif

			json_decref (data_p -> wsd_config_p);
		}
}


bool CallCurlWebservice (WebServiceData *data_p)
{
	bool success_flag = false;
	CURLcode res = CURLE_FAILED_INIT;

	if (data_p -> wsd_method == SM_GET)
		{
			char *full_url_s = ConcatenateStrings (data_p -> wsd_base_uri_s, GetByteBufferData (data_p -> wsd_buffer_p));

			if (full_url_s)
				{
					res = curl_easy_setopt (data_p -> wsd_curl_data_p -> ct_curl_p, CURLOPT_URL, full_url_s);

					FreeCopiedString (full_url_s);
				}
		}
	else
		{
			res = curl_easy_setopt (data_p -> wsd_curl_data_p -> ct_curl_p, CURLOPT_URL, data_p -> wsd_base_uri_s);
		}

	if (res == CURLE_OK)
		{
			res = curl_easy_perform (data_p -> wsd_curl_data_p -> ct_curl_p);

			if (res == CURLE_OK)
				{
					success_flag = true;
				}
			else
				{
					const char *service_name_s = GetServiceName (data_p -> wsd_base_data.sd_service_p);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to call webservice for %s, error %s\n", service_name_s, curl_easy_strerror (res));
				}
		}

	return success_flag;
}





bool AddParametersToGetWebService (WebServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = true;
	ByteBuffer *buffer_p = data_p -> wsd_buffer_p;
	char c = '?';
	ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);

	/* If the url already has a ? starting the params, we need to use a & */
	const char *c_p = strrchr (data_p -> wsd_base_uri_s, '/');
	if (!c_p)
		{
			c_p = data_p -> wsd_base_uri_s;
		}

	if (strchr (c_p, '?'))
		{
			c = '&';
		}


	ResetByteBuffer (buffer_p);

	while (node_p && success_flag)
		{
			Parameter *param_p = node_p -> pn_parameter_p;
			success_flag = false;

			if (AppendToByteBuffer (buffer_p, &c, 1))
				{
					if (c == '?')
						{
							c = '&';
						}

					if (AppendToByteBuffer (buffer_p, param_p -> pa_name_s, strlen (param_p -> pa_name_s)))
						{
							const char * const equal_s = "=";

							if (AppendToByteBuffer (buffer_p, equal_s, strlen (equal_s)))
								{
									if (AppendParameterValue (buffer_p, param_p, data_p -> wsd_curl_data_p))
										{
											success_flag = true;
										}
								}
						}

				}		/* if (AppendToByteBuffer (buffer_p, &c, 1)) */

			if (success_flag)
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}

	return success_flag;
}


bool AddMatchTypeParameter (WebServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	ParameterMultiOptionArray *match_type_options_p = NULL;
	SharedType match_types_p [MT_NUM_MATCH_TYPES];
	uint32 i;

	for (i = 0; i < MT_NUM_MATCH_TYPES; ++ i)
		{
			(* (match_types_p + i)).st_string_value_s = (char *) (* (S_MATCH_TYPE_VALUES_SS + i));
		}

	match_type_options_p = AllocateParameterMultiOptionArray (MT_NUM_MATCH_TYPES, NULL, match_types_p, PT_STRING, true);

	if (match_type_options_p)
		{
			SharedType def;
			Parameter *param_p = NULL;

			def.st_string_value_s = match_types_p [0].st_string_value_s;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> wsd_base_data), param_set_p, NULL, PT_STRING, false, "Query matching", NULL,
			  "How the query will be interpreted by the service.",
			  match_type_options_p, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
				{
					success_flag = true;
				}
		}

	return success_flag;
}


MatchType GetMatchTypeParameterValue (ParameterSet * const param_set_p)
{
	MatchType mt = MT_AND;
	Parameter *param_p = DetachParameterByName (param_set_p, S_MATCH_TYPE_S);

	if (param_p)
		{
			uint32 i;
			const char * const value_s = param_p -> pa_current_value.st_string_value_s;

			for (i = 0; i < MT_NUM_MATCH_TYPES; ++ i)
				{
					if (strcmp (value_s, * (S_MATCH_TYPE_VALUES_SS + i)) == 0)
						{
							mt = (MatchType) i;
							i = MT_NUM_MATCH_TYPES; 	/* force exit from loop */
						}

				}		/* for (i = 0; i < MT_NUM_MATCH_TYPES; ++ i) */

			FreeParameter (param_p);
		}		/* if (param_p) */

	return mt;
}




static bool AddPostParameter (const Parameter * const param_p, CurlTool *curl_data_p)
{
	bool success_flag = false;
	bool alloc_flag = false;
	char *value_s = GetParameterValueAsString (param_p, &alloc_flag);

	if (value_s)
		{
			CURLFORMcode res;
			CURLformoption opt;

			switch (param_p -> pa_type)
				{
					case PT_DIRECTORY:
					case PT_FILE_TO_READ:
					case PT_FILE_TO_WRITE:
						opt = CURLFORM_FILE;
						break;

					default:
						opt = CURLFORM_COPYCONTENTS;
						break;
				}		/* switch (param_p -> pa_type) */

			res = curl_formadd (& (curl_data_p -> ct_form_p), & (curl_data_p -> ct_last_field_p), CURLFORM_COPYNAME, param_p -> pa_name_s, opt, value_s, CURLFORM_END);

			if (res == 0)
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add post parameter %s=%s to web service\n", param_p -> pa_name_s, value_s);
				}

			if (alloc_flag)
				{
					FreeCopiedString (value_s);
				}

		}		/* if (value_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get post parameter value for %s\n", param_p -> pa_name_s);
		}

	return success_flag;
}


bool AddParametersToPostWebService (WebServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = true;
	ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);

	while (node_p && success_flag)
		{
			success_flag = AddPostParameter (node_p -> pn_parameter_p, data_p -> wsd_curl_data_p);

			if (success_flag)
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}

	if (success_flag)
		{
			CURLcode res = curl_easy_setopt (data_p -> wsd_curl_data_p -> ct_curl_p, CURLOPT_HTTPPOST, data_p -> wsd_curl_data_p -> ct_form_p);

			success_flag = (res == CURLE_OK);
		}

	return success_flag;
}


/*
  curl's project page on SourceForge.net

Sponsors:
Haxx
	cURL > Mailing List > Monthly Index > Single Mail
curl-library Archives

Re: HTTP Post with json body and client SSL certificate validation

    This message: [ Message body ] [ More options ]
    Related messages: [ Next message ] [ Previous message ] [ In reply to ]

From: C�dric Deltheil <cedric_at_moodstocks.com>
Date: Fri, 27 Sep 2013 14:31:48 +0200

Le 27 sept. 2013 � 13:28, Victor Dodon <dodonvictor_at_gmail.com> a �crit :

> Q1. I need to set some custom headers for each request. The list of custom headers can be freed imediately after curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist) or only after the curl_easy_perform has returned?

After. See the HTTP custom header example[1].

> Q2. How to do a post request with json HTTP body? Is something like this:
>
> char *post_body = json_dumps(json, 0);
> struct curl_slist *slist=NULL;
> slist = curl_slist_append(slist, "Content-Type: application/json");
> curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
> curl_easy_setopt(curl, CURLOPT_POST, 1);
> curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);
> curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_body);
>
> ?

Yes. You do not even need to pass the CURLOPT_POSTFIELDSIZE since by default libcurl will do the strlen() for you.

Also, I would say CURLOPT_POST is redundant: you can omit it.

> Q3. Microsoft and Apple server require client SSL certificate validation. Assuming that I have a suitable SSL certififcate, what options I need to set on curl handles to achieve this? CURLOPT_SSLCERT and CURLOPT_SSLCERTTYPE are enough?

typedef struct CurlTool
{
	struct curl_httppost *ct_form_p;
	struct curl_httppost *ct_last_field_p;
	struct curl_slist *ct_headers_list_p;
} CurlTool;


Don't know about that :)
*/

bool AddParametersToBodyWebService (WebServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	ByteBuffer *buffer_p = data_p -> wsd_buffer_p;
	const SchemaVersion *sv_p = GetSchemaVersion ();
	json_t *json_p = GetParameterSetAsJSON (param_set_p, sv_p, true);

	if (json_p)
		{
			char *dump_s = json_dumps (json_p, JSON_INDENT (2));

			if (dump_s)
				{
					if (AppendToByteBuffer (buffer_p, dump_s, strlen (dump_s)))
						{
							CurlTool *curl_data_p = data_p -> wsd_curl_data_p;
							curl_data_p -> ct_headers_list_p = curl_slist_append (curl_data_p -> ct_headers_list_p, "Content-Type: application/json");

							if (curl_data_p -> ct_headers_list_p)
								{
									CURLcode res = curl_easy_setopt (curl_data_p -> ct_curl_p, CURLOPT_POSTFIELDS, buffer_p -> bb_data_p);

									if (res == CURLE_OK)
										{
											res = curl_easy_setopt (curl_data_p -> ct_curl_p, CURLOPT_POSTFIELDSIZE, GetByteBufferSize (buffer_p));

											if (res == CURLE_OK)
												{
													success_flag = true;
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set postfield size for json call by %s, error %s\n", GetServiceName (data_p -> wsd_base_data.sd_service_p), curl_easy_strerror (res));
												}

										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set postfield for json call by %s, error %s\n", GetServiceName (data_p -> wsd_base_data.sd_service_p), curl_easy_strerror (res));
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set content type header for json call by %s\n", GetServiceName (data_p -> wsd_base_data.sd_service_p));
								}

						}

					free (dump_s);
				}

			json_decref (json_p);
		}

	return success_flag;
}



static bool AppendParameterValue (ByteBuffer *buffer_p, const Parameter *param_p, CurlTool *curl_tool_p)
{
	bool success_flag = false;
	char *value_s = NULL;
	const SharedType * const value_p = & (param_p -> pa_current_value);
	enum alloc_type { AT_NONE, AT_STANDARD, AT_URL_ENCODED_STRING };
	enum alloc_type alloc_value = AT_NONE;


	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				value_s = (char *) ((value_p -> st_boolean_value == true) ? "true" : "false");
				break;

			case PT_SIGNED_INT:
			case PT_NON_POSITIVE_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_long_value), 0);
				alloc_value = AT_STANDARD;
				break;

			case PT_UNSIGNED_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_ulong_value), 0);
				alloc_value = AT_STANDARD;
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				value_s = ConvertNumberToString (value_p -> st_data_value, 0);
				alloc_value = AT_STANDARD;
				break;

			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				{
					value_s = GetURLEscapedString (curl_tool_p, value_p -> st_resource_value_p -> re_value_s);
					alloc_value = AT_URL_ENCODED_STRING;
				}
				break;

			case PT_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				{
					value_s = GetURLEscapedString (curl_tool_p, value_p -> st_string_value_s);
					alloc_value = AT_URL_ENCODED_STRING;
				}
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */

	if (value_s)
		{
			success_flag = AppendToByteBuffer (buffer_p, value_s, strlen (value_s));

			switch (alloc_value)
				{
					case AT_STANDARD:
						FreeCopiedString (value_s);
						break;

					case AT_URL_ENCODED_STRING:
						FreeURLEscapedString (value_s);
						break;

					case AT_NONE:
						break;

				}
		}

	return success_flag;
}

