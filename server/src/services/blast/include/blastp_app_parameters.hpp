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
 * blastp_parameters.h
 *
 *  Created on: 18 Oct 2016
 *      Author: billy
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_BLASTP_PARAMETERS_H_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_BLASTP_PARAMETERS_H_


#include "blast_app_parameters.hpp"


class BLAST_SERVICE_LOCAL BlastPAppParameters : public BlastAppParameters
{
public:
	BlastPAppParameters ();
	virtual ~BlastPAppParameters ();

	virtual bool AddParameters (BlastServiceData *data_p, ParameterSet *params_p);

	virtual bool ParseParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p);

private:
	static NamedParameterType BPAP_MATRIX;
	static NamedParameterType BPAP_COMP_BASED_STATS;

	static const uint32 BPAP_NUM_MATRICES;

	static const char *BPAP_MATRICES_SS [];

	static uint32 BPAP_NUM_COMP_BASED_STATS;

};



#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_BLASTP_PARAMETERS_H_ */
