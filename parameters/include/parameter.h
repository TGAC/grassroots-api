/**@file parameter.h
*/

#ifndef PARAMETER_H
#define PARAMETER_H

#include "jansson.h"
#include "linked_list.h"
#include "wheatis_params_library.h"
#include "tags.h"
#include "resource.h"
#include "hash_table.h"


/******* FORWARD DECLARATION *******/
struct Parameter;


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
	PT_SIGNED_REAL,
	PT_UNSIGNED_REAL,
	PT_STRING,
	PT_FILE_TO_WRITE,
	PT_FILE_TO_READ,
	PT_DIRECTORY,
	PT_CHAR,
	PT_PASSWORD,
	PT_KEYWORD,
	PT_LARGE_STRING,
	PT_NUM_TYPES
} ParameterType;


/**
 * The ParameterLevel defines the level that a user
 * should be to adjust the Parameter. The level of
 * Parameters that a user wishes to work with can
 * be adjusted within the user interface.
 */
typedef uint8 ParameterLevel;

#define PL_BASIC (1)
#define PL_INTERMEDIATE (1 << 1)
#define PL_ADVANCED (1 << 2)
#define PL_ALL (PL_BASIC | PL_INTERMEDIATE | PL_ADVANCED)


/**
 * A datatype to store values for a Parameter.
 * Use the ParameterType to access the correct
 * value.
 */
typedef union SharedType
{
	bool st_boolean_value;

	int32 st_long_value;

	uint32 st_ulong_value;

	double64 st_data_value;

	char *st_string_value_s;

	char st_char_value;

	Resource *st_resource_value_p;

	LinkedList *st_multiple_values_p;

} SharedType;


typedef struct SharedTypeNode
{
	ListItem stn_node;
	SharedType stn_value;
} SharedTypeNode;



typedef struct ParameterMultiOption
{
	char *pmo_description_s;
	SharedType pmo_value;
} ParameterMultiOption;


typedef struct ParameterMultiOptionArray
{
	ParameterMultiOption *pmoa_options_p;
	uint32 pmoa_num_options;
	ParameterType pmoa_values_type;
} ParameterMultiOptionArray;


typedef struct ParameterBounds
{
	SharedType pb_lower;
	SharedType pb_upper;
} ParameterBounds;

typedef struct TagItem
{
	Tag ti_tag;
	SharedType ti_value;
} TagItem;


/**
 * A datatype to tell the system that certain
 * parameters should be grouped together in the
 * client's user interface if possible.
 */
typedef struct ParameterGroup
{
	/** The name of the ParameterGroup */
	char *pg_name_s;

	/** The number of Parameters in this ParameterGroup */
	uint32 pg_num_params;

	/**
	 * An array of pointers to the Parameters that make up
	 * this ParameterGroup.
	 */
	struct Parameter **pg_params_pp;
} ParameterGroup;




/**
 * The datatype that stores all of the information about a
 * Parameter.
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
	Tag pa_tag;

	HashTable *pa_store_p;

	/**
	 * The ParameterGroup to which this Parameter belongs. If this
	 * Parameter is not in a ParameterGroup, then this will be
	 * <code>NULL</code>
	 */
	ParameterGroup *pa_group_p;
} Parameter;


/**
 * A datatype for storing Parameters in a
 * LinkedList.
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


WHEATIS_PARAMS_API ParameterMultiOptionArray *AllocateParameterMultiOptionArray (const uint32 num_options, const char ** const descriptions_p, SharedType *values_p, ParameterType pt);


WHEATIS_PARAMS_API void FreeParameterMultiOptionArray (ParameterMultiOptionArray *options_p);


WHEATIS_PARAMS_API bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value);


WHEATIS_PARAMS_API Parameter *AllocateParameter (ParameterType type, bool multi_valued_flag, const char * const name_s, const char * const display_name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


/**
 * Free a Parameter.
 *
 * @param param_p The Parameter to free.
 * @memberof Parameter
 */
WHEATIS_PARAMS_API void FreeParameter (Parameter *param_p);


/**
 * Allocate a ParameterBounds.
 *
 * @return The ParameterBounds or <code>NULL</code> upon error.
 * @memberof ParameterBounds
 */
WHEATIS_PARAMS_API ParameterBounds *AllocateParameterBounds (void);


WHEATIS_PARAMS_API bool CompareParameterLevels (const ParameterLevel param_level, const ParameterLevel threshold);


/**
 * Make a copy of a ParameterBounds.
 *
 * @param src_p The ParameterBounds to copy.
 * @param pt The ParameterType that the given ParameterBounds refers to.
 * @return The newly-allocate ParameterBounds copy or <code>NULL</code> upon error.
 * @memberof ParameterBounds
 */
WHEATIS_PARAMS_API ParameterBounds *CopyParameterBounds (const ParameterBounds * const src_p, const ParameterType pt);


/**
 * Free a ParameterBounds.
 *
 * @param bounds_p The ParameterBounds to free.
 * @pt The ParameterTye that the given ParameterBounds refers to.
 * @memberof ParameterBounds
 */
WHEATIS_PARAMS_API void FreeParameterBounds (ParameterBounds *bounds_p, const ParameterType pt);


WHEATIS_PARAMS_API ParameterNode *GetParameterNode (ParameterType type, const char * const name_s, const char * const key_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType current_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


WHEATIS_PARAMS_API const char *CheckForSignedReal (const Parameter * const parameter_p, const void *value_p);


WHEATIS_PARAMS_API const char *CheckForNotNull (const Parameter * const parameter_p, const void *value_p);


/**
 * Set the current value of a Parameter.
 *
 * @param parameter_p The Parameter to update.
 * @param value_p The value to update the Parameter to.
 * @return <code>true</code> if the Parameter was updated successfully, <code>false</code> otherwise.
 * @memberof Parameter
 */
WHEATIS_PARAMS_API bool SetParameterValue (Parameter * const parameter_p, const void *value_p);


WHEATIS_PARAMS_API bool AddParameterKeyValuePair (Parameter * const parameter_p, const char *key_s, const char *value_s);


WHEATIS_PARAMS_API void RemoveParameterKeyValuePair (Parameter * const parameter_p, const char *key_s);


WHEATIS_PARAMS_API const char *GetParameterKeyValue (const Parameter * const parameter_p, const char *key_s);


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
WHEATIS_PARAMS_API json_t *GetParameterAsJSON (const Parameter * const parameter_p, const bool full_definition_flag);


/**
 * Create a Parameter from a json-based representation.
 *
 * @param json_p The json-based decription of the Parameter.
 * @return A newly-allocated Parameter derived from the json-based description
 * or <code>NULL</code> upon error.
 * @memberof Parameter
 */
WHEATIS_PARAMS_API Parameter *CreateParameterFromJSON (const json_t * const json_p);



WHEATIS_PARAMS_API bool IsJSONParameterConcise (const json_t * const json_p);


/**
 * Clear the value of a SharedType.
 *
 * @param st_p The SharedType to clear.
 */
WHEATIS_PARAMS_API void ClearSharedType (SharedType *st_p);


WHEATIS_PARAMS_API const char *GetUIName (const Parameter * const parameter_p);


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
WHEATIS_PARAMS_API char *GetParameterValueAsString (const Parameter * const param_p, bool *alloc_flag_p);


/**
 * Allocate a SharedTypeNode set to the given value.
 *
 * @param value The value that will be copied to the SharedTypeNode.
 * @return The newly-allocated SharedTypeNodeor <code>NULL</code> if there was an error.
 * @memberof SharedTypeNode
 */
WHEATIS_PARAMS_API SharedTypeNode *AllocateSharedTypeNode (SharedType value);


/**
 * Free a SharedTypeNode.
 *
 * @param node_p The SharedTypeNode to free.
 * @memberof SharedTypeNode
 */
WHEATIS_PARAMS_API void FreeSharedTypeNode (ListItem *node_p);



#ifdef __cplusplus
}
#endif

#endif	/* #ifndef PARAMETER_H */

