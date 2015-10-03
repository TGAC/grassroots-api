/*
 * string_int_pair.c
 *
 *  Created on: 3 Oct 2015
 *      Author: billy
 */

#include "string_int_pair.h"


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
