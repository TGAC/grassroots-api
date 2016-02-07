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
 * remote_parameter_details.c
 *
 *  Created on: 6 Feb 2016
 *      Author: billy
 */

#include "remote_parameter_details.h"
#include "string_utils.h"
#include "json_util.h"


RemoteParameterDetails *AllocateRemoteParameterDetails (const char * const uri_s, Tag tag)
{
	char *copied_uri_s = CopyToNewString (uri_s, 0, false);

	if (copied_uri_s)
		{
			RemoteParameterDetails *details_p = (RemoteParameterDetails *) AllocMemory (sizeof (RemoteParameterDetails));

			if (details_p)
				{
					details_p -> rpd_server_uri_s = copied_uri_s;
					details_p -> rpd_tag = tag;

					return details_p;
				}

			FreeCopiedString (copied_uri_s);
		}

	return NULL;
}


void FreeRemoteParameterDetails (RemoteParameterDetails *details_p)
{
	FreeCopiedString (details_p -> rpd_server_uri_s);
	FreeMemory (details_p);
}


RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNode (RemoteParameterDetails *details_p)
{
	RemoteParameterDetailsNode *node_p = (RemoteParameterDetailsNode *) AllocMemory (sizeof (RemoteParameterDetailsNode));

	if (node_p)
		{
			node_p -> rpdn_details_p = details_p;
			node_p -> rpdn_node.ln_prev_p = NULL;
			node_p -> rpdn_node.ln_next_p = NULL;
		}

	return node_p;
}


RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNodeByParts (const char * const uri_s, Tag tag)
{
	RemoteParameterDetails *details_p = AllocateRemoteParameterDetails (uri_s, tag);

	if (details_p)
		{
			RemoteParameterDetailsNode *node_p = (RemoteParameterDetailsNode *) AllocMemory (sizeof (RemoteParameterDetailsNode));

			if (node_p)
				{
					node_p -> rpdn_details_p = details_p;
					node_p -> rpdn_node.ln_prev_p = NULL;
					node_p -> rpdn_node.ln_next_p = NULL;

					return node_p;
				}

			FreeRemoteParameterDetails (details_p);
		}		/* if (details_p) */

	return NULL;
}


void FreeRemoteParameterDetailsNode (ListItem *node_p)
{
	RemoteParameterDetailsNode *details_node_p = (RemoteParameterDetailsNode *) node_p;

	FreeRemoteParameterDetails (details_node_p -> rpdn_details_p);
	FreeMemory (details_node_p);
}


json_t *GetRemoteParameterDetailsAsJSON (const RemoteParameterDetails *details_p)
{
	json_error_t err;
	json_t *json_p = json_pack_ex (&err, 0, "{s:s,s:i}", PARAM_REMOTE_URI_S, details_p -> rpd_server_uri_s, PARAM_REMOTE_TAG_S, details_p -> rpd_tag);

	if (!json_p)
		{

		}

	return json_p;

}


