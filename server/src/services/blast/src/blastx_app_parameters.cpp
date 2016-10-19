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
 * blastx_parameters.c
 *
 *  Created on: 19 Oct 2016
 *      Author: billy
 */


#include "blastx_app_parameters.hpp"


/*************************************/
/********* API DEFINITIONS ***********/
/*************************************/

BlastXAppParameters :: BlastXAppParameters ()
{

}


BlastXAppParameters :: ~BlastXAppParameters ()
{

}


bool BlastXAppParameters :: ParseParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p)
{
	bool success_flag = false;

	return success_flag;
}



bool BlastXAppParameters :: AddParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;



	return success_flag;
}
