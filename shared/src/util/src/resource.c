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
#include <string.h>

#define ALLOCATE_RESOURCE_TAGS
#include "data_resource.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "json_util.h"


static bool ReplaceValue (const char * const src_s, char **dest_ss);


bool GetResourceProtocolAndPath (const char * const resource_s, char ** const protocol_ss, char ** const path_ss)
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
							char *path_s = NULL;

							delimiter_p += delimiter_length;

							path_s = CopyToNewString (delimiter_p, resource_s + resource_length - delimiter_p, false);

							if (path_s)
								{
									*protocol_ss = protocol_s;
									*path_ss = path_s;

									return true;
								}		/* if (path_s) */

							FreeCopiedString (protocol_s);
						}		/* if (protocol_s) */
				}
		}

	return false;
}


Resource *ParseStringToResource (const char * const resource_s)
{
	Resource *resource_p = NULL;

	if (resource_s)
		{
			char *protocol_s = NULL;
			char *path_s = NULL;

			if (GetResourceProtocolAndPath (resource_s, &protocol_s, &path_s))
				{
					resource_p = AllocateResource (NULL, NULL, NULL);

					if (resource_p)
						{
							resource_p -> re_protocol_s = protocol_s;
							resource_p -> re_value_s = path_s;
						}		/* if (resource_p) */

				}		/* if (GetResourceProtocolAndPath (resource_s, &protocol_s, &path_s)) */

		}		/* if (resource_s) */

	return NULL;
}


Resource *AllocateResource (const char *protocol_s, const char *value_s, const char * const title_s)
{
	Resource *resource_p = (Resource *) AllocMemory (sizeof (Resource));

	if (resource_p)
		{
			InitResource (resource_p);

			if (!SetResourceValue (resource_p, protocol_s, value_s, title_s))
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
	resource_p -> re_data_p = NULL;
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

	if (resource_p -> re_title_s)
		{
			FreeCopiedString (resource_p -> re_title_s);
			resource_p -> re_title_s = NULL;
		}

	if ((resource_p -> re_data_p) && (resource_p -> re_owns_data_flag))
		{
			json_object_clear (resource_p -> re_data_p);
			json_decref (resource_p -> re_data_p);
		}
}


bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s, const char *title_s)
{
	bool success_flag = false;
	char *new_protocol_s = NULL;

	if (ReplaceValue (protocol_s, &new_protocol_s))
		{
			char *new_value_s = NULL;

			if (ReplaceValue (value_s, &new_value_s))
				{
					char *new_title_s = NULL;

					if (ReplaceValue (title_s, &new_title_s))
						{
							resource_p -> re_protocol_s = new_protocol_s;
							resource_p -> re_value_s = new_value_s;
							resource_p -> re_title_s = new_title_s;

							success_flag = true;
						}
				}
		}

	return success_flag;
}


bool SetResourceData (Resource *resource_p, json_t *data_p, const bool owns_data_flag)
{
	bool success_flag = false;

	if (resource_p -> re_data_p)
		{
			if (resource_p -> re_owns_data_flag)
				{
					success_flag = (json_object_clear (resource_p -> re_data_p) == 0);
					json_decref (resource_p -> re_data_p);
				}
		}

	resource_p -> re_data_p = data_p;
	resource_p -> re_owns_data_flag = owns_data_flag;

	return success_flag;
}


Resource *CloneResource (const Resource * const src_p)
{
	Resource *resource_p = (Resource *) AllocMemory (sizeof (Resource));

	if (resource_p)
		{
			if (CopyResource (src_p, resource_p))
				{
					return resource_p;
				}

			FreeResource (resource_p);
		}		/* if (resource_p) */

	return NULL;
}


bool CopyResource (const Resource * const src_p, Resource * const dest_p)
{
	bool success_flag = SetResourceValue (dest_p, src_p -> re_protocol_s, src_p -> re_value_s, src_p -> re_title_s);

	if (src_p -> re_data_p)
		{
			json_t *copy_p = json_deep_copy (src_p -> re_data_p);

			if (copy_p)
				{
					if (dest_p -> re_data_p)
						{
							json_decref (dest_p -> re_data_p);
						}

					dest_p -> re_data_p = copy_p;
				}
			else
				{
					success_flag = false;
				}
		}

	return success_flag;
}


json_t *GetResourceAsJSONByParts (const char * const protocol_s, const char * const path_s, const char *title_s, json_t *data_p)
{
	json_t *json_p = json_object ();

	if (json_p)
		{
			if (json_object_set_new (json_p, RESOURCE_PROTOCOL_S, json_string (protocol_s)) == 0)
				{
					if ((path_s == NULL) || (json_object_set_new (json_p, RESOURCE_VALUE_S, json_string (path_s)) == 0))
						{
							if ((title_s == NULL)  || (json_object_set_new (json_p, RESOURCE_TITLE_S, json_string (title_s)) == 0))
								{
									if ((data_p == NULL) || (json_object_set (json_p, RESOURCE_DATA_S, data_p) == 0))
										{
											return json_p;
										}
								}
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
