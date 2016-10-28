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
 * drmaa_blast_tool.hpp
 *
 *  Created on: 27 Apr 2015
 *      Author: billy
 */

#ifndef DRMAA_BLAST_TOOL_HPP_
#define DRMAA_BLAST_TOOL_HPP_

#include "external_blast_tool.hpp"
#include "drmaa_tool.h"
#include "drmaa_tool_args_processor.hpp"


/**
 * A class that will run Blast as a drmaa process.
 */
class BLAST_SERVICE_LOCAL DrmaaBlastTool : public ExternalBlastTool
{
public:


	static DrmaaBlastTool *GetFromJSON (const json_t *json_p, const char *name_s, const char *program_name_s, ServiceJob *job_p, BlastServiceData *service_data_p);

	/**
	 * Create a DrmaaBlastTool for a given ServiceJob.
	 *
	 * @param job_p The ServiceJob to associate with this DrmaaBlastTool.
	 * @param name_s The name to give to this DrmaaBlastTool.
	 * @param factory_s The name of the DrmaaBlastFactory that is creating this DrmaaBlastTool.
	 * @param data_p The BlastServiceData for the Service that will run this DrmaaBlastTool.
	 * @param blast_program_name_s The name of blast command line executable that this DrmaaBlastTool.
	 * will call to run its blast job.
	 * @param queue_name_s The name of the queue that this DrmaaBlastTool will use.
	 * @param output_path_s The folder where all of the Blast output and log files will be written to.
	 * @param async_flag Should the DrmaaBlastTool run in asynchronous mode.
	 */
	DrmaaBlastTool (BlastServiceJob *job_p, const char *name_s, const char *factory_s, const BlastServiceData *data_p, const char *blast_program_name_s, const char *queue_name_s, const char *const output_path_s, bool async_flag);


	DrmaaBlastTool (BlastServiceJob *job_p, const BlastServiceData *data_p, const json_t *json_p);


	/**
	 * Set the Drmaa settings for this DrmaaBlastTool.
	 *
	 * @param drmaa_p The DrmaaTool that will be used to rub the Blast job.
	 * @param async_flag If this is <code>true</code> then the call to run
	 * the Blast job will return immediately and can be polled for results
	 * as required. If this is <code>false</code> then the call to Run will
	 * not return until the Blast job has finished running.
	 */
	void SetDrmaaOptions (DrmaaTool *drmaa_p, bool async_flag);

	/**
	 *  The DrmaaBlastTool destructor.
	 */
	virtual ~DrmaaBlastTool ();

	/**
	 * Run this DrmaaBlastTool.
	 *
	 * If this is set to run asynchronously, then the call to run
	 * the Blast job will return immediately and can be polled for results
	 * as required. If not, then this will
	 * not return until the Blast job has finished running.
	 *
	 * @return The OperationStatus of this DrmaaBlastTool after
	 * it has started running.
	 */
	virtual OperationStatus Run ();


	/**
	 * Get the status of a DrmaaBlastTool.
	 *
	 * @return The OperationStatus of this DrmaaBlastTool.
	 * @see BlastTool::GetStatus
	 */
	virtual OperationStatus GetStatus (bool update_flag = true);


	/**
	 * Set the number of cores that this DrmaaBlastTool that will try to use
	 * when it runs.
	 *
	 * @param cores The number of cores.
	 * @see SetDrmaaToolCores
	 */
	void SetCoresPerSearch (uint32 cores);

	/**
	 * This sets up notification emails for each Blast job via DRMAA.
	 *
	 * @param email_addresses_ss A NULL-terminated array of email addresses to send
	 * notification emails to
	 * @return <code>true</code> if the notifications were set successfully, <code>false</code> otherwise.
	 * @see SetDrmaaToolEmailNotifications
	 */
	bool SetEmailNotifications (const char **email_addresses_ss);

	/**
	 * Set the output file parameter.
	 *
	 * @return <code>true</code> if the input filename was set
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool SetUpOutputFile ();


protected:

	DrmaaToolArgsProcessor *dbt_args_processor_p;


	virtual ArgsProcessor *GetArgsProcessor ();

	/**
	 * Add the argument to the command line arguments
	 * that this DrmaaBlastTool will run with.
	 *
	 * @param arg_s The argument to add.
	 * @return <code>true</code> if the argument was added
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool AddBlastArg (const char * const arg_s, const bool hyphen_flag);



	/**
	 * This method is used to serialise this DrmaaBlastTool so that
	 * it can be recreated from another calling process when required.
	 * This is used to store the DrmaaBlastTool in the JobsManager.
	 *
	 * @param root_p The json object to add the serialisation of this DrmaaBlastTool to.
	 * @return <code>true</code> if the DrmaaBlastTool was serialised
	 * successfully, <code>false</code>
	 * otherwise.
	 */
	virtual bool AddToJSON (json_t *root_p);


	bool ConvertArgsToDrmaa ();


private:

	static const char * const DBT_ASYNC_S;

	static const char * const DBT_DRMAA_S;

	/**
	 * @private
	 *
	 * The DrmaaTool used to run the Blast job.
	 */
	DrmaaTool *dbt_drmaa_tool_p;

	/**
	 * @private
	 *
	 * Whether the DRMAA call should be asynchronous or not.
	 */
	bool dbt_async_flag;


	DrmaaBlastTool (ServiceJob *job_p, DrmaaTool *drmaa_p, bool async_flag);
};


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get a newly created BlastTool
 *
 * @return The BlastTool or <code>NULL</code> upon error.
 */
BLAST_SERVICE_API BlastTool *CreateDrmaaBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);


#ifdef __cplusplus
}
#endif


#endif /* DRMAA_BLAST_TOOL_HPP_ */
