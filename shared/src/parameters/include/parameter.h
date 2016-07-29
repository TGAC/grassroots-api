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
/**@file parameter.h
*/

#ifndef PARAMETER_H
#define PARAMETER_H

#include "jansson.h"
#include "uuid/uuid.h"

#include "linked_list.h"
#include "grassroots_params_library.h"
#include "tags.h"
#include "data_resource.h"
#include "hash_table.h"


#include "remote_parameter_details.h"


/******* FORWARD DECLARATION *******/
struct Parameter;
struct ServiceData;

/**
 * The different types that a Parameter can take.
 */
typedef enum ParameterType
{
	/** A boolean parameter */
	PT_BOOLEAN,

	/** A 32-bit integer */
	PT_SIGNED_INT,

	/** A non-negative 32-bit integer */
	PT_UNSIGNED_INT,

	/** A real number */
	PT_SIGNED_REAL,

	/** An unsigned real number */
	PT_UNSIGNED_REAL,

	/** A c-style string */
	PT_STRING,

	/** An output filename string */
	PT_FILE_TO_WRITE,

	/** An input filename string*/
	PT_FILE_TO_READ,

	/** A directory string */
	PT_DIRECTORY,

	/** A single 1-byte character */
	PT_CHAR,

	/**
	 * A sensitive c-style string that shouldn't
	 * be displayed explicitly to the user.
	 */
	PT_PASSWORD,

	/**
	 * A value that a Service can use without any other parameters
	 * being set to produce results.
	 */
	PT_KEYWORD,


	/**
	 * A potentially large c-style string. This is used by clients to determine
	 * the appropriate editor to show to the user. E.g. a multi-line text bos
	 * as opposed to a single-line text box for a PT_STRING.
	 */
	PT_LARGE_STRING,

	/**
	 * A JSON fragment.
	 */
	PT_JSON,

	/**
	 * A c-style string of tabular data.
	 */
	PT_TABLE,

	/** The number of possible ParameterType values. */
	PT_NUM_TYPES
} ParameterType;



/**
 * The ParameterLevel defines the level that a user
 * should be to adjust the Parameter. The level of
 * Parameters that a user wishes to work with can
 * be adjusted within the user interface.
 */
typedef uint8 ParameterLevel;

/**
 * The value of a ParameterLevel for Parameters that
 * are considered the most simple options.
 */
#define PL_BASIC (1)


/**
 * The value of a ParameterLevel for Parameters that
 * are considered suitable for more knowledgeable users.
 */
#define PL_INTERMEDIATE (1 << 1)

/**
 * The value of a ParameterLevel for Parameters that
 * are considered the options that require the most expertise.
 */
#define PL_ADVANCED (1 << 2)



/**
 * The value of a ParameterLevel for Parameters that
 * are considered the options suitable for all levels
 */
#define PL_ALL (PL_BASIC | PL_INTERMEDIATE | PL_ADVANCED)


/**
 * A datatype to store values for a Parameter.
 * Use the ParameterType to access the correct
 * value.
 */
typedef union SharedType
{
	/** A Boolean value */
	bool st_boolean_value;

	/** A signed integer */
	int32 st_long_value;

	/** An unsigned integer */
	uint32 st_ulong_value;

	/** A real number */
	double64 st_data_value;

	/** A c-style string */
	char *st_string_value_s;

	/** A single character */
	char st_char_value;

	/** A Resource */
	Resource *st_resource_value_p;

	/** A LinkedList of multiple values */
	LinkedList *st_multiple_values_p;

	/** A JSON fragment */
	json_t *st_json_p;
} SharedType;


/**
 * A datatype allowing a SharedType to be stored
 * on a LinkedList.
 *
 * @extends ListItem
 */
typedef struct SharedTypeNode
{
	/** The basic ListItem */
	ListItem stn_node;

	/** The SharedType value */
	SharedType stn_value;
} SharedTypeNode;


/**
 * A ParameterMultiOption is used when a Parameter
 * wants to constrain the possible values to one from
 * a fixed set.
 * @see ParameterMultiOptionArray
 */
typedef struct ParameterMultiOption
{
	/** The user-friendly description for this value */
	char *pmo_description_s;

	/** The internal value for this option */
	SharedType pmo_value;
} ParameterMultiOption;


/**
 * A ParameterMultiOptionArray is a set of ParameterMultiOptions
 * that a user can choose one of.
 * @see ParameterMultiOption
 */
 typedef struct ParameterMultiOptionArray
{
	/** The array of possible options that the user can choose from */
	ParameterMultiOption *pmoa_options_p;

	/** The number of options pointed to by pmoa_options_p */
	uint32 pmoa_num_options;

	/** The ParameterType for these options */
	ParameterType pmoa_values_type;
} ParameterMultiOptionArray;


/**
 * A datatype used for numeric parameters that
 * have a finite range of values.
 */
typedef struct ParameterBounds
{
	/** The minimum value that the Parameter can take. */
	SharedType pb_lower;

	/** The maximum value that the Parameter can take. */
	SharedType pb_upper;
} ParameterBounds;


/**
 * A datatype consisting of a Tag as a key and a
 * SharedType as a value.
 */
typedef struct TagItem
{
	/** The Tag key */
	Tag ti_tag;

	/** The SharedTye value */
	SharedType ti_value;
} TagItem;


/**
 * This is a datatype that stores a read-only c-style string
 * along with a ParameterType.
 *
 * It's primary use is for error-checking of input parameters
 * within a Service as its parameters are being generated from
 * a json request object to make sure that the datatypes are
 * correct.
 */
typedef struct NamedParameterType
{
	/** The read-only name of the Parameter. */
	const char * const npt_name_s;

	/**
	 * The read-only ParameterType for the Parameter with the name
	 * given by npt_name_s.
	 */
	const ParameterType npt_type;
} NamedParameterType;

#define SET_NAMED_PARAMETER_TYPE_TAGS(a,b) {a, b}

/* forward declaration */
struct ParamerterGroup;


/**
 * @brief The datatype that stores all of the information about a Parameter.
 *
 * A Parameter stores all of the information required for a Service or Client to
 * set or get the information required for a settable option. This includes a name,
 * description, the datatype of this parameter and much more. The Parameter can also
 * hold arbitrary key-value pairs of char * for extra information if needed.
 */
typedef struct Parameter
{
	/** The type of the parameter. */
	ParameterType pa_type;

	/** Does the parameter store multiple values? */
	bool pa_multi_valued_flag;

	/** The name of the parameter. */
	char *pa_name_s;

	/** An optional user-friendly name of the parameter to use for client user interfaces. */
	char *pa_display_name_s;

	/** The description for this parameter. */
	char *pa_description_s;

	/**
	 * The default value for this parameter. It requires use
	 * of pa_type to access the correct value.
	 */
	SharedType pa_default;

	/**
	 * If the parameter can only take one of a
	 * constrained set of values, this will be
	 * an array of the possible options. If it's
	 * NULL, then any value can be taken.
	 */
	ParameterMultiOptionArray *pa_options_p;

	/**
	 * Does the parameter have any upper or lower limits?
	 */
	ParameterBounds *pa_bounds_p;

	/**
	 * Callback function to check whether a particular value
	 * is valid for the given parameter.
	 *
	 * @param parameter_p The Parameter to check.
	 * @param value_p Pointer to the potential value
	 * to check for.
	 * @return NULL if the value is valid or an error string if the value is invalid for this Parameter, .
	 */
	const char * (*pa_check_value_fn) (const struct Parameter * const parameter_p, const void *value_p);

	/**
	 * The level of the parameter.
	 */
	ParameterLevel pa_level;

	/**
	 * The current value for this parameter. It requires use
	 * of pa_type to access the correct value.
	 */
	SharedType pa_current_value;

	/** A tag representing this Parameter */
	//Tag pa_tag;

	/**
	 * A map allowing the Parameter to store an arbitrary set of key-value
	 * pairs. Both the keys and values are char *.
	 */
	HashTable *pa_store_p;

	/**
	 * The ParameterGroup to which this Parameter belongs. If this
	 * Parameter is not in a ParameterGroup, then this will be
	 * <code>NULL</code>
	 */
	struct ParameterGroup *pa_group_p;


	/**
	 * A LinkedList of RemoteParameterNodes that hold the information
	 * for Parameters for PairedServices to the Service that owns
	 * this Parameter.
	 */
	LinkedList *pa_remote_parameter_details_p;


} Parameter;


/**
 * A datatype for storing Parameters in a
 * LinkedList.
 *
 * @extends ListItem
 */
typedef struct ParameterNode
{
	/** The ListItem */
	ListItem pn_node;

	/** Pointer to the associated Parameter. */
	Parameter *pn_parameter_p;
} ParameterNode;



#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Allocate a ParameterMultiOptionArray.
 *
 * @param num_options The number of ParameterMultiOptions in this ParameterMultiOptionArray.
 * @param descriptions_ss An array of strings of num_options size to use the ParameterMultiOption descriptions.
 * @param values_p An array of SharedTypes of num_options size to use the ParameterMultiOption values.
 * @param pt The ParameterType for the values in this ParameterMultiOptionArray.
 * @param copy_values_flag If this is <code>true</code>, the entry will make a deep copy of value. If this
 * is <code>false</code> then it will take ownership of the value and use it directly. Note that this
 * means that the data stored by value will be freed when the ParameterMultiOptionArray is freed.
 * @return The newly-allocated ParameterMultiOptionArray or <code>NULL</code> upon error.
 * @memberof ParameterMultiOptionArray
 */
GRASSROOTS_PARAMS_API ParameterMultiOptionArray *AllocateParameterMultiOptionArray (const uint32 num_options, const char ** const descriptions_ss, SharedType *values_p, ParameterType pt, bool copy_values_flag);


/**
 * Free a ParameterMultiOptionArray.
 *
 * @param options_p The ParameterMultiOptionArray to free.
 * @memberof ParameterMultiOptionArray
 */
GRASSROOTS_PARAMS_API void FreeParameterMultiOptionArray (ParameterMultiOptionArray *options_p);


/**
 * Set an entry in a ParameterMultiOptionArray.
 *
 * @param options_p The ParameterMultiOptionArray to free.
 * @param index The index of ParameterMultiOption to adjust in this ParameterMultiOptionArray.
 * @param description_s The description to set for the given ParameterMultiOption.
 * @param value  The SharedType value to set for the given ParameterMultiOption.
 * @param copy_value_flag If this is <code>true</code>, the entry will make a deep copy of value. If this
 * is <code>false</code> then it will take ownership of the value and use it directly. Note that this
 * means that the data stored by value will be freed when the ParameterMultiOptionArray is freed.
 * @return <code>true</code> if the ParameterMultiOption was updated successfully, <code>false</code> otherwise
 * @memberof ParameterMultiOptionArray
 */
GRASSROOTS_PARAMS_API bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value, bool copy_value_flag);


/**
 * Allocate a Parameter
 *
 * @param type The ParameterType for this Parameter.
 * @param multi_valued_flag If this is <code>true</code> then the Parameter can hold multiple values. For single value Parameters, set this to <code>false</code>.
 * @param name_s The name of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param display_name_s An optional name to display for the Parameter for use in Clients. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * This can be <code>NULL</code>.
 * @param description_s The description of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param options_p This can be used to constrain the Parameter to a fixed set of values. If this is <code>NULL</code> then the Parameter can be set to any value.
 * @param default_value The default value for this Parameter.
 * @param current_value_p If this is not <code>NULL</code>, then copy this value as the current value of the Parameter. If this is <code>NULL</code>, then current value for this Parameter
 * will be set to be a copy of its default value.
 * @param bounds_p If this is not <code>NULL</code>, then this will be used to specify the minimum and maximum values that this Parameter can take. If this is <code>NULL</code>,
 * then the Parameter can take any value.
 * @param level The ParameterLevel for this Parameter. This determines when the Client should display this Parameter to the user.
 * @param check_value_fn If this is not <code>NULL</code>, then this will be used to check whether the Parameter has been set to a valid value.
 * @return A newly-allocated Parameter or <code>NULL</code> upon error.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API Parameter *AllocateParameter (const struct ServiceData *service_data_p, ParameterType type, bool multi_valued_flag, const char * const name_s, const char * const display_name_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


/**
 * Free a Parameter.
 *
 * @param param_p The Parameter to free.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API void FreeParameter (Parameter *param_p);


/**
 * Allocate a ParameterBounds.
 *
 * @return The ParameterBounds or <code>NULL</code> upon error.
 * @memberof ParameterBounds
 */
GRASSROOTS_PARAMS_API ParameterBounds *AllocateParameterBounds (void);


/**
 * Test whether a given ParameterLevel matches or exceeds another.
 *
 * @param param_level The level to check.
 * @param threshold The level to check against.
 * @return <code>true</code> if the param_level matched or exceeded the threshold, <code>false</code> otherwise.
 */
GRASSROOTS_PARAMS_API bool CompareParameterLevels (const ParameterLevel param_level, const ParameterLevel threshold);


/**
 * Make a copy of a ParameterBounds.
 *
 * @param src_p The ParameterBounds to copy.
 * @param pt The ParameterType that the given ParameterBounds refers to.
 * @return The newly-allocate ParameterBounds copy or <code>NULL</code> upon error.
 * @memberof ParameterBounds
 */
GRASSROOTS_PARAMS_API ParameterBounds *CopyParameterBounds (const ParameterBounds * const src_p, const ParameterType pt);


/**
 * Free a ParameterBounds.
 *
 * @param bounds_p The ParameterBounds to free.
 * @param pt The ParameterTye that the given ParameterBounds refers to.
 * @memberof ParameterBounds
 */
GRASSROOTS_PARAMS_API void FreeParameterBounds (ParameterBounds *bounds_p, const ParameterType pt);



/**
 * Allocate a ParameterNode with an associated Parameter.
 *
 * @param type The ParameterType for this Parameter.
 * @param name_s The name of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param display_name_s An optional name to display for the Parameter for use in Clients. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * This can be <code>NULL</code>.
 * @param description_s The description of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param tag The Tag for this Parameter.
 * @param options_p This can be used to constrain the Parameter to a fixed set of values. If this is <code>NULL</code> then the Parameter can be set to any value.
 * @param default_value The default value for this Parameter.
 * @param current_value_p If this is not <code>NULL</code>, then copy this value as the current value of the Parameter. If this is <code>NULL</code>, then current value for this Parameter
 * will be set to be a copy of its default value.
 * @param bounds_p If this is not <code>NULL</code>, then this will be used to specify the minimum and maximum values that this Parameter can take. If this is <code>NULL</code>,
 * then the Parameter can take any value.
 * @param level The ParameterLevel for this Parameter. This determines when the Client should display this Parameter to the user.
 * @param check_value_fn If this is not <code>NULL</code>, then this will be used to check whether the Parameter has been set to a valid value.
 * @return A newly-allocated ParameterNode with a Parameter set to the given values or <code>NULL</code> upon error.
 * @memberof ParameterNode
 * @see AllocateParameter
 */
GRASSROOTS_PARAMS_API ParameterNode *GetParameterNode (ParameterType type, const char * const name_s, const char * const display_name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


/**
 * Check whether the value of a Parameter is a non-negative real value.
 *
 * @param parameter_p The Parameter to check.
 * @param value_p The value to check.
 * @return An error message string on failure or <code>NULL</code> if the value
 * is a non-negative real number.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API const char *CheckForSignedReal (const Parameter * const parameter_p, const void *value_p);


/**
 * Check whether the value of a Parameter is not <code>NULL</code>.
 *
 * @param parameter_p The Parameter to check.
 * @param value_p The value to check.
 * @return An error message string on failure or <code>NULL</code> if the value
 * is a not <code>NULL</code>.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API const char *CheckForNotNull (const Parameter * const parameter_p, const void *value_p);


/**
 * Set the current value of a Parameter.
 *
 * @param parameter_p The Parameter to update.
 * @param value_p The value to update the Parameter to. It will be cast to the appropriate type based upon
 * the Parameter's type.
 * @param current_value_flag If this is <code>true</code> then the current parameter value is the one that will be set.
 * If this is <code>false</code>, then the Parameter's default value will be altered.
 * @return <code>true</code> if the Parameter was updated successfully, <code>false</code> otherwise.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API bool SetParameterValue (Parameter * const parameter_p, const void *value_p, const bool current_value_flag);




/**
 * Set the current value of a Parameter from a SharedType.
 *
 * @param parameter_p The Parameter to update.
 * @param src_p The SharedType to update the Parameter from.
 * @param current_value_flag If this is <code>true</code> then the current parameter value is the one that will be set.
 * If this is <code>false</code>, then the Parameter's default value will be altered.
 * @return <code>true</code> if the Parameter was updated successfully, <code>false</code> otherwise.
 * @memberof Parameter
 */GRASSROOTS_PARAMS_API bool SetParameterValueFromSharedType (Parameter * const parameter_p, const SharedType * src_p, const bool current_value_flag);


/**
 * Add a key value pair to a Parameter.
 *
 * @param parameter_p The Parameter to update.
 * @param key_s The key to add to the Parameter. A deep copy of this will be made by the
 * Parameter so the value passed in can go out of scope withut issues.
 * can go out of scope
 * @param value_s The value to add to the Parameter. A deep copy of this will be made by the
 * Parameter so the value passed in can go out of scope withut issues.
 * @return <code>true</code> if the Parameter was updated successfully, <code>false</code> otherwise.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API bool AddParameterKeyValuePair (Parameter * const parameter_p, const char *key_s, const char *value_s);


/**
 * Remove a key value pair from a Parameter.
 *
 * @param parameter_p The Parameter to update.
 * @param key_s The key to remove from the Parameter. Its associated value will also be removed.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API void RemoveParameterKeyValuePair (Parameter * const parameter_p, const char *key_s);


/**
 * Get a value from a Parameter.
 *
 * @param parameter_p The Parameter to query.
 * @param key_s The key used to get the associated value from the Parameter.
 * @return The matching value or <code>NULL</code> if the key did not exist for the Parameter.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API const char *GetParameterKeyValue (const Parameter * const parameter_p, const char *key_s);


/**
 * Get the json-based representation of a Parameter.
 *
 * @param parameter_p The Parameter to get.
 * @param full_definition_flag If this is <code>true</code> then all of the details for this
 * Parameter will get added. If this is <code>false</code> then just the name and current value
 * will get added. This is useful is you just want to send the values to use when running a
 * service.
 * @return A newly-allocated json-based description of the Parameter or <code>NULL</code>
 * upon error. When you no longer require the value you need to call json_decref upon it.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API json_t *GetParameterAsJSON (const Parameter * const parameter_p, const bool full_definition_flag);


/**
 * Create a Parameter from a json-based representation.
 *
 * @param json_p The json-based decription of the Parameter.
 * @param concise_flag If this is <code>true</code>, then just the Parameter
 * values that are needed to run the Service are added. If this is <code>false</code>
 * then user-facing attributes such as description, parameter level, group, etc. will
 * be added.
 * @return A newly-allocated Parameter derived from the json-based description
 * or <code>NULL</code> upon error.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API Parameter *CreateParameterFromJSON (const json_t * const json_p, const bool concise_flag);



/**
 * Does the JSON fragment describe a full set of a Parameter's features or
 * just enough to get its current value.
 *
 * @param json_p The JSON fragment
 * @return <code>true</code> if the JSON fragment contains only enough data
 * to get the current value of the Parameter. If the fragment contains data such
 * as the display name, description, etc. then this will return <code>false</code>.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API bool IsJSONParameterConcise (const json_t * const json_p);


/**
 * Clear the value of a SharedType.
 *
 * @param st_p The SharedType to clear.
 * @param pt The ParameterType for this SharedType
 * @memberof SharedType
 */
GRASSROOTS_PARAMS_API void ClearSharedType (SharedType *st_p, const ParameterType pt);


/**
 * Make a deep copy of the value of one SharedType to another.
 *
 * @param src The SharedType to copy the value from.
 * @param dest_p The SharedType to copy the value to.
 * @param pt The ParameterType for this SharedType
 * @return <code>true</code> if the value was copied successfully, <code>false</code> otherwise.
 * @memberof SharedType
 */
GRASSROOTS_PARAMS_API bool CopySharedType (const SharedType src, SharedType *dest_p, const ParameterType pt);


/**
 * Get the name to use for a Client to use for this Parameter.
 *
 * @param parameter_p The Parameter to get the value for.
 * @return The Parameter's display name if it is not <code>NULL</code>, else
 * the Parameter's name.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API const char *GetUIName (const Parameter * const parameter_p);


/**
 * Get the current value of a Parameter as a string.
 *
 * @param param_p The Parameter to get the current value for.
 * @param alloc_flag_p If the returned value had to be newly created, for example if
 * the type of this Parameter is a number, then this will be set to <code>true</code>
 * and the returned value will need to be freed using FreeCopiedString to avoid a memory
 * leak. If this is <code>false</code> then the returned value points directly to a string
 * within the Parameter's current value.
 * @return The Parameter value as a string or <code>NULL</code> if there was an error.
 * @see FreeCopiedString
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API char *GetParameterValueAsString (const Parameter * const param_p, bool *alloc_flag_p);


/**
 * Allocate a SharedTypeNode set to the given value.
 *
 * @param value The value that will be copied to the SharedTypeNode.
 * @return The newly-allocated SharedTypeNodeor <code>NULL</code> if there was an error.
 * @memberof SharedTypeNode
 */
GRASSROOTS_PARAMS_API SharedTypeNode *AllocateSharedTypeNode (SharedType value);


/**
 * Free a SharedTypeNode.
 *
 * @param node_p The SharedTypeNode to free.
 * @memberof SharedTypeNode
 */
GRASSROOTS_PARAMS_API void FreeSharedTypeNode (ListItem *node_p);


/**
 * Create a RemoteParameterDetails and add it to the given Parameter.
 *
 *
 * @param param_p The Parameter to add the new RemoteParameterDetails to.
 * @param uri_s The URI of the ExternalServer that runs the PairedService that this RemoteParameter belongs to.
 * @param tag The tag for this Parameter on the PairedService.
 * @return <code>true</code> if the RemoteParameterDetails was added successfully or <code>false</code> otherwise.
 * @see AllocateRemoteParameterDetails
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API bool AddRemoteDetailsToParameter (Parameter *param_p, const char * const uri_s, const Tag tag);


/**
 * Get the remote Tag from a RemoteParameterDetails stored on a given Parameter.
 *
 * @param param_p The Parameter whose set of RemoteParameterDetails will be checked.
 * @param uri_s The uri to match.
 * @return A pointer to the Tag or <code>NULL</code> upon error.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API const Tag *GetRemoteTagForURI (Parameter *param_p, const char * const uri_s);


/**
 * Make a deep copy of all of the RemoteParameterDetails stored on one Parameter to another.
 *
 * @param src_param_p The Parameter to copy of all of the RemoteParameterDetails from.
 * @param dest_param_p The Parameter to copy of all of the RemoteParameterDetails to.
 * @return <code>true</code> if the RemoteParameterDetails was copied successfully or <code>false</code> otherwise.
 * @memberof Parameter
 */
GRASSROOTS_PARAMS_API bool CopyRemoteParameterDetails (const Parameter * const src_param_p, Parameter *dest_param_p);


/**
 * Get the human-readable name for a given ParameterType.
 *
 * @param param_type The ParameterType to get the name for.
 * @return The human-readable name or <code>NULL</code> upon error.
 */
GRASSROOTS_PARAMS_API const char *GetGrassrootsTypeAsString (const ParameterType param_type);


/**
 * Get the ParameterType from its human-readable name.
 *
 * @param param_type_s The human-readable name of a ParameterType.
 * @param param_type_p Pointer to where the ParameterType will be set.
 * @return <code>true</code> if the ParameterType was set successfully, <code>false</code> otherwise.
 */
GRASSROOTS_PARAMS_API bool GetGrassrootsTypeFromString (const char *param_type_s, ParameterType *param_type_p);



GRASSROOTS_PARAMS_API bool SetSharedTypeFromJSON (SharedType *value_p, const json_t *json_p, const ParameterType pt);



GRASSROOTS_PARAMS_API bool GetParameterGroupVisibility (const struct ServiceData *service_data_p, const char *group_name_s, bool *visibility_p);


GRASSROOTS_PARAMS_API bool GetParameterDefaultValueFromConfig (const struct ServiceData *service_data_p, const char *param_name_s, const ParameterType pt, SharedType *value_p);


GRASSROOTS_PARAMS_API bool GetParameterDescriptionFromConfig (const struct ServiceData *service_data_p, const char *param_name_s, char **description_ss);


GRASSROOTS_PARAMS_API bool GetParameterDisplayNameFromConfig (const struct ServiceData *service_data_p, const char *param_name_s, char **display_name_ss);


GRASSROOTS_PARAMS_API bool GetParameterLevelFromConfig (const struct ServiceData *service_data_p, const char *param_name_s, ParameterLevel *level_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef PARAMETER_H */
