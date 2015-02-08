#include <string.h>

#define ALLOCATE_RESOURCE_TAGS
#include "resource.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "json_util.h"


static bool ReplaceValue (const char * const src_s, char **dest_ss);



Resource *ParseStringToResource (const char * const resource_s)
{
	if (resource_s)
		{
			char *delimiter_p = strstr (resource_s, RESOURCE_DELIMITER_S);

			if (delimiter_p)
				{
					const size_t resource_length = strlen (resource_s);
					const size_t delimiter_length = strlen (RESOURCE_DELIMITER_S);

					/* check that resource_s has valid data after the delimiter */
					if (delimiter_p + delimiter_length < resource_s + resource_length)
						{
							char *protocol_s = CopyToNewString (resource_s, delimiter_p - resource_s, false);

							if (protocol_s)
								{
									char *value_s = NULL;

									delimiter_p += delimiter_length;

									value_s = CopyToNewString (delimiter_p, resource_s + resource_length - delimiter_p, false);

									if (value_s)
										{
											Resource *resource_p = AllocateResource (NULL, NULL);

											if (resource_p)
												{
													resource_p -> re_protocol_s = protocol_s;
													resource_p -> re_value_s = value_s;

													return resource_p;
												}

											FreeCopiedString (value_s);
										}		/* if (value_s) */

									FreeCopiedString (protocol_s);
								}		/* if (protocol_s) */
						}
				}
		}

	return NULL;
}

Resource *AllocateResource (const char *protocol_s, const char *value_s)
{
	Resource *resource_p = (Resource *) AllocMemory (sizeof (Resource));

	if (resource_p)
		{
			InitResource (resource_p);

			if (!SetResourceValue (resource_p, protocol_s, value_s))
				{
					FreeResource (resource_p);
					resource_p = NULL;
				}
		}

	return resource_p;
}


void InitResource (Resource *resource_p)
{
	resource_p -> re_protocol_s = NULL;
	resource_p -> re_value_s = NULL;
}


void FreeResource (Resource *resource_p)
{
	ClearResource (resource_p);
	FreeMemory (resource_p);
}


void ClearResource (Resource *resource_p)
{
	if (resource_p -> re_protocol_s)
		{
			FreeCopiedString (resource_p -> re_protocol_s);
			resource_p -> re_protocol_s = NULL;
		}

	if (resource_p -> re_value_s)
		{
			FreeCopiedString (resource_p -> re_value_s);
			resource_p -> re_value_s = NULL;
		}

}


bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s)
{
	bool success_flag = false;
	char *new_value_s = NULL;
	char *new_protocol_s = NULL;

	if (ReplaceValue (protocol_s, &new_protocol_s))
		{
			if (ReplaceValue (value_s, &new_value_s))
				{
					resource_p -> re_protocol_s = new_protocol_s;
					resource_p -> re_value_s = new_value_s;

					success_flag = true;
				}
		}

	return success_flag;
}


bool CopyResource (const Resource * const src_p, Resource * const dest_p)
{
	bool success_flag = SetResourceValue (dest_p, src_p -> re_protocol_s, src_p -> re_value_s);

	return success_flag;
}


json_t *GetResourceAsJSON (const char * const protocol_s, const char * const path_s)
{
	json_t *json_p = json_object ();

	if (json_p)
		{
			if (json_object_set_new (json_p, RESOURCE_PROTOCOL_S, json_string (protocol_s)) == 0)
				{
					if (json_object_set_new (json_p, RESOURCE_VALUE_S, json_string (path_s)) == 0)
						{
							return json_p;
						}
				}

			json_object_clear (json_p);
			json_decref (json_p);
		}

	return NULL;
}


static bool ReplaceValue (const char * const src_s, char **dest_ss)
{
	bool success_flag = false;

	if (src_s)
		{
			char *new_value_s = CopyToNewString (src_s, 0, false);

			if (new_value_s)
				{
					if (*dest_ss)
						{
							FreeCopiedString (*dest_ss);
						}

					*dest_ss = new_value_s;
					success_flag = true;
				}
		}
	else
		{
			if (*dest_ss)
				{
					FreeCopiedString (*dest_ss);
					*dest_ss = NULL;
				}

			success_flag = true;
		}

	return success_flag;
}
