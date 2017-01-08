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
 *
 *
 *  Created on: 6 Jan 2017
 *      Author: billy
 *
 * @file schema_keys.h
 * @brief The keys for the Grassroots Schema.
 * The messages sent between the Grassroots Servers and Clients are JSON-based
 * and the various available keys are documented in this file.
 *
 * The keys are available as constant strings where the first word or phrase
 * shows the object which the key represents a member of. For instance ::PARAM_CURRENT_VALUE_S
 * and ::PARAM_SET_NAME_S refer to the current value of a Parameter and the name of
 * a ParameterSet respectively.
 *
 * Under normal usage you will not need to explicitly do the conversion of a JSON description to the
 * object that it represents in a step-by-step way as each object will have functions to do this e.g.
 * a ParameterSet has GetParameterSetAsJSON() to get the JSON message for a given ParameterSet and
 * CreateParameterSetFromJSON() to convert back from the JSON to a ParameterSet.
 */

#ifndef SHARED_SRC_UTIL_INCLUDE_SCHEMA_KEYS_H_
#define SHARED_SRC_UTIL_INCLUDE_SCHEMA_KEYS_H_


/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_JSON_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * json_util.c.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_JSON_TAGS
	#define PREFIX GRASSROOTS_UTIL_API
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


#ifdef __cplusplus
extern "C"
{
#endif


	/**
	 *  nosubgrouping
	 *
	 */

	/**
	 * The JSON key for the errors object of the server response.
	 *
	 * @ingroup SCHEMA_ERROR_GROUP
	 */
	PREFIX const char *ERROR_S VAL("error");

	PREFIX const char *HEADER_S VAL("header");

	PREFIX const char *REQUEST_S VAL("request");
	PREFIX const char *REQUEST_VERBOSE_S VAL("verbose");


	/**
	 * @name Schema definitions for specifying the Grassroots schema version.
	 */
	/* Start of doxygen member group */
	/**@{*/

	/**
	 * The JSON key for the schema object of the server response.
	 * This appears at the top level of JSON request objects.
	 */
	PREFIX const char *SCHEMA_S VAL("schema");

	/**
	 * The JSON key for specifying the major version of the schema.
	 * This is a child of @link SCHEMA_S @endlink
	 */
	PREFIX const char *VERSION_MAJOR_S VAL("major");

	/**
	 * The JSON key for specifying the minor version of the schema.
	 * This is a child of @link SCHEMA_S @endlink
	 */
	PREFIX const char *VERSION_MINOR_S VAL("minor");

	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying the Service Providers.
	 * @ingroup SCHEMA_SERVICE_PROVIDERS_GROUP
	 */
	/* Start of doxygen member group */
	/**@{*/
	/**
	 * @brief The JSON key for specifying an array of external Grassroots Servers.
	 *
	 * The objects contained in this array each have ::PROVIDER_NAME_S,
	 * ::PROVIDER_DESCRIPTION_S and ::PROVIDER_URI_S keys to describe each of the
	 * Servers.
	 */
	PREFIX const char *SERVER_MULTIPLE_PROVIDERS_S VAL("providers");

	/**
	 * @brief The JSON key for specifying details about this Grassroots Server.
	 *
	 * The child key-value pairs of this object are ::PROVIDER_NAME_S,
	 * ::PROVIDER_DESCRIPTION_S and ::PROVIDER_URI_S keys to describe each of the
	 * Servers.
	 */
	PREFIX const char *SERVER_PROVIDER_S VAL("provider");

	/**
	 * @brief The JSON key for the name of the Server to show to Clients and other connected
	 * Servers.
	 *
	 * @see ::SERVER_MULTIPLE_PROVIDERS_S
	 * @see :: SERVER_PROVIDER_S
	 */
	PREFIX const char *PROVIDER_NAME_S VAL("name");

	/**
	 * @brief The JSON key for the description of the Server to show to Clients and other connected
	 * Servers.
	 *
	 * @see ::SERVER_MULTIPLE_PROVIDERS_S
	 * @see :: SERVER_PROVIDER_S
	 */
	PREFIX const char *PROVIDER_DESCRIPTION_S VAL("description");

	/**
	 * @brief The JSON key for the URI that accepts Grassroots message on the Server.
	 *
	 * @see ::SERVER_MULTIPLE_PROVIDERS_S
	 * @see :: SERVER_PROVIDER_S
	 */
	PREFIX const char *PROVIDER_URI_S VAL("uri");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying Servers. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SERVERS_S VAL("servers");
	PREFIX const char *SERVER_UUID_S VAL("server_uuid");
	PREFIX const char *SERVER_NAME_S VAL("server_name");
	PREFIX const char *SERVER_CONNECTION_TYPE_S VAL("server_connection");
	PREFIX const char *SERVER_URI_S VAL("server_uri");
	/* End of doxygen member group */
	/**@}*/


	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SERVER_PAIRED_SERVICES_S VAL("paired_services");
	PREFIX const char *SERVER_LOCAL_PAIRED_SERVCE_S VAL("local");
	PREFIX const char *SERVER_REMOTE_PAIRED_SERVCE_S VAL("remote");
	/**@}*/

	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *CONNECTION_RAW_S VAL("connection_raw");
	PREFIX const char *CONNECTION_WEB_S VAL("connection_web");
	/* End of doxygen member group */
	/**@}*/

	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SERVER_OPERATIONS_S VAL("operations");
	PREFIX const char *OPERATION_ID_OLD_S VAL("operationId");
	PREFIX const char *OPERATION_ID_S VAL("operation_id");
	PREFIX const char *OPERATION_S VAL("operation");
	PREFIX const char *OPERATION_DESCRIPTION_S VAL("description");
	PREFIX const char *OPERATION_INFORMATION_URI_S VAL("about_url");
	PREFIX const char *OPERATION_ICON_PATH_S VAL("icon");
	PREFIX const char *OPERATION_SYNCHRONOUS_S VAL("synchronous");
	/* End of doxygen member group */
	/**@}*/

	/** @name The Schema definitions for specifying Services. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SERVICES_NAME_S VAL("services");
	PREFIX const char *SERVICES_ID_S VAL("path");
	PREFIX const char *SERVICES_DESCRIPTION_S VAL("description");
	PREFIX const char *SERVICE_RUN_S VAL("start_service");
	PREFIX const char *SERVICES_STATUS_S VAL("status");
	PREFIX const char *SERVICES_STATUS_DEFAULT_S VAL("default");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying Services. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SERVICE_NAME_S VAL("service_name");
	PREFIX const char *SERVICE_UUID_S VAL("service_uuid");
	PREFIX const char *SERVICE_STATUS_VALUE_S VAL("status");
	PREFIX const char *SERVICE_STATUS_S VAL("status_text");
	PREFIX const char *SERVICE_RESULTS_S VAL("results");
	PREFIX const char *SERVICE_METADATA_S VAL("metadata");
	PREFIX const char *SERVICE_JOBS_S VAL("jobs");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying LinkedServices. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *LINKED_SERVICES_S VAL("linked_services");
	PREFIX const char *MAPPED_PARAM_INPUT_S VAL("input");
	PREFIX const char *MAPPED_PARAM_OUTPUT_S VAL("output");
	PREFIX const char *MAPPED_PARAM_REQUIRED_S VAL("required");
	PREFIX const char *MAPPED_PARAMS_ROOT_S VAL("input_root");
	PREFIX const char *MAPPED_PARAM_MULTI_VALUED_S VAL("multiple");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying ServiceJobs. */
	/* Start of doxygen member group */
	/**@{*/

	/**
	 * The JSON key for the name of a ServiceJob.
	 */
	PREFIX const char *JOB_NAME_S VAL("name");

	/**
	 * The JSON key of the Service name for a ServiceJob.
	 */
	PREFIX const char *JOB_SERVICE_S VAL("service_name");
	PREFIX const char *JOB_DESCRIPTION_S VAL("description");

	PREFIX const char *JOB_RESULTS_S VAL("results");
	PREFIX const char *JOB_ERRORS_S VAL("errors");
	PREFIX const char *JOB_METADATA_S VAL("metadata");
	PREFIX const char *JOB_UUID_S VAL("job_uuid");
	PREFIX const char *JOB_REMOTE_URI_S  VAL("remote_uri");
	PREFIX const char *JOB_REMOTE_UUID_S  VAL("remote_uuid");
	/* End of doxygen member group */
	/**@}*/


	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *PLUGIN_NAME_S VAL("plugin");
	/* End of doxygen member group */
	/**@}*/


	/** @name  The Schema definitions for specifying ParameterSets. */
	/* Start of doxygen member group */
	/**@{*/

	/**
	 * The JSON key for the ParameterSet object of the server response.
	 */
	PREFIX const char *PARAM_SET_KEY_S VAL("parameter_set");

	/**
	 * The JSON key for the name of ParameterSet object of the server response.
	 * This is a child of @link PARAM_SET_KEY_S @endlink
	 */
	PREFIX const char *PARAM_SET_NAME_S VAL("name");

	/**
	 * The JSON key for the description of ParameterSet object of the server response.
	 * This is a child of @link PARAM_SET_KEY_S @endlink
	 */
	PREFIX const char *PARAM_SET_DESCRIPTION_S VAL("description");

	/**
	 * The JSON key for the child object detailing all of the Parameters within a ParameterSet.
	 *
	 * This is a child of @link PARAM_SET_KEY_S @endlink
	 */
	PREFIX const char *PARAM_SET_PARAMS_S VAL("parameters");

	/**
	 * The JSON key for the child object containing the names of all of the
	 * ParameterGroups within a ParameterSet.

	 * This is a child of @link PARAM_SET_KEY_S @endlink
	 */
	PREFIX const char *PARAM_SET_GROUPS_S VAL("groups");
	/* End of doxygen member group */
	/**@}*/


	/**
	 * @name The Schema definitions for specifying Parameters
	 * Each Parameter is an object in a json array value which
	 * is a child of @link PARAM_SET_PARAMS_S @endlink.
	 */
	/* Start of doxygen member group */
	/**@{*/

	/**
	 * The JSON key for the name of Parameter.
	 */
	PREFIX const char *PARAM_NAME_S VAL("param");

	/**
	 * The JSON key for the display name of Parameter.
	 */
	PREFIX const char *PARAM_DISPLAY_NAME_S VAL("name");


	PREFIX const char *PARAM_CONCISE_DEFINITION_S VAL("concise");

	/**
	 * The JSON key for the description of Parameter.
	 */
	PREFIX const char *PARAM_DESCRIPTION_S VAL("description");

	PREFIX const char *PARAM_STORE_S VAL("store");

	PREFIX const char *PARAM_REMOTE_NAME_S VAL("name");


	PREFIX const char *PARAM_TYPE_S VAL("type");
	PREFIX const char *PARAM_GRASSROOTS_TYPE_INFO_S VAL("grassroots_type_number");
	PREFIX const char *PARAM_GRASSROOTS_TYPE_INFO_TEXT_S VAL("grassroots_type");
	PREFIX const char *PARAM_GRASSROOTS_S VAL("parameter_type");
	PREFIX const char *PARAM_COMPOUND_TEXT_S VAL("text");
	PREFIX const char *PARAM_COMPOUND_VALUE_S VAL("value");

	/**
	 * The JSON key for the default value of Parameter.
	 */
	PREFIX const char *PARAM_DEFAULT_VALUE_S  VAL("default_value");

	/**
	 * The JSON key for the current value of Parameter.
	 */
	PREFIX const char *PARAM_CURRENT_VALUE_S  VAL("current_value");

	PREFIX const char *PARAM_OPTIONS_S  VAL("enum");

	/**
	 * The JSON key for the name of the ParameterGroup for this Parameter.
	 */
	PREFIX const char *PARAM_GROUP_S VAL("group");

	/**
	 * The JSON key for specifying the default visibility of a ParameterGroup
	 * to a user.
	 */
	PREFIX const char *PARAM_GROUP_VISIBLE_S VAL("visible");

	/**
	 * The JSON key for specifying the minimum value that a numeric Parameter
	 * can take.
	 */
	PREFIX const char *PARAM_MIN_S  VAL("min");


	/**
	 * The JSON key for specifying the maximum value that a numeric Parameter
	 * can take.
	 */
	PREFIX const char *PARAM_MAX_S  VAL("max");

	/**
	 * The JSON key for specifying the level of the Parameter.
	 */
	PREFIX const char *PARAM_LEVEL_S  VAL("level");
	PREFIX const char *PARAM_LEVEL_TEXT_S  VAL("level_text");


	/**
	 * The JSON key for specifying that a Parameter is suitable for
	 * all levels.
	 */

	PREFIX const char *PARAM_LEVEL_TEXT_ALL_S  VAL("all");

	/**
	 * The JSON key for specifying that a Parameter to be available
	 * for a client at the beginner level.
	 */
	PREFIX const char *PARAM_LEVEL_TEXT_BASIC_S  VAL("beginner");


	/**
	 * The JSON key for specifying that a Parameter to be available
	 * for a client at the intermediate level.
	 */
	PREFIX const char *PARAM_LEVEL_TEXT_INTERMEDIATE_S  VAL("intermediate");


	/**
	 * The JSON key for specifying that a Parameter to be available
	 * for a client at the advanced level.
	 */
	PREFIX const char *PARAM_LEVEL_TEXT_ADVANCED_S  VAL("advanced");
	PREFIX const char *PARAM_ERRORS_S  VAL("errors");
	PREFIX const char *PARAM_SERVER_ID_S  VAL("server_id");
	PREFIX const char *PARAM_REMOTE_URI_S  VAL("remote_uri");
	PREFIX const char *PARAM_REMOTE_S  VAL("remote_details");
	PREFIX const char *KEYWORDS_QUERY_S VAL("query");
	/* End of doxygen member group */
	/**@}*/


	/** @name  The Schema definitions for specifying SharedType values. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *SHARED_TYPE_DESCRIPTION_S  VAL("description");
	PREFIX const char *SHARED_TYPE_VALUE_S  VAL("value");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying Resources.
	 * The JSON keys for defining a Resource
	 */
	/**@{*/

	/**
	 * The JSON key for specifying a Resource object.
	 */
	PREFIX const char *RESOURCE_S  VAL("resource");

	/**
	 * The JSON key for specifying the protocol for a Resource.
	 */
	PREFIX const char *RESOURCE_PROTOCOL_S  VAL("protocol");

	/**
	 * The JSON key for specifying the value of a Resource.
	 * This could be the filename, uri, etc. depending upon
	 * the protocol for this Resource.
	 */
	PREFIX const char *RESOURCE_VALUE_S  VAL("value");

	/**
	 * The JSON key for specifying the a user-friendly description
	 * of a Resource.
	 */
	PREFIX const char *RESOURCE_DESCRIPTION_S  VAL("description");

	/**
	 * The JSON key for specifying a title to display to Clients
	 * and external Servers, rather than show the name.
	 */
	PREFIX const char *RESOURCE_TITLE_S  VAL("title");


	/**
	 * The JSON key for specifying any inline data for a Resource.
	 */
	PREFIX const char *RESOURCE_DATA_S  VAL("data");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying Parameters.
	 * */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *TAG_INPUT_NAME_S VAL("input");
	PREFIX const char *TAG_OUTPUT_NAME_S VAL("output");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying Parameters. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *CREDENTIALS_S VAL("credentials");
	PREFIX const char *CREDENTIALS_NAME_S VAL("name");
	PREFIX const char *CREDENTIALS_USERNAME_S VAL("user");
	PREFIX const char *CREDENTIALS_PASSWORD_S VAL("pass");
	PREFIX const char *CREDENTIALS_UUID_S VAL("user_uuid");
	PREFIX const char *CREDENTIALS_TOKEN_KEY_S VAL("token_key");
	PREFIX const char *CREDENTIALS_TOKEN_SECRET_S VAL("token_secret");
	PREFIX const char *CREDENTIALS_APP_KEY_S VAL("app_key");
	PREFIX const char *CREDENTIALS_APP_SECRET_S VAL("app_secret");
	PREFIX const char *CREDENTIALS_ENCRYPTION_METHOD_S VAL("encrypt_method");
	PREFIX const char *CREDENTIALS_ENCRYPTION_KEY_S VAL("encrypt_key");
	/* End of doxygen member group */
	/**@}*/


	/** @name The Schema definitions for specifying DRMAA details. */
	/* Start of doxygen member group */
	/**@{*/
	PREFIX const char *DRMAA_S VAL("drmaa");
	PREFIX const char *DRMAA_PROGRAM_NAME_S VAL("program_name");
	PREFIX const char *DRMAA_JOB_NAME_S VAL("job_name");
	PREFIX const char *DRMAA_QUEUE_S VAL("queue");
	PREFIX const char *DRMAA_WORKING_DIR_S VAL("working_dir");
	PREFIX const char *DRMAA_OUTPUT_FILE_S VAL("output_file");
	PREFIX const char *DRMAA_ID_S VAL("drmma_job_id");
	PREFIX const char *DRMAA_UUID_S VAL("drmma_grassroots_uuid");
	PREFIX const char *DRMAA_OUT_ID_S VAL("drmma_job_out_id");
	PREFIX const char *DRMAA_HOSTNAME_S VAL("host");
	PREFIX const char *DRMAA_USERNAME_S VAL("user");
	PREFIX const char *DRMAA_EMAILS_S VAL("emails");
	PREFIX const char *DRMAA_ARGS_S VAL("args");
	PREFIX const char *DRMAA_NUM_CORES_S VAL("num_cores");
	PREFIX const char *DRMAA_MEM_USAGE_S VAL("mem");
	/* End of doxygen member group */
	/**@}*/



#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_UTIL_INCLUDE_SCHEMA_KEYS_H_ */
