/*
 * multi_value_parameter.h
 *
 *  Created on: 29 Apr 2015
 *      Author: tyrrells
 */

#ifndef MULTI_VALUE_PARAMETER_H_
#define MULTI_VALUE_PARAMETER_H_

#include "parameter.h"

typedef struct MultiValueParameter
{
	Parameter mvp_base_param;

	SharedTypePair *mvp_values_p;

	uint32 mvp_num_values;

} SingleValueParameter;



#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API MultiValueParameter *AllocateMultiValueParameter (ParameterType type, const char * const name_s, const char * const display_name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


WHEATIS_SERVICE_API void FreeMultiValueParameter (MultiValueParameter *param_p);


#ifdef __cplusplus
}
#endif

#endif /* MULTI_VALUE_PARAMETER_H_ */
