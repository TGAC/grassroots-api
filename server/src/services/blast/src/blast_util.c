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
/*
 * blast_util.c
 *
 *  Created on: 19 Oct 2016
 *      Author: billy
 */

#include <string.h>

#include "blast_util.h"
#include "string_utils.h"


bool AddArg (const char *arg_s, ByteBuffer *buffer_p)
{
	bool success_flag = true;

	if (buffer_p -> bb_current_index > 0)
		{
			success_flag = AppendStringsToByteBuffer (buffer_p, " ", arg_s, NULL);
		}
	else
		{
			success_flag =  AppendStringToByteBuffer (buffer_p, arg_s);
		}

	return success_flag;
}




bool AddArgsPair (const char *key_s, const char *value_s, ByteBuffer *buffer_p)
{
	bool success_flag = false;

	if (AddArg (key_s, buffer_p))
		{
			if (AddArg (value_s, buffer_p))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add value arg for \"%s\"=\"%s\"", key_s, value_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add key arg for \"%s\"=\"%s\"", key_s, value_s);
		}

	return success_flag;
}



bool AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const char * const param_name_s, const char *key_s, ByteBuffer *buffer_p, const bool unsigned_flag, const bool required_flag)
{
	bool success_flag = !required_flag;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (params_p, param_name_s, &value, true))
		{
			int32 param_value = unsigned_flag ? ((int32) value.st_ulong_value) : (value.st_long_value);
			char *value_s = ConvertIntegerToString (param_value);

			if (value_s)
				{
					success_flag = AddArgsPair (key_s, value_s, buffer_p);
					FreeCopiedString (value_s);
				}		/* if (value_s) */
		}

	return success_flag;
}



bool AddArgsPairFromStringParameter (const ParameterSet *params_p, const char * const param_name_s, const char *key_s, ByteBuffer *buffer_p,  const bool required_flag)
{
	bool success_flag = !required_flag;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (params_p, param_name_s, &value, true))
		{
			success_flag = AddArgsPair (key_s, value.st_string_value_s, buffer_p);
		}

	return success_flag;
}

