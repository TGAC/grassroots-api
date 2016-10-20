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
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <syslog.h>

#include "blast_tool.hpp"
#include "blast_service.h"

#include "io_utils.h"

#include "byte_buffer.h"
#include "string_utils.h"
#include "jansson.h"
#include "blast_service_job.h"


#include "blastn_app_parameters.hpp"
#include "blastp_app_parameters.hpp"
#include "blastx_app_parameters.hpp"


const char * const BlastTool :: BT_NAME_S = "name";

const char * const BlastTool :: BT_FACTORY_NAME_S = "factory";



void FreeBlastTool (BlastTool *tool_p)
{
	delete tool_p;
}


OperationStatus RunBlast (BlastTool *tool_p)
{
	OperationStatus status = OS_IDLE;

	tool_p -> PreRun  ();
	status = tool_p -> Run ();
	tool_p -> PostRun  ();

	return status;
}


OperationStatus GetBlastStatus (BlastTool *tool_p)
{
	return (tool_p -> GetStatus ());
}


/******************************/



BlastTool :: BlastTool (BlastServiceJob *service_job_p, const char *name_s, const char *factory_s, const BlastServiceData *data_p, bool (*parse_params_fn) (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p))
{
	bt_job_p = service_job_p;
	bt_name_s = name_s;
	bt_service_data_p = data_p;
	bt_factory_name_s = factory_s;

	bt_parse_params_fn = parse_params_fn;


	if (service_job_p)
		{
			SetServiceJobName (& (bt_job_p -> bsj_job), name_s);
		}

}



BlastTool :: BlastTool (BlastServiceJob *job_p, const BlastServiceData *data_p, const json_t *root_p)
{
	bt_factory_name_s = GetJSONString (root_p, BT_FACTORY_NAME_S);
	if (!bt_factory_name_s)
		{
			throw std :: invalid_argument ("factory name not set");
		}

	bt_name_s = GetJSONString (root_p, BT_NAME_S);
	if (!bt_name_s)
		{
			throw std :: invalid_argument ("name not set");
		}

	bt_job_p = job_p;
	bt_service_data_p = data_p;
	bt_parse_params_fn = NULL;

}


BlastAppParameters *BlastTool :: GetBlastAppParameters (const BlastServiceData *service_data_p)
{
	BlastAppParameters *app_p = 0;

	switch (service_data_p -> bsd_type)
		{
			case BST_BLASTN:
				app_p = new BlastNAppParameters;
				break;

			case BST_BLASTP:
				app_p = new BlastPAppParameters;
				break;

			case BST_BLASTX:
				app_p = new BlastXAppParameters;
				break;

			default:
				break;
		}

	return app_p;
}


OperationStatus BlastTool :: GetStatus (bool update_flag)
{
	OperationStatus status;

	if (update_flag)
		{
			status = GetServiceJobStatus (& (bt_job_p -> bsj_job));
		}
	else
		{
			status = GetCachedServiceJobStatus (& (bt_job_p -> bsj_job));
		}

	return status;
}

const uuid_t &BlastTool :: GetUUID () const
{
	return bt_job_p -> bsj_job.sj_id;
}


const char *BlastTool :: GetName () const
{
	return bt_name_s;
}


const char *BlastTool :: GetFactoryName () const
{
	return bt_factory_name_s;
}


BlastTool :: ~BlastTool ()
{
	if (bt_app_params_p)
		{
			delete bt_app_params_p;
		}
}


void BlastTool :: PreRun ()
{
	SetServiceJobStatus (& (bt_job_p -> bsj_job), OS_STARTED);
}


void BlastTool :: PostRun ()
{
}


json_t *BlastTool :: GetAsJSON ()
{
	json_t *blast_tool_json_p = json_object ();

	if (blast_tool_json_p)
		{
			if (!AddToJSON (blast_tool_json_p))
				{
					json_decref (blast_tool_json_p);
					blast_tool_json_p = 0;
				}
		}

	return blast_tool_json_p;
}




bool BlastTool :: AddToJSON (json_t *root_p)
{
	bool success_flag = false;

	if (json_object_set_new (root_p, BT_NAME_S, json_string (bt_name_s)) == 0)
		{
			if (json_object_set_new (root_p, BT_FACTORY_NAME_S, json_string (bt_factory_name_s)) == 0)
				{
					success_flag = true;
				}		/* if (json_object_set_new (root_p, BT_FACTORY_NAME_S, json_string (bt_factory_name_s)) == 0) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to BlastTool json", BT_FACTORY_NAME_S, bt_factory_name_s);
				}

		}		/* if (json_object_set_new (root_p, BT_NAME_S, json_string (bt_name_s)) == 0) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to BlastTool json", BT_NAME_S, bt_name_s);
		}

	return success_flag;
}

