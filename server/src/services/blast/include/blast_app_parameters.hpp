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
 * blast_app_parameters.hpp
 *
 *  Created on: 19 Oct 2016
 *      Author: billy
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_APP_PARAMETERS_HPP_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_APP_PARAMETERS_HPP_


#include "blast_service_api.h"
#include "blast_service.h"
#include "parameter_set.h"
#include "byte_buffer.h"



class BLAST_SERVICE_LOCAL BlastAppParameters
{
public:
	BlastAppParameters ();
	virtual ~BlastAppParameters ();

	virtual bool AddParameters (BlastServiceData *data_p, ParameterSet *params_p) = 0;

	virtual bool ParseParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p) = 0;
};


#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_APP_PARAMETERS_HPP_ */
