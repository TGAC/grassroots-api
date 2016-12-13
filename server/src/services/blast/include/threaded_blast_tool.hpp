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
 * @file
 * @brief
 */

/*
 * threaded_blast_tool.hpp
 *
 *  Created on: 9 Nov 2015
 *      Author: billy
 */

#ifndef THREADED_BLAST_TOOL_HPP_
#define THREADED_BLAST_TOOL_HPP_


#include "system_blast_tool.hpp"
#include "pthread.h"

/**
 * A class that will run Blast within a pthread.
 *
 * @ingroup blast_service
 */
class BLAST_SERVICE_LOCAL ThreadedBlastTool : public SystemBlastTool
{
public:
	ThreadedBlastTool (BlastServiceJob *service_job_p, const char *name_s, const char *factory_s, const BlastServiceData *data_p, const char *blast_program_name_s);
	virtual ~ThreadedBlastTool ();

	/**
	 * Run this ThreadedBlastTool
	 *
	 * @return The OperationStatus of this ThreadedBlastTool after
	 * it has started running.
	 */
	virtual OperationStatus Run ();

	/**
	 * Get the results after the ThreadedBlastTool has finished
	 * running.
	 *
	 * @param formatter_p The BlastFormatter to convert the results
	 * into a different format. If this is 0, then the results will be
	 * returned without any conversion.
	 * @return The results as a c-style string or 0 upon error.
	 */
	virtual char *GetResults (BlastFormatter *formatter_p);

	/**
	 * Get the status of a ThreadedBlastTool
	 *
	 * @param update_flag if this is <code>true</code> then the ThreadedBlastTool
	 * will check the status of its running jobs if necessary, if this is
	 * <code>false</code> it will return the last cached value.
	 * @return The OperationStatus of this ThreadedBlastTool.
	 */
	virtual OperationStatus GetStatus (bool update_flag = true);

protected:
	pthread_t *tbt_thread_p;
	pthread_mutex_t *tbt_mutex_p;
	int tbt_thread_id;
};




#endif /* THREADED_BLAST_TOOL_HPP_ */
