/**@file parameter.h
*/

#ifndef PARAMETER_H
#define PARAMETER_H

#include "jansson.h"

#include "linked_list.h"
#include "wheatis_service_library.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * The different types that a Parameter can take.
 */
typedef enum ParameterType
{
	PT_BOOLEAN,
	PT_SIGNED_INT,
	PT_UNSIGNED_INT,
	PT_SIGNED_REAL,
	PT_UNSIGNED_REAL,
	PT_STRING,
	PT_FILE_TO_WRITE,
	PT_FILE_TO_READ,
	PT_DIRECTORY,
	PT_CHAR,
	PT_NUM_TYPES
} ParameterType;


/**
 * The ParameterLevel defines the level that a user
 * should be to adjust the Parameter. The level of
 * Parameters that a user wishes to work with can
 * be adjusted within the user interface.
 */
typedef enum ParameterLevel
{
	PL_BASIC,
	PL_INTERMEDIATE,
	PL_ADVANCED
} ParameterLevel;

/**
 * A datatype to store values for a Parameter.
 */
typedef union SharedType
{
	bool st_boolean_value;

	int32 st_long_value;

	uint32 st_ulong_value;

	double64 st_data_value;

	char *st_string_value_s;

	char st_char_value;

} SharedType;



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

/******* FORWARD DECLARATION *******/
struct Parameter;

/**
 * The datatype that stores all of the information about a
 * Parameter.
 */
typedef struct Parameter
{
	/** The type of the parameter. */
	ParameterType pa_type;

	/** The user-friendly name of the parameter. */
	char *pa_name_s;

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

} Parameter;


typedef struct ParameterNode
{
	ListItem pn_node;

	Parameter *pn_parameter_p;

} ParameterNode;




WHEATIS_SERVICE_API ParameterMultiOptionArray *AllocateParameterMultiOptionArray (const uint32 num_options, const char ** const descriptions_p, SharedType *values_p, ParameterType pt);


WHEATIS_SERVICE_API void FreeParameterMultiOptionArray (ParameterMultiOptionArray *options_p);


WHEATIS_SERVICE_API bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value);


WHEATIS_SERVICE_API Parameter *AllocateParameter (ParameterType type, const char * const name_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


WHEATIS_SERVICE_API void FreeParameter (Parameter *param_p);


WHEATIS_SERVICE_API ParameterBounds *AllocateParameterBounds (void);


WHEATIS_SERVICE_API int CompareParameterLevels (const ParameterLevel pl0, const ParameterLevel pl1);


WHEATIS_SERVICE_API ParameterBounds *CopyParameterBounds (const ParameterBounds * const src_p, const ParameterType pt);


WHEATIS_SERVICE_API void FreeParameterBounds (ParameterBounds *bounds_p, const ParameterType pt);


WHEATIS_SERVICE_API ParameterNode *GetParameterNode (ParameterType type, const char * const name_s, const char * const key_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


WHEATIS_SERVICE_API const char *CheckForSignedReal (const Parameter * const parameter_p, const void *value_p);


WHEATIS_SERVICE_API const char *CheckForNotNull (const Parameter * const parameter_p, const void *value_p);


WHEATIS_SERVICE_API bool SetParameterValue (Parameter * const parameter_p, const void *value_p);


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
 */
WHEATIS_SERVICE_API json_t *GetParameterAsJSON (const Parameter * const parameter_p, const bool full_definition_flag);


/**
 * Create a Parameter from a json-based representation.
 *
 * @param json_p The json-based decription of the Parameter.
 * @return A newly-allocated Parameter derived from the json-based description
 * or <code>NULL</code> upon error.
 */
WHEATIS_SERVICE_API Parameter *CreateParameterFromJSON (const json_t * const json_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef PARAMETER_H */

