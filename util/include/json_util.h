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
#ifndef JSON_UTIL_H
#define JSON_UTIL_H


#include <stdio.h>

#include "grassroots_util_library.h"

#include "jansson.h"
#include "linked_list.h"
#include "string_linked_list.h"



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
#ifdef ALLOCATE_JSON_TAGS
	#define PREFIX GRASSROOTS_UTIL_API
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)	
#endif

PREFIX const char *ERROR_S VAL("error");

PREFIX const char *SERVER_MULTIPLE_PROVIDERS_S VAL("providers");
PREFIX const char *SERVER_PROVIDER_S VAL("provider");
PREFIX const char *PROVIDER_NAME_S VAL("name");
PREFIX const char *PROVIDER_DESCRIPTION_S VAL("description");
PREFIX const char *PROVIDER_URI_S VAL("url");

PREFIX const char *SERVERS_S VAL("servers");
PREFIX const char *SERVER_UUID_S VAL("server_uuid");
PREFIX const char *SERVER_NAME_S VAL("server_name");
PREFIX const char *SERVER_CONNECTION_TYPE_S VAL("server_connection");
PREFIX const char *SERVER_URI_S VAL("server_url");


PREFIX const char *SERVER_PAIRED_SERVICES_S VAL("paired_services");
PREFIX const char *SERVER_LOCAL_PAIRED_SERVCE_S VAL("local");
PREFIX const char *SERVER_REMOTE_PAIRED_SERVCE_S VAL("remote");

PREFIX const char *CONNECTION_RAW_S VAL("connection_raw");
PREFIX const char *CONNECTION_WEB_S VAL("connection_web");

PREFIX const char *SERVER_OPERATIONS_S VAL("operations");
PREFIX const char *OPERATION_ID_OLD_S VAL("operationId");
PREFIX const char *OPERATION_ID_S VAL("operation_id");
PREFIX const char *OPERATION_DESCRIPTION_S VAL("description");
PREFIX const char *OPERATION_INFORMATION_URI_S VAL("about_url");
PREFIX const char *OPERATION_ICON_PATH_S VAL("icon");
PREFIX const char *OPERATION_SYNCHRONOUS_S VAL("synchronous");

PREFIX const char *SERVICES_NAME_S VAL("services");
PREFIX const char *SERVICES_ID_S VAL("path");
PREFIX const char *SERVICES_DESCRIPTION_S VAL("description");
PREFIX const char *SERVICE_RUN_S VAL("run");
PREFIX const char *SERVICES_STATUS_S VAL("status");
PREFIX const char *SERVICES_STATUS_DEFAULT_S VAL("default");

PREFIX const char *SERVICE_NAME_S VAL("service");
PREFIX const char *SERVICE_UUID_S VAL("service_uuid");
PREFIX const char *SERVICE_STATUS_VALUE_S VAL("status");
PREFIX const char *SERVICE_STATUS_S VAL("status_text");
PREFIX const char *SERVICE_RESULTS_S VAL("results");
PREFIX const char *SERVICE_METADATA_S VAL("metadata");
PREFIX const char *SERVICE_JOBS_S VAL("jobs");


PREFIX const char *JOB_NAME_S VAL("name");
PREFIX const char *JOB_SERVICE_S VAL("service");
PREFIX const char *JOB_DESCRIPTION_S VAL("description");
PREFIX const char *JOB_RESULTS_S VAL("results");
PREFIX const char *JOB_ERRORS_S VAL("errors");
PREFIX const char *JOB_METADATA_S VAL("metadata");
PREFIX const char *JOB_UUID_S VAL("job_uuid");

PREFIX const char *PLUGIN_NAME_S VAL("plugin");

PREFIX const char *PARAM_SET_KEY_S VAL("parameter_set");
PREFIX const char *PARAM_SET_NAME_S VAL("name");
PREFIX const char *PARAM_SET_DESCRIPTION_S VAL("description");
PREFIX const char *PARAM_SET_PARAMS_S VAL("parameters");
PREFIX const char *PARAM_SET_GROUPS_S VAL("groups");

PREFIX const char *PARAM_NAME_S VAL("param");
PREFIX const char *PARAM_DISPLAY_NAME_S VAL("name");
PREFIX const char *PARAM_CONCISE_DEFINITION_S VAL("concise");
PREFIX const char *PARAM_DESCRIPTION_S VAL("description");
PREFIX const char *PARAM_STORE_S VAL("store");
PREFIX const char *PARAM_TAG_S VAL("tag");
PREFIX const char *PARAM_TYPE_S VAL("type");
PREFIX const char *PARAM_GRASSROOTS_TYPE_INFO_S VAL("grassroots_type");
PREFIX const char *PARAM_DEFAULT_VALUE_S  VAL("default");
PREFIX const char *PARAM_CURRENT_VALUE_S  VAL("current_value");
PREFIX const char *PARAM_OPTIONS_S  VAL("enum");
PREFIX const char *PARAM_GROUP_S VAL("group");
PREFIX const char *PARAM_MIN_S  VAL("min");
PREFIX const char *PARAM_MAX_S  VAL("max");
PREFIX const char *PARAM_LEVEL_S  VAL("level");
PREFIX const char *PARAM_ERRORS_S  VAL("errors");
PREFIX const char *PARAM_SERVER_ID_S  VAL("server_id");
PREFIX const char *PARAM_REMOTE_TAG_S  VAL("remote_tag");
PREFIX const char *PARAM_REMOTE_URI_S  VAL("remote_uri");
PREFIX const char *PARAM_REMOTE_S  VAL("remote_details");

PREFIX const char *SHARED_TYPE_DESCRIPTION_S  VAL("description");
PREFIX const char *SHARED_TYPE_VALUE_S  VAL("value");

PREFIX const char *RESOURCE_PROTOCOL_S  VAL("protocol");
PREFIX const char *RESOURCE_VALUE_S  VAL("value");
PREFIX const char *RESOURCE_DESCRIPTION_S  VAL("description");
PREFIX const char *RESOURCE_TITLE_S  VAL("title");
PREFIX const char *RESOURCE_DATA_S  VAL("data");

PREFIX const char *TAG_INPUT_NAME_S VAL("input");
PREFIX const char *TAG_OUTPUT_NAME_S VAL("output");

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



PREFIX const char *DRMAA_S VAL("drmaa");
PREFIX const char *DRMAA_PROGRAM_NAME_S VAL("program_name");
PREFIX const char *DRMAA_JOB_NAME_S VAL("job_name");
PREFIX const char *DRMAA_QUEUE_S VAL("queue");
PREFIX const char *DRMAA_WORKING_DIR_S VAL("working_dir");
PREFIX const char *DRMAA_OUTPUT_FILE_S VAL("output_file");
PREFIX const char *DRMAA_ID_S VAL("drmma_job_id");
PREFIX const char *DRMAA_OUT_ID_S VAL("drmma_job_out_id");
PREFIX const char *DRMAA_HOSTNAME_S VAL("host");
PREFIX const char *DRMAA_USERNAME_S VAL("user");
PREFIX const char *DRMAA_EMAILS_S VAL("emails");
PREFIX const char *DRMAA_ARGS_S VAL("args");
PREFIX const char *DRMAA_NUM_CORES_S VAL("num_cores");
PREFIX const char *DRMAA_MEM_USAGE_S VAL("mem");



PREFIX const char *KEYWORDS_QUERY_S VAL("query");



/**
 * A datatype for storing a json_t on a LinkedList.
 *
 * @see LinkedList
 */
typedef struct JsonNode
{
	/**
	 * The base List node
	 */
	ListItem jn_node;

	/** Pointer to the json object. */
	json_t *jn_json_p;
} JsonNode;



typedef struct FieldNode
{
	StringListNode fn_base_node;

	json_type fn_type;

} FieldNode;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Print a json fragment to a FILE pointer.
 *
 * @param out_f The FILE to print to.
 * @param json_p The json object to print.
 * @param prefix_s An optional string to precede the json output in the given FILE.
 * @return O if successful, less than 0 upon error.
 */
GRASSROOTS_UTIL_API int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s);


/**
 * Get the value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return The corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object.
 */
GRASSROOTS_UTIL_API const char *GetJSONString (const json_t *json_p, const char * const key_s);



/**
 * Get the newly-allocated value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return A newly-allocated copy of the corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object or failed to be copied. This value, if valid, should be freed with FreeCopiedString.
 * @see FreeCopiedString.
 */
GRASSROOTS_UTIL_API char *GetCopiedJSONString (const json_t *json_p, const char * const key_s);


/**
 * Get the integer value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool GetJSONInteger (const json_t *json_p, const char * const key_s, int *value_p);


/**
 * Get the long value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool GetJSONLong (const json_t *json_p, const char * const key_s, long *value_p);


/**
 * Get the double value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a double.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool GetJSONReal (const json_t *json_p, const char * const key_s, double *value_p);


/**
 * Get the boolean value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a boolean.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool GetJSONBoolean (const json_t *json_p, const char * const key_s, bool *value_p);


/**
 * Create and add an array of c-style strings to a json object.
 *
 * @param parent_p The json_t object to add the array to.
 * @param values_ss The array of c-style strings with a final element set to NULL.
 * @param child_key_s The key to use to add the json array to parent_p.
 * @return <code>true</code> if the json_array was created and added successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool AddStringArrayToJSON (json_t *parent_p, const char ** const values_ss, const char * const child_key_s);


/**
 * Created an array of c-style string of StringListNodes from a json array.
 *
 * @param array_p The json_t array to create the c-style strings arrayt from.
 * @return The array of c-style strings or <code>NULL</code> if array_p is
 * an empty array or if there was an error.
 */
GRASSROOTS_UTIL_API char **GetStringArrayFromJSON (const json_t * const array_p);


/**
 * Create and add a LinkedList of StringListNodes to a json object.
 *
 * @param parent_p The json_t object to add the array to.
 * @param values_ss The LinkedList of StringListNodes.
 * @param child_key_s The key to use to add the json array to parent_p.
 * @return <code>true</code> if the json_array was created and added successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool AddStringListToJSON (json_t *parent_p, LinkedList *values_p, const char * const child_key_s);


/**
 * Created a LinkedList of StringListNodes from a json array.
 *
 * @param array_p The json_t object to generate the list from.
 * @return The LinkedList of StringListNodes or <code>NULL</code> if array_p is
 * an empty array or if there was an error.
 */
GRASSROOTS_UTIL_API LinkedList *GetStringListFromJSON (const json_t * const array_p);


/**
 * Allocate a JsonNode that points to the given json object.
 *
 * @param json_p The json object for the JsonNode to point to.
 * @return A newly-allocated JsonNode or <code>NULL</code> upon error.
 * @memberof JsonNode
 * @see FreeJsonNode
 */
GRASSROOTS_UTIL_API JsonNode *AllocateJsonNode (json_t *json_p);


/**
 * Free a JsonNode.
 *
 * @param node_p The JsonNode to free. The number of references to the
 * associated json object will be decremented by 1.
 * @memberof JsonNode
 */
GRASSROOTS_UTIL_API void FreeJsonNode (ListItem *node_p);


/**
 * Allocate a FieldNode that points to the given json object.
 *
 * @param name_s The json object for the newly to point to.
 * @return A newly-allocated newly or <code>NULL</code> upon error.
 * @memberof FieldNode
 * @see FreeFieldNode
 */
GRASSROOTS_UTIL_API FieldNode *AllocateFieldNode (const char *name_s, const MEM_FLAG mf, json_type field_type);

/**
 * Free a FieldNode.
 *
 * @param node_p The FieldNode to free.
 * @memberof FieldNode
 */
GRASSROOTS_UTIL_API void FreeFieldNode (ListItem *node_p);


GRASSROOTS_UTIL_API json_t *LoadJSONConfig (const char * const filename_s);


GRASSROOTS_UTIL_API json_t *ConvertTabularDataToJSON (char *data_s, const char column_delimiter, const char row_delimiter, json_type (*get_type_fn) (const char *name_s, const void * const data_p), const void * const type_data_p);


GRASSROOTS_UTIL_LOCAL json_t *GetJSONFromString (const char * const value_s, json_type field_type);


GRASSROOTS_UTIL_API json_t *ConvertRowToJSON (char *row_s, LinkedList *headers_p, const char delimiter);



GRASSROOTS_UTIL_API bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s);



/**
 * Print a json_t object to the logging stream
 *
 * @param json_p The json_t object to print.
 * @param prefix_s An optional message to print to the log prior to parsing the value's reference counts.
 * If this is <code>NULL</code>, then no message will be printed.
 * @param level The logging level to use.
 * @param filename_s The filename of the source file calling this method.
 * @param line_number The line number in the source file which called this method.
 * @see PrintLog
 */
GRASSROOTS_UTIL_API void PrintJSONToLog (const json_t *json_p, const char * const prefix_s, const uint32 level, const char *filename_s, const int line_number);


/**
 * Is a json_t NULL or empty?
 *
 * @param json_p The json_t object to test.
 * @return <code>true</code> if the json_t is NULL, an empty array, an empty object or
 * an empty string, <code>false</code> otherwise.
 * @see IsStringEmpty
 */
GRASSROOTS_UTIL_API bool IsJSONEmpty (const json_t *json_p);



/**
 * Print the reference counts for all of the entries in a json_t object to the logging stream
 *
 * @param value_p The json_t object to print.
 * @param initial_s An optional message to print to the log prior to parsing the value's reference counts.
 * If this is <code>NULL</code>, then no message will be printed.
 * @param log_level The logging level to use.
 * @param filename_s The filename of the source file calling this method.
 * @param line_number The line number in the source file which called this method.
 * @see PrintLog
 */
GRASSROOTS_UTIL_API void PrintJSONRefCounts (const json_t * const value_p, const char *initial_s, const uint32 log_level, const char * const filename_s, const int line_number);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_UTIL_H */
