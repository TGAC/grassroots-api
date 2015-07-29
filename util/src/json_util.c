
#define ALLOCATE_JSON_TAGS
#include "json_util.h"
#include "memory_allocations.h"
#include "streams.h"

JsonNode *AllocateJsonNode (json_t *json_p)
{
	JsonNode *json_node_p = (JsonNode *) AllocMemory (sizeof (JsonNode));
	
	if (json_node_p)
		{
			json_node_p -> jn_json_p = json_p;
		}
	
	return json_node_p;
}


void FreeJsonNode (ListItem *node_p)
{
	JsonNode *json_node_p = (JsonNode *) node_p;

	json_decref (json_node_p -> jn_json_p);
	FreeMemory (json_node_p);
}


int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s)
{
	int result = 0;
	char *json_s = NULL;
	
	if (prefix_s)
		{
			fprintf (out_f, "%s", prefix_s);
		}
	
	if (json_p)
		{
			json_s = json_dumps (json_p, JSON_INDENT (2));

			if (json_s)
				{
					fprintf (out_f, "%s\n", json_s);
					free (json_s);
				}
			else
				{
					fprintf (out_f, "ERROR DUMPING!!\n");
					result = -1;
				}
		}
	else
		{
			fprintf (out_f, "NULL JSON OBJECT!!\n");
		}
	
					
	return result;
}


json_t *LoadJSONConfig (const char * const filename_s)
{
	json_t *config_p = NULL;
	FILE *config_f = fopen (filename_s, "r");

	if (config_f)
		{
			json_error_t error;

			config_p = json_loadf (config_f, 0, &error);

			if (!config_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to parse %s, error at line %d column %d\n", filename_s, error.line, error.column);
				}

			if (fclose (config_f) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to close service config file \"%s\"", filename_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to open service config file \"%s\"", filename_s);
		}


	return config_p;
}



const char *GetJSONString (const json_t *json_p, const char * const key_s)
{
	const char *value_s = NULL;
	json_t *value_p = json_object_get (json_p, key_s);

	if (value_p)
		{
			if (json_is_string (value_p))
				{
					value_s = json_string_value (value_p);
				}
		}
	
	return value_s;
}



bool GetJSONInteger (const json_t *json_p, const char * const key_s, int *value_p)
{
	bool success_flag = false;
	json_t *json_value_p = json_object_get (json_p, key_s);

	if (json_value_p)
		{
			if (json_is_integer (json_value_p))
				{
					*value_p = json_integer_value (json_value_p);
					success_flag = true;
				}
		}

	return success_flag;

}


bool SetJSONHTML (json_t *json_p, const char *key_s, const char *html_s)
{
	bool success_flag = false;


	return success_flag;
}



json_t *ConvertTabularDataToJSON (const char *data_s, const char column_delimiter, const char row_delimiter)
{
	json_t *json_values_p = NULL;
	const char *current_row_s = data_s;
	const char *next_row_s;
	int col = 0;
	bool loop_flag = true;
	bool valid_row_flag = false;
	bool success_flag = true;
	LinkedList *headers_p = AllocateLinkedList (FreeStringListNode);

	if (headers_p)
		{
			/* Get the keys from the first row */
			next_row_s = strchr (current_row_s, row_delimiter);

			if (next_row_s)
				{
					const char *current_token_s = current_row_s;
					const char *next_token_s = NULL;

					while (loop_flag && success_flag)
						{
							char *value_s = NULL;
							bool alloc_flag = false;

							next_token_s = strchr (current_token_s, column_delimiter);

							if (next_token_s)
								{
									value_s = CopyToNewString (current_token_s, next_token_s - current_token_s, false);

									if (value_s)
										{
											StringListNode *node_p = AllocateStringListNode (value_s, MF_SHALLOW_COPY);

											if (node_p)
												{
													LinkedListAddTail (headers_p, (const ListItem *) node_p);
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, "Failed to allocate header node for %s", value_s);
													success_flag = false;
													FreeCopiedString (value_s);
												}
										}		/* if (value_s) */

									current_token_s = next_token_s + 1;
								}
							else
								{
									StringListNode *node_p = AllocateStringListNode (current_token_s, MF_DEEP_COPY);

									if (node_p)
										{
											LinkedListAddTail (headers_p, (const ListItem *) node_p);
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, "Failed to allocate header node for %s", current_token_s);
											success_flag = false;
										}

									loop_flag = false;
								}
						}		/* while (loop_flag && success_flag) */

				}		/* if (next_row_s) */

			/* Did we get the headers successfully? */
			if (success_flag)
				{

				}


			FreeLinkedList (headers_p);
		}		/* if (headers_p) */


	return json_values_p;
}


