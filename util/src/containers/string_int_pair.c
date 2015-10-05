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
 * string_int_pair.c
 *
 *  Created on: 3 Oct 2015
 *      Author: billy
 */

#include <stdlib.h>

#include "string_int_pair.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "data_resource.h"


static int CompareStringIntCounts (const void *v0_p, const void *v1_p);


StringIntPairArray *AllocateStringIntPairArray (const uint32 size)
{
	StringIntPair *pairs_p = (StringIntPair *) AllocMemoryArray (size, sizeof (StringIntPair));

	if (pairs_p)
		{
			StringIntPairArray *array_p = (StringIntPairArray *) AllocMemory (sizeof (StringIntPairArray));

			if (array_p)
				{
					array_p -> sipa_values_p = pairs_p;
					array_p -> sipa_size = size;

					return array_p;
				}

			FreeMemory (pairs_p);
		}

	return NULL;
}


void FreeStringIntPairArray (StringIntPairArray *array_p)
{
	uint32 i = array_p -> sipa_size;
	StringIntPair *pair_p = array_p -> sipa_values_p;

	for ( ; i > 0; -- i, ++ pair_p)
		{
			ClearStringIntPair (pair_p);
		}

	FreeMemory (array_p -> sipa_values_p);
	FreeMemory (array_p);
}


bool SetStringIntPair (StringIntPair *pair_p, char *text_s, MEM_FLAG text_mem, const uint32 value)
{
	bool success_flag = true;
	char *dest_s = NULL;

	switch (text_mem)
		{
			case MF_DEEP_COPY:
				if (text_s)
					{
						dest_s = CopyToNewString (text_s, 0, false);
					}
				else
					{
						success_flag = false;
					}
				break;

			case MF_SHALLOW_COPY:
			case MF_SHADOW_USE:
				dest_s = text_s;
				break;

			default:
				success_flag = false;
				break;
		}

	if (success_flag)
		{
			ClearStringIntPair (pair_p);

			pair_p -> sip_string_mem = text_mem;
			pair_p -> sip_string_s = dest_s;
			pair_p -> sip_value = value;
		}

	return success_flag;
}


void ClearStringIntPair (StringIntPair *pair_p)
{
	switch (pair_p -> sip_string_mem)
		{
			case MF_DEEP_COPY:
			case MF_SHALLOW_COPY:
				if (pair_p -> sip_string_s)
					{
						FreeCopiedString (pair_p -> sip_string_s);
					}
				break;

			case MF_SHADOW_USE:
			case MF_ALREADY_FREED:
			default:
				break;
		}

	pair_p -> sip_string_mem = MF_ALREADY_FREED;
	pair_p -> sip_string_s = NULL;
	pair_p -> sip_value = 0;
}



void SortStringIntPairsByCount (StringIntPairArray *pairs_p)
{
	qsort (pairs_p -> sipa_values_p, pairs_p -> sipa_size, sizeof (StringIntPair), CompareStringIntCounts);
}



json_t *GetStringIntPairsAsResourceJSON (const StringIntPairArray *pairs_p)
{
	json_t *root_p = json_array ();

	if (root_p)
		{
			uint32 i = pairs_p -> sipa_size;
			StringIntPair *pair_p = pairs_p -> sipa_values_p;

			for ( ; i > 0; -- i, ++ pair_p)
				{
					json_t *resource_p = NULL;
					char *title_s = GetFilenameOnly (pair_p -> sip_string_s);

					if (title_s)
						{
							resource_p = GetResourceAsJSONByParts (PROTOCOL_IRODS_S, pair_p -> sip_string_s, title_s, NULL);
							FreeCopiedString (title_s);
						}
					else
						{
							resource_p = GetResourceAsJSONByParts (PROTOCOL_IRODS_S, pair_p -> sip_string_s, pair_p -> sip_string_s, NULL);
						}

					if (resource_p)
						{
							if (json_array_append (root_p, resource_p) != 0)
								{

								}
						}
				}

		}

	return root_p;
}


static int CompareStringIntCounts (const void *v0_p, const void *v1_p)
{
	const StringIntPair *pair0_p = (const StringIntPair *) v0_p;
	const StringIntPair *pair1_p = (const StringIntPair *) v1_p;

	return ((pair0_p -> sip_value) - (pair1_p -> sip_value));
}
