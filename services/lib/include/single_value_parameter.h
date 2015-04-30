/*
 * single_value_parameter.h
 *
 *  Created on: 29 Apr 2015
 *      Author: tyrrells
 */

#ifndef SINGLE_VALUE_PARAMETER_H_
#define SINGLE_VALUE_PARAMETER_H_

#include "parameter.h"


typedef struct SingleValueParameter
{
	Parameter svp_base_param;

	SharedType svp_current_value;

	SharedType svp_default_value;


} SingleValueParameter;



#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API SingleValueParameter *AllocateSingleValueParameter (ParameterType type, const char * const name_s, const char * const display_name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


WHEATIS_SERVICE_API void FreeSingleValueParameter (SingleValueParameter *param_p);


WHEATIS_SERVICE_API bool SetSingleValuedParameterValue (SingleValueParameter * const param_p, const void *value_p);


WHEATIS_SERVICE_API char *GetSingleParameterValueAsString (const SingleValueParameter * const param_p, bool *alloc_flag_p);


#ifdef __cplusplus
}
#endif


#endif /* SINGLE_VALUE_PARAMETER_H_ */
