/*
** Copyright 2014-2015 The Genome Analysis Centre
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
 * threaded_blast_tool.hpp
 *
 *  Created on: 9 Nov 2015
 *      Author: billy
 */

#ifndef THREADED_BLAST_TOOL_HPP_
#define THREADED_BLAST_TOOL_HPP_


/*
** Copyright 2014-2015 The Genome Analysis Centre
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
 * threaded_blast_tool.cpp
 *
 *  Created on: 9 Nov 2015
 *      Author: billy
 */

#include "system_blast_tool.hpp"
#include "pthread.h"

/**
 * A class that will run Blast within a pthread.
 */
class BLAST_SERVICE_LOCAL ThreadedBlastTool : public SystemBlastTool
{
public:
	ThreadedBlastTool (ServiceJob *service_job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s);
	virtual ~ThreadedBlastTool ();

	virtual OperationStatus Run ();

	virtual const char *GetResults ();

	virtual OperationStatus GetStatus ();

protected:
	pthread_t tbt_thread;
	pthread_mutex_t tbt_mutex;
	int tbt_thread_id;
};




#endif /* THREADED_BLAST_TOOL_HPP_ */
