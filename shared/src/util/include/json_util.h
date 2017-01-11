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
#ifndef JSON_UTIL_H
#define JSON_UTIL_H


#include <stdio.h>

#include "grassroots_util_library.h"

#include "jansson.h"
#include "linked_list.h"
#include "string_linked_list.h"
#include "schema_keys.h"


/**
 * A datatype for storing a json_t on a LinkedList.
 *
 * @see LinkedList
 * @extends ListItem
 *
 * @ingroup utility_group
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


/**
 * A datatype for storing a value as a string along with the datatype to
 * convert it to.
 *
 * This is used when reading in data where all the values are stored as
 * strings regardless of their actual datatype e.g.
 *
 * 	"score": "10"
 *
 * where score is actually an integer. This datatype is to allow the conversion
 * back to the correct type.
 *
 * @extends StringListNode
 *
 * @ingroup utility_group
 */
typedef struct FieldNode
{
	/** The base node. */
	StringListNode fn_base_node;

	/** This defines the type of the data. */
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
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s);


/**
 * Get the value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return The corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API const char *GetJSONString (const json_t *json_p, const char * const key_s);



/**
 * Get the newly-allocated value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return A newly-allocated copy of the corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object or failed to be copied. This value, if valid, should be freed with FreeCopiedString().
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API char *GetCopiedJSONString (const json_t *json_p, const char * const key_s);


/**
 * Get the integer value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool GetJSONInteger (const json_t *json_p, const char * const key_s, int *value_p);


/**
 * Get the long value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool GetJSONLong (const json_t *json_p, const char * const key_s, long *value_p);


/**
 * Get the double value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a double.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool GetJSONReal (const json_t *json_p, const char * const key_s, double *value_p);


/**
 * Get the boolean value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a boolean.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool GetJSONBoolean (const json_t *json_p, const char * const key_s, bool *value_p);


/**
 * Create and add an array of c-style strings to a json object.
 *
 * @param parent_p The json_t object to add the array to.
 * @param values_ss The array of c-style strings with a final element set to NULL.
 * @param child_key_s The key to use to add the json array to parent_p.
 * @return <code>true</code> if the json_array was created and added successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool AddStringArrayToJSON (json_t *parent_p, const char ** const values_ss, const char * const child_key_s);


/**
 * Created an array of c-style string of StringListNodes from a json array.
 *
 * @param array_p The json_t array to create the c-style strings array from.
 * @return The array of c-style strings or <code>NULL</code> if array_p is
 * an empty array or if there was an error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API char **GetStringArrayFromJSON (const json_t * const array_p);


/**
 * Create and add a LinkedList of StringListNodes to a json object.
 *
 * @param parent_p The json_t object to add the array to.
 * @param values_p The LinkedList of StringListNodes.
 * @param child_key_s The key to use to add the json array to parent_p.
 * @return <code>true</code> if the json_array was created and added successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool AddStringListToJSON (json_t *parent_p, LinkedList *values_p, const char * const child_key_s);


/**
 * Created a LinkedList of StringListNodes from a json array.
 *
 * @param array_p The json_t object to generate the list from.
 * @return The LinkedList of StringListNodes or <code>NULL</code> if array_p is
 * an empty array or if there was an error.
 *
 * @ingroup utility_group
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
 * @param mf The MEM_FLAG used to store the name of this FieldNode.
 * @param field_type The type of JSON object that this FieldNode will store.
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


/**
 * Load a JSON file
 *
 * @param filename_s The filename to load.
 * @return The resultant JSON fragment or <code>NULL</code> upon errors
 * such as the file being unable to be read or not a JSON file.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API json_t *LoadJSONFile (const char * const filename_s);


/**
 * Convert a string of tabular data into a JSON array using the given column headings.
 *
 * @param data_s The string of data to convert.
 * @param column_delimiter The character used to separate the data into columns.
 * @param row_delimiter The character used to separate the data into rows.
 * @param headers_p A LinkedList of FieldNodes where specifying the headers for each column of data.
 * @return The tabular data as a JSON array with each child object containing the data for a row
 * with the keys being the header values or <code>NULL</code> upon error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API json_t *ConvertTabularDataToJSON (char *data_s, const char column_delimiter, const char row_delimiter, LinkedList *headers_p);


/**
 * Convert a string of tabular data into a JSON array. This effectively is a wrapper for calling
 * GetTabularHeaders and ConvertTabularDataToJSON.
 *
 * @param data_s The string of data to convert.
 * @param column_delimiter The character used to separate the data into columns.
 * @param row_delimiter The character used to separate the data into rows.
 * @param get_type_fn A function used to determine the datatype for each of the header values.
 * If this is <code>NULL</code>, then each value will be treated as a string.
 * @param type_data_p If get_type_fn needs any extra data, it can be passed in here.
 * @return The tabular data as a JSON array with each child object containing the data for a row
 * with the keys being the header values or <code>NULL</code> upon error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API json_t *ConvertTabularDataWithHeadersToJSON (char *data_s, const char column_delimiter, const char row_delimiter, json_type (*get_type_fn) (const char *name_s, const void * const data_p), const void * const type_data_p);


/**
 * Get the column headers from the first row of a tabular data variable.
 *
 * @param data_ss A pointer to the tabular data. If the headers get retrieved successfully
 * then the value that this points to will get incremented to the first row of data.
 * @param column_delimiter The character used to separate the data into columns.
 * @param row_delimiter The character used to separate the data into rows.
 * @param get_type_fn A function used to determine the datatype for each of the header values.
 * If this is <code>NULL</code>, then each value will be treated as a string.
 * @param type_data_p If get_type_fn needs any extra data, it can be passed in here.
 * @return A LinkedList of FieldNodes with each one containing a header value or
 * <code>NULL</code> upon error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API LinkedList *GetTabularHeaders (char **data_ss,  const char column_delimiter, const char row_delimiter, json_type (*get_type_fn) (const char *name_s, const void * const data_p), const void * const type_data_p);


/**
 * Create a JSON object to store a given value in the fiven format.
 *
 * For instance, if the value is "6" and the given field_type is JSON_INTEGER
 * a json_int with the value of 6 will be created.
 *
 * @param value_s The string to derive the value from.
 * @param field_type The json_type to convert the value to.
 * @return The newly-created JSON object containing the value or <code>NULL</code>
 * upon error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_LOCAL json_t *GetJSONFromString (const char *value_s, json_type field_type);


/**
 * Create a JSON object from a delimited string of data.
 *
 * @param row_s The delimited data for a single row.
 * @param headers_p The LinkedList of FieldNodes defining the column titles.
 * @param delimiter The column delimiter.
 * @return The JSON object containing the data from headers_p as the keys and the data
 * from row_s as the values or <code>NULL</code> upon error.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API json_t *ConvertRowToJSON (char *row_s, LinkedList *headers_p, const char delimiter);


/**
 * Add a string key-value pair to a JSON object only if the value is not NULL
 *
 * @param parent_p The JSON object that the key-value pair will be added to.
 * @param key_s The key to use.
 * @param value_s The value to use.
 * @return <code>true</code> if value_s is not <code>NULL</code> and the key-value pair were added successfully
 * or if value_s is <code>NULL</code>. If value_s points to valid data and the key-value pair fails to get added,
 * <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s);


/**
 * Print a json_t object to the logging stream
 *
 * @param json_p The json_t object to print.
 * @param level The logging level to use.
 * @param filename_s The filename of the source file calling this method.
 * @param line_number The line number in the source file which called this method.
 * @param message_s A char * using the same format as printf, etc.
 * @return The number of characters written or a negative number upon error.
 * @see PrintLog
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API int PrintJSONToLog (const uint32 level, const char *filename_s, const int line_number, const json_t *json_p, const char *message_s, ...);


/**
 * Print a json_t object to the error stream
 *
 * @param json_p The json_t object to print.
 * @param level The logging level to use.
 * @param filename_s The filename of the source file calling this method.
 * @param line_number The line number in the source file which called this method.
 * @param message_s A char * using the same format as printf, etc.
 * @return The number of characters written or a negative number upon error.
 * @see PrintErrors
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API int PrintJSONToErrors (const uint32 level, const char *filename_s, const int line_number, const json_t *json_p, const char *message_s, ...);



/**
 * Is a json_t <code>NULL</code> or empty?
 *
 * @param json_p The json_t object to test.
 * @return <code>true</code> if the json_t is <code>NULL</code>, an empty array, an empty object or
 * an empty string, <code>false</code> otherwise.
 * @see IsStringEmpty
 *
 * @ingroup utility_group
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
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API void PrintJSONRefCounts (const uint32 log_level, const char * const filename_s, const int line_number, const json_t * const value_p, const char *initial_s);


/**
 * Set the boolean value of a JSON object.
 *
 * @param json_p The JSON value to get the boolean value from.
 * @param value_p Pointer to where the boolean value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a boolean type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool SetBooleanFromJSON (const json_t *json_p, bool *value_p);


/**
 * Get the real value of a JSON object.
 *
 * @param json_p The JSON value to get the real value from.
 * @param value_p Pointer to where the real value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a real type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool SetRealFromJSON (const json_t *json_p, double *value_p);


/**
 * Get the integer value of a JSON object.
 *
 * @param json_p The JSON value to get the integer value from.
 * @param value_p Pointer to where the integer value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a integer type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool SetIntegerFromJSON (const json_t *json_p, int32 *value_p);


/**
 * Get the long value of a JSON object.
 *
 * @param json_p The JSON value to get the long value from.
 * @param value_p Pointer to where the long value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a long type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool SetLongFromJSON (const json_t *json_p, int64 *value_p);


/**
 * Get the string value of a JSON object.
 *
 * @param json_p The JSON value to get the boolean value from.
 * @param value_ss Pointer to where the string value will be copied to if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved and copied. If
 * the JSON value is not a string type or the memory for copying the string value
 * could not be allocated then <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool SetStringFromJSON (const json_t *json_p, char **value_ss);


/**
 * Get a descendant JSON object from another using a given selector
 *
 * @param input_p The JSON object to get the value from
 * @param compound_s The selector value using a dot notation. For example "foo.bar" would
 * search for a "bar" child key belonging to a "foo" child object of the input value.
 * @return The found JSON object or <code>NULL</code> if it could not be found.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API json_t *GetCompoundJSONObject (json_t *input_p, const char * const compound_s);


/**
 * Make a copy of given key-value pair from one JSON object to another.
 *
 * The value is a string.
 *
 * @param src_p The JSON object to get the value from.
 * @param dest_p The JSON object to copy the values to.
 * @param key_s The key to search for in the input JSON and uses as the key in the output JSON.
 * @param optional_flag If <code>true</code> the function will return <code>true</code> if the
 * key was missing in the input object. If <code>false</code> then this will make the function
 * return <code>false</code> when the input object does not contain the key.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool CopyJSONKeyStringValuePair (const json_t *src_p, json_t *dest_p, const char * const key_s, bool optional_flag);




/**
 * Make a copy of given key-value pair from one JSON object to another.
 *
 * The value is an integer.
 *
 * @param src_p The JSON object to get the value from.
 * @param dest_p The JSON object to copy the values to.
 * @param key_s The key to search for in the input JSON and uses as the key in the output JSON.
 * @param optional_flag If <code>true</code> the function will return <code>true</code> if the
 * key was missing in the input object. If <code>false</code> then this will make the function
 * return <code>false</code> when the input object does not contain the key.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool CopyJSONKeyIntegerValuePair (const json_t *src_p, json_t *dest_p, const char * const key_s, bool optional_flag);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_UTIL_H */
