#include <string.h>

#define ALLOCATE_JSON_TAGS

#include "json_util.h"
#include "memory_allocations.h"
#include "streams.h"
#include "string_linked_list.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define JSON_UTIL_DEBUG	(STM_LEVEL_FINE)
#else
	#define JSON_UTIL_DEBUG	(STM_LEVEL_NONE)
#endif

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


FieldNode *AllocateFieldNode (const char *name_s, const MEM_FLAG mf, json_type field_type)
{
	FieldNode *node_p = (FieldNode *) AllocMemory (sizeof (FieldNode));

	if (node_p)
		{
			if (InitStringListNode (& (node_p -> fn_base_node), name_s, mf))
				{
					node_p -> fn_type = field_type;

					return node_p;
				}

			FreeMemory (node_p);
		}

	return NULL;
}



void FreeFieldNode (ListItem *node_p)
{
	FieldNode *field_node_p = (FieldNode *) node_p;

	ClearStringListNode (& (field_node_p -> fn_base_node));

	FreeMemory (field_node_p);
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


bool GetJSONReal (const json_t *json_p, const char * const key_s, double *value_p)
{
	bool success_flag = false;
	json_t *json_value_p = json_object_get (json_p, key_s);

	if (json_value_p)
		{
			if (json_is_real (json_value_p))
				{
					*value_p = json_real_value (json_value_p);
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




json_t *ConvertTabularDataToJSON (char *data_s, const char column_delimiter, const char row_delimiter, json_type (*get_type_fn) (const char *name_s, const void * const data_p), const void * const type_data_p)
{
	json_t *json_values_p = NULL;
	char *current_row_s = data_s;
	char *next_row_s;
	bool loop_flag = true;
	bool success_flag = true;
	LinkedList *headers_p = AllocateLinkedList (FreeFieldNode);

	if (headers_p)
		{
			/* Get the keys from the first row */
			next_row_s = strchr (current_row_s, row_delimiter);

			if (next_row_s)
				{
					char *current_token_s = current_row_s;
					char *next_token_s = NULL;

					/*
					 * Temporarily terminate the current row to treat it as string
					 */
					*next_row_s = '\0';

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
											FieldNode *node_p = NULL;
											json_type field_type = JSON_STRING;

											if (get_type_fn)
												{
													field_type = get_type_fn (value_s, type_data_p);
												}

											node_p = AllocateFieldNode (value_s, MF_SHALLOW_COPY, field_type);

											if (node_p)
												{
													LinkedListAddTail (headers_p, (ListItem *) node_p);
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
											LinkedListAddTail (headers_p, (ListItem *) node_p);
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, "Failed to allocate header node for %s", current_token_s);
											success_flag = false;
										}

									loop_flag = false;
								}
						}		/* while (loop_flag && success_flag) */

					*next_row_s = row_delimiter;
					current_row_s = next_row_s + 1;
				}		/* if (next_row_s) */

			/* Did we get the headers successfully? */
			if (success_flag)
				{
					json_values_p = json_array ();

					if (json_values_p)
						{
							loop_flag = true;

							/* Now loop through each row creating a json object for it */
							while (loop_flag && success_flag)
								{
									next_row_s = strchr (current_row_s, row_delimiter);

									if (next_row_s)
										{
											json_t *row_p;

											/*
											 * Temporarily terminate the current row to treat it as string
											 */
											*next_row_s = '\0';
											row_p = ConvertRowToJSON (current_row_s, headers_p, column_delimiter);

											if (row_p)
												{
													if (json_array_append_new (json_values_p, row_p) != 0)
														{
															PrintErrors (STM_LEVEL_WARNING, "Failed to add %s to json rows", current_row_s);
														}

													#if JSON_UTIL_DEBUG >= STM_LEVEL_FINE
													PrintLog (STM_LEVEL_FINE, "row: %s", current_row_s);
													PrintJSONToLog (row_p, "row_p ", STM_LEVEL_FINE);
													PrintJSONToLog (json_values_p, "json_values_p ", STM_LEVEL_FINE);
													#endif
												}

											*next_row_s = row_delimiter;
											current_row_s = next_row_s + 1;
										}		/* if (next_row_s) */
									else
										{
											json_t *row_p;

											row_p = ConvertRowToJSON (current_row_s, headers_p, column_delimiter);

											if (row_p)
												{
													if (json_array_append_new (json_values_p, row_p) != 0)
														{
															PrintErrors (STM_LEVEL_WARNING, "Failed to add %s to json rows", current_row_s);
														}
												}

											loop_flag = false;
										}
								}		/* while (loop_flag && success_flag) */

						}		/* if (json_values_p) */

				}		/* if (success_flag) */

			FreeLinkedList (headers_p);
		}		/* if (headers_p) */


	return json_values_p;
}


json_t *GetJSONFromString (const char * const value_s, json_type field_type)
{
	json_t *value_p = NULL;

	switch (field_type)
		{
			case JSON_STRING:
				value_p = json_string (value_s);
				break;

			case JSON_INTEGER:
				{
					int i;

					if (GetValidInteger (&value_s, &i))
						{
							value_p = json_integer (i);
						}
				}
				break;

			case JSON_REAL:
				{
					double d;

					if (GetValidRealNumber (&value_s, &d))
						{
							value_p = json_real (d);
						}
				}
				break;

			case JSON_TRUE:
			case JSON_FALSE:
				if (Stricmp (value_s, "true") == 0)
					{
						value_p = json_true ();
					}
				else if (Stricmp (value_s, "false") == 0)
					{
						value_p = json_false ();
					}
				break;

			case JSON_OBJECT:
			case JSON_ARRAY:
			case JSON_NULL:
				break;
		}

	return value_p;
}


json_t *ConvertRowToJSON (char *row_s, LinkedList *headers_p, const char delimiter)
{
	json_t *row_json_p = json_object ();

	if (row_json_p)
		{
			FieldNode *header_p = (FieldNode *) (headers_p -> ll_head_p);
			char *current_token_s = row_s;
			char *next_token_s = NULL;

			while (header_p)
				{
					if (*current_token_s == delimiter)
						{
							++ current_token_s;
						}
					else
						{
							next_token_s = strchr (current_token_s, delimiter);

							if (next_token_s)
								{
									int res = -1;
									json_t *value_p = NULL;

									*next_token_s = '\0';

									value_p = GetJSONFromString (current_token_s, header_p -> fn_type);

									if (value_p)
										{
											res = json_object_set_new (row_json_p, header_p -> fn_base_node.sln_string_s, value_p);
										}

									*next_token_s = delimiter;

									if (res != 0)
										{

										}

									current_token_s = next_token_s + 1;
								}
							else
								{

								}
						}

					header_p = (StringListNode *) (header_p -> fn_base_node.sln_node.ln_next_p);
				}		/* while (header_p) */

		}		/* if (row_json_p) */

	return row_json_p;
}


void PrintJSONToLog (const json_t *json_p, const char * const prefix_s, const uint32 level)
{
	char *json_s = json_dumps (json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

	if (json_s)
		{
			if (prefix_s)
				{
					PrintLog (level, "%s %s", prefix_s, json_s);
				}
			else
				{
					PrintLog (level, "%s", json_s);
				}

			free (json_s);
		}
}
