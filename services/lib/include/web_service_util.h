/*
 * web_service_util.h
 *
 *  Created on: 17 Feb 2015
 *      Author: tyrrells
 */

#ifndef WEB_SERVICE_UTIL_H_
#define WEB_SERVICE_UTIL_H_

#include "jansson.h"

#include "wheatis_service_library.h"
#include "service.h"
#include "parameter_set.h"
#include "byte_buffer.h"
#include "curl_tools.h"


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
	ParameterSet *wsd_params_p;
	ByteBuffer *wsd_buffer_p;
	SubmissionMethod wsd_method;
	CurlTool *wsd_curl_data_p;
} WebServiceData;

#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API SubmissionMethod GetSubmissionMethod (const json_t *op_json_p);


WHEATIS_SERVICE_API bool InitWebServiceData (WebServiceData * const data_p, json_t *op_json_p);


WHEATIS_SERVICE_API void ClearWebServiceData (WebServiceData * const data_p);


#ifdef __cplusplus
}
#endif


#endif /* WEB_SERVICE_UTIL_H_ */
