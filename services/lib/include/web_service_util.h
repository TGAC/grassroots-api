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
 * web_service_util.h
 *
 *  Created on: 17 Feb 2015
 *      Author: tyrrells
 */

#ifndef WEB_SERVICE_UTIL_H_
#define WEB_SERVICE_UTIL_H_

#include "jansson.h"

#include "grassroots_service_library.h"
#include "service.h"
#include "parameter_set.h"
#include "byte_buffer.h"
#include "curl_tools.h"
#include "service_job.h"


typedef enum MatchType
{
	MT_EXACT_MATCH,
	MT_ALL,
	MT_OR,
	MT_NUM_MATCH_TYPES
} MatchType;

typedef enum SubmissionMethod
{
	SM_UNKNOWN = -1,
	SM_POST,
	SM_GET,
	SM_BODY,
	SM_NUM_METHODS
} SubmissionMethod;



typedef struct WebServiceData
{
	ServiceData wsd_base_data;
	json_t *wsd_config_p;
	const char *wsd_name_s;
	const char *wsd_description_s;
	const char *wsd_info_uri_s;
	ParameterSet *wsd_params_p;
	ByteBuffer *wsd_buffer_p;
	const char *wsd_base_uri_s;
	SubmissionMethod wsd_method;
	CurlTool *wsd_curl_data_p;
} WebServiceData;


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_API SubmissionMethod GetSubmissionMethod (const json_t *op_json_p);


GRASSROOTS_SERVICE_API bool InitWebServiceData (WebServiceData * const data_p, json_t *op_json_p);


GRASSROOTS_SERVICE_API void ClearWebServiceData (WebServiceData * const data_p);


GRASSROOTS_SERVICE_API bool CallCurlWebservice (WebServiceData *data_p);


GRASSROOTS_SERVICE_API bool AddParametersToGetWebService (WebServiceData *data_p, ParameterSet *param_set_p);


GRASSROOTS_SERVICE_API bool AddParametersToPostWebService (WebServiceData *data_p, ParameterSet *param_set_p);


GRASSROOTS_SERVICE_API bool AddParametersToBodyWebService (WebServiceData *data_p, ParameterSet *param_set_p);


GRASSROOTS_SERVICE_API ServiceJobSet *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);


GRASSROOTS_SERVICE_API bool AddMatchTypeParameter (ParameterSet *param_set_p);


GRASSROOTS_SERVICE_API MatchType GetMatchTypeParameterValue (ParameterSet * const param_set_p);


#ifdef __cplusplus
}
#endif


#endif /* WEB_SERVICE_UTIL_H_ */
