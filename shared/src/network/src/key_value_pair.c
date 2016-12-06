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
#include "memory_allocations.h"
#include "key_value_pair.h"
#include "string_utils.h"


KeyValuePair *AllocateKeyValuePair (const char *key_s, const char *value_s)
{
	char *copied_key_s = CopyToNewString (key_s, 0, false);

	if (copied_key_s)
		{
			char *copied_value_s = CopyToNewString (value_s, 0, false);

			if (copied_value_s)
				{
					KeyValuePair *kvp_p = (KeyValuePair *) AllocMemory (sizeof (KeyValuePair));

					if (kvp_p)
						{
							kvp_p -> kvp_key_s = copied_key_s;
							kvp_p -> kvp_value_s = copied_value_s;

							return kvp_p;
						}

					FreeCopiedString (copied_value_s);
				}		/* if (copied_value_s) */

			FreeCopiedString (copied_key_s);
		}		/* if (copied_key_s) */

	return NULL;
}


void FreeKeyValuePair (KeyValuePair *kvp_p)
{
	FreeCopiedString (kvp_p -> kvp_key_s);
	FreeCopiedString (kvp_p -> kvp_value_s);
	FreeMemory (kvp_p);
}



KeyValuePairNode *AllocateKeyValuePairNode (const char *key_s, const char *value_s)
{
	KeyValuePairNode *node_p = (KeyValuePairNode *) AllocMemory (sizeof (KeyValuePairNode));

	if (node_p)
		{
			node_p -> kvpn_pair_p = AllocateKeyValuePair (key_s, value_s);

			if (node_p -> kvpn_pair_p)
				{
					node_p -> kvpn_node.ln_prev_p = NULL;
					node_p -> kvpn_node.ln_next_p = NULL;

					return node_p;
				}

			FreeMemory (node_p);
		}		/* if (node_p) */

	return NULL;
}


void FreeKeyValuePairNode (ListItem *node_p)
{
	KeyValuePairNode *kvp_node_p = (KeyValuePairNode *) node_p;

	FreeKeyValuePair (kvp_node_p -> kvpn_pair_p);
	FreeMemory (kvp_node_p);
}


