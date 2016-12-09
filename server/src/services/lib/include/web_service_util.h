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

/**
 * @file
 * @brief
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


/**
 * An enumeration detailing how to combine different parameters upon submission to a web service.
 */
typedef enum MatchType
{
	/** The user requires an exact match of the parameter values. */
	MT_EXACT_MATCH,

	/** The user wants the parameters AND'ed together. */
	MT_AND,

	/** Treat the parameters as a set of options to be OR'ed together. */
	MT_OR,

	/** The number of MatchTypes */
	MT_NUM_MATCH_TYPES
} MatchType;


/**
 * An enumeration of the different web-based
 * submission methods.
 */
typedef enum SubmissionMethod
{
	/** An invalid SubmissionMethod. */
	SM_UNKNOWN = -1,

	/** An HTTP POST request. */
	SM_POST,

	/** An HTTP GET request. */
	SM_GET,

	/**
	 * An HTTP POST request where the body of the request
	 * uses JSON.
	 */
	SM_BODY,

	/** The number of SubmissionMethods. */
	SM_NUM_METHODS
} SubmissionMethod;


/**
 * The configuration data for a WebService.
 *
 * This is used by reference services to call
 * web services.
 *
 * @extends ServiceData
 */
typedef struct WebServiceData
{
	/** The base ServiceData. */
	ServiceData wsd_base_data;

	/**
	 * The configuration from the reference service.
	 */
	json_t *wsd_config_p;

	/** The name of the WebService. */
	const char *wsd_name_s;

	/** The description of the WebService. */
	const char *wsd_description_s;

	/** The uri for more information about this WebService. */
	const char *wsd_info_uri_s;

	/** The ParameterSet for this WebService. */
	ParameterSet *wsd_params_p;

	/** This is used to store the data when building the submission uri. */
	ByteBuffer *wsd_buffer_p;

	/** The uri that the WebService will submit its query to. */
	const char *wsd_base_uri_s;

	/** The SubmissionMethod to use to submit the query. */
	SubmissionMethod wsd_method;

	/** The CurlTool used to send the request and receive the results. */
	CurlTool *wsd_curl_data_p;
} WebServiceData;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the requested SubmissionMethod from a reference service.
 *
 * @param op_json_p The JSON fragment for the reference service which will be used
 * to initialise the WebServiceData.
 * @return The SubmissionMethod.
 */
GRASSROOTS_SERVICE_API SubmissionMethod GetSubmissionMethod (const json_t *op_json_p);


/**
 * Initialise a WebServiceData.
 *
 * @param data_p The WebServiceData to initialise.
 * @param op_json_p The JSON fragment for the reference service which will be used
 * to initialise the WebServiceData.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API bool InitWebServiceData (WebServiceData * const data_p, json_t *op_json_p);


/**
 * Clear a WebServiceData and free any associated memory.
 *
 * @param data_p The WebServiceData to clear.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API void ClearWebServiceData (WebServiceData * const data_p);


/**
 * Make the request to the web service uri and store the response.
 *
 * @param data_p The WebServiceData to make the request for.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API bool CallCurlWebservice (WebServiceData *data_p);


/**
 * Add parameters to the HTTP GET request used by the given WebServiceData.
 *
 * @param data_p The WebServiceData to build the request for.
 * @param param_set_p The ParameterSet to use to build the request.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API bool AddParametersToGetWebService (WebServiceData *data_p, ParameterSet *param_set_p);


/**
 * Add parameters to the HTTP POST request used by the given WebServiceData.
 *
 * @param data_p The WebServiceData to build the request for.
 * @param param_set_p The ParameterSet to use to build the request.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API bool AddParametersToPostWebService (WebServiceData *data_p, ParameterSet *param_set_p);


/**
 * Add parameters to an HTTP POST request with a JSON fragment in its body used by the given WebServiceData.
 *
 * @param data_p The WebServiceData to build the request for.
 * @param param_set_p The ParameterSet to use to build the request.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof WebServiceData
 */
GRASSROOTS_SERVICE_API bool AddParametersToBodyWebService (WebServiceData *data_p, ParameterSet *param_set_p);


/**
 * Run the referred service with its WebServiceData and get the resultant ServiceJobSet results.
 *
 * @param service_p The Service that uses a WebServiceData.
 * @param param_set_p The ParameterSet to use.
 * @param credentials_p Any user credentials.
 * @return The resultant ServiceJobSet or <code>NULL</code> upon error.
 */
GRASSROOTS_SERVICE_API ServiceJobSet *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);


/**
 * Add a Parameter giving the choice of MatchTypes to a ParameterSet.
 *
 * @param data_p The WebServiceData to to use for configuration details.
 * @param param_set_p The ParameterSet to add the MatchType parameter to.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 */
GRASSROOTS_SERVICE_API bool AddMatchTypeParameter (WebServiceData *data_p, ParameterSet *param_set_p);


/**
 * Get the value of the MatchType parameter from a ParameterSet.
 *
 * @param param_set_p The ParameterSet to get the MatchType parameter from.
 * @return The MatchType parameter.
 */
GRASSROOTS_SERVICE_API MatchType GetMatchTypeParameterValue (ParameterSet * const param_set_p);


#ifdef __cplusplus
}
#endif


#endif /* WEB_SERVICE_UTIL_H_ */
