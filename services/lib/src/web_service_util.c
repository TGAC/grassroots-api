/*
 * web_service_util.c
 *
 *  Created on: 17 Feb 2015
 *      Author: tyrrells
 */

#include <string.h>

#include "web_service_util.h"


static const char *S_METHOD_S = "method";
static const char *S_URI_S = "uri";

static bool SetURI (const json_t *op_json_p, CURL *curl_p);


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
	bool success_flag = false;

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
							json_t *params_p = json_object_get (op_json_p, PARAM_SET_PARAMS_S);

							if (params_p)
								{
									data_p -> wsd_params_p = CreateParameterSetFromJSON (op_json_p);

									if (data_p -> wsd_params_p)
										{
											data_p -> wsd_curl_data_p = AllocateCurlTool ();

											if (data_p -> wsd_curl_data_p)
												{
													data_p -> wsd_method = GetSubmissionMethod (op_json_p);

													if (data_p -> wsd_method != SM_UNKNOWN)
														{
															if (SetURI (op_json_p, data_p -> wsd_curl_data_p -> ct_curl_p))
																{
																	success_flag = true;
																}
														}

													FreeCurlTool (data_p -> wsd_curl_data_p);
												}

											FreeParameterSet (data_p -> wsd_params_p);
										}

								}		/* if (params_p) */

							FreeByteBuffer (data_p -> wsd_buffer_p);
						}
				}
		}

	return success_flag;
}


void ClearWebServiceData (WebServiceData * const data_p)
{
	FreeParameterSet (data_p -> wsd_params_p);
	FreeByteBuffer (data_p -> wsd_buffer_p);

	FreeCurlTool (data_p -> wsd_curl_data_p);

	json_decref (data_p -> wsd_config_p);
}



static bool SetURI (const json_t *op_json_p, CURL *curl_p)
{
	bool success_flag = false;
	const char *uri_s = GetJSONString (op_json_p, S_URI_S);

	if (uri_s)
		{
			CURLcode res = curl_easy_setopt (curl_p, CURLOPT_URL, uri_s);

			if (res == CURLE_OK)
				{
					success_flag = true;
				}
		}

	return success_flag;
}
