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
/**
 * polymarker_tool.cpp
 *
 *  Created on: 6 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#include "polymarker_tool.hpp"



PolymarkerTool :: PolymarkerTool (PolymarkerServiceData *data_p, ServiceJob *job_p)
	: pt_service_data_p  (data_p), pt_service_job_p (job_p)
{
	pt_process_id = 0;
}


PolymarkerTool :: ~PolymarkerTool ()
{

}
