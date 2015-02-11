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
