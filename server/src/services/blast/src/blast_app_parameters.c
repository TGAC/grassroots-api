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
 * blast_app_parameters.cpp
 *
 *  Created on: 19 Oct 2016
 *      Author: billy
 */



#include "blast_app_parameters.h"


bool ParseBlastAppParametersToByteBuffer (BlastAppParameters *app_p, const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p)
{
	return app_p -> bap_parse_params_to_byte_buffer_fn (data_p, params_p, buffer_p);
}
