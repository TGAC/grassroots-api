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
 * paired_samtools_service.c
 *
 *  Created on: 13 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#include <string.h>

#include "paired_samtools_service.h"
#include "string_utils.h"
#include "parameter_set.h"





bool SaveRemoteSamtoolsJobDetails (RemoteServiceJob *job_p, const ServiceData *service_data_p)
{
	bool success_flag = false;

	return success_flag;
}



bool AddPairedIndexParameters (Service *service_p, Parameter *internal_dbs_param_p, ParameterSet *internal_params_p)
{
	bool success_flag = true;

	if (service_p -> se_paired_services.ll_size > 0)
		{
			PairedServiceNode *paired_service_node_p = (PairedServiceNode *) (service_p -> se_paired_services.ll_head_p);

			while (paired_service_node_p)
				{
					/*
					 * Try and add the external server's indexes
					 */
					PairedService *paired_service_p = paired_service_node_p -> psn_paired_service_p;
					Parameter *external_dbs_param_p = GetParameterFromParameterSetByName (paired_service_p -> ps_params_p, SS_INDEX.npt_name_s);

					if (external_dbs_param_p)
						{
							if (external_dbs_param_p -> pa_options_p)
								{
									ParameterOptionNode *node_p = (ParameterOptionNode *) (external_dbs_param_p -> pa_options_p -> ll_head_p);

									while (success_flag && node_p)
										{
											ParameterOption *option_p = node_p -> pon_option_p;
											char *db_s = CreateDatabaseName (option_p -> po_description_s, paired_service_p -> ps_server_name_s);

											if (db_s)
												{
													if (!CreateAndAddParameterOptionToParameter (internal_dbs_param_p, option_p -> po_value, db_s))
														{
															PrintLog (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add database \"%s\" from paired service \"%s\"", option_p -> po_description_s, paired_service_p -> ps_name_s);
														}

													FreeCopiedString (db_s);
												}

											node_p = (ParameterOptionNode *) (node_p -> pon_node.ln_next_p);
										}		/* while (success_flag && node_p) */

								}		/* if (external_dbs_param_p -> pa_options_p) */
							else
								{
									PrintLog (STM_LEVEL_WARNING, __FILE__, __LINE__, "No contig options available from paired service \"%s\"", paired_service_p -> ps_name_s);
								}

						}		/* if (external_dbs_param_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get parameter with name \"%s\"", SS_INDEX.npt_name_s);
						}


					paired_service_node_p = (PairedServiceNode *) (paired_service_node_p -> psn_node.ln_next_p);
				}		/* while (paired_service_node_p) */

		}		/* if (service_p -> se_paired_services.ll_size > 0) */

	return success_flag;
}



char *CreateDatabaseName (const char *database_s, const char *server_s)
{
	char *database_name_s = ConcatenateVarargsStrings (database_s, " provided by ", server_s, NULL);

	return database_name_s;
}

