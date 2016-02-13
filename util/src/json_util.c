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

#define ALLOCATE_JSON_TAGS

#include "json_util.h"
#include "math_utils.h"
#include "memory_allocations.h"
#include "streams.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "byte_buffer.h"


#ifdef _DEBUG
	#define JSON_UTIL_DEBUG	(STM_LEVEL_FINE)
#else
	#define JSON_UTIL_DEBUG	(STM_LEVEL_NONE)
#endif


static void PrintJSONRefCountsWithIndent (const json_t * const value_p, const uint32 log_level, const char * key_s, ByteBuffer *buffer_p, const char * const filename_s, const int line_number);


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
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to parse %s, error at line %d column %d\n", filename_s, error.line, error.column);
				}

			if (fclose (config_f) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to close service config file \"%s\"", filename_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to open service config file \"%s\"", filename_s);
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


char *GetCopiedJSONString (const json_t *json_p, const char * const key_s)
{
	char *dest_s = NULL;
	const char *src_s = GetJSONString (json_p, key_s);

	if (src_s)
		{
			dest_s = CopyToNewString (src_s, 0, false);

			if (!dest_s)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to copy \"%s\" for key \"%s\"", src_s, key_s);
				}
		}

	return dest_s;
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


bool GetJSONLong (const json_t *json_p, const char * const key_s, long *value_p)
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


bool GetJSONBoolean (const json_t *json_p, const char * const key_s, bool *value_p)
{
	bool success_flag = false;
	json_t *json_value_p = json_object_get (json_p, key_s);

	if (json_value_p)
		{
			if (json_is_boolean (json_value_p))
				{
					if (json_value_p == json_true ())
						{
							*value_p = true;
							success_flag = true;
						}
					else if (json_value_p == json_false ())
						{
							*value_p = false;
							success_flag = true;
						}
				}
		}

	return success_flag;
}



bool SetJSONHTML (json_t *json_p, const char *key_s, const char *html_s)
{
	bool success_flag = false;


	return success_flag;
}


bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s)
{
	bool success_flag = true;

	if (value_s)
		{
			if (json_object_set_new (parent_p, key_s, json_string (value_s)) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set \"%s\": \"%s\" for json", key_s, value_s);
					success_flag = false;
				}
		}

	return success_flag;
}



bool AddStringArrayToJSON (json_t *parent_p, const char ** const values_ss, const char * const child_key_s)
{
	bool success_flag = false;
	json_t *child_p = json_array ();

	if (child_p)
		{
			const char **value_ss = values_ss;
			bool loop_flag = (*value_ss != NULL);
			bool add_flag = true;

			/* Populate the child array */
			while (loop_flag && add_flag)
				{
					if (json_array_append_new (child_p, json_string (*value_ss)) == 0)
						{
							++ value_ss;
							loop_flag = (*value_ss != NULL);
						}		/* if (json_array_append_new (child_p, json_string (*email_ss)) == 0) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add value \"%s\" to array", *value_ss);
							add_flag = false;
						}

				}		/* while (loop_flag && add_flag) */

			if (add_flag)
				{
					/* Is there anything to add? */
					if (json_array_size (child_p) > 0)
						{
							if (json_object_set_new (parent_p, child_key_s, child_p) == 0)
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add array for \"%s\" to drmaa tool json", child_key_s);
									json_decref (child_p);
									add_flag = false;
								}
						}		/* if (json_array_size (child_p) > 0) */
					else
						{
							json_decref (child_p);
						}
				}		/* if (add_flag) */

		}		/* if (child_p) */

	return success_flag;
}


char **GetStringArrayFromJSON (const json_t * const array_p)
{
	char **array_ss = NULL;

	if (json_is_array (array_p))
		{
			size_t num_values = json_array_size (array_p);

			if (num_values > 0)
				{
					array_ss = (char **) AllocMemoryArray (num_values + 1, sizeof (char *));

					if (array_ss)
						{
							size_t i = 0;
							bool loop_flag = true;
							bool success_flag = true;
							char **dest_ss = array_ss;

							while (loop_flag && success_flag)
								{
									json_t *value_p = json_array_get (array_p, i);

									if (json_is_string (value_p))
										{
											const char *src_s = json_string_value (value_p);

											*dest_ss = CopyToNewString (src_s, 0, false);

											if (*dest_ss)
												{
													++ i;

													if (i < num_values)
														{
															++ dest_ss;
														}
													else
														{
															loop_flag = false;
														}

												}		/* if (*dest_ss) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy \"%s\"", src_s);
													success_flag = false;
												}

										}		/* if (json_is_string (value_p)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "value is %d not a string", json_typeof (value_p));
											success_flag = false;
										}

								}		/* while (loop_flag && success_flag) */

							if (!success_flag)
								{
									dest_ss = array_ss;

									while (*dest_ss)
										{
											FreeCopiedString (*dest_ss);
											++ dest_ss;
										}

									FreeMemory (array_ss);
									array_ss = NULL;
								}
						}		/* if (array_ss) */

				}		/* if (num_values > 0) */

		}		/* if (json_is_array (array_p)) */

	return array_ss;
}



LinkedList *GetStringListFromJSON (const json_t * const array_p)
{
	LinkedList *values_list_p = NULL;

	if (json_is_array (array_p))
		{
			size_t num_values = json_array_size (array_p);

			if (num_values > 0)
				{
					values_list_p = AllocateLinkedList (FreeStringListNode);

					if (values_list_p)
						{
							size_t i = 0;
							bool loop_flag = true;
							bool success_flag = true;

							while (loop_flag && success_flag)
								{
									json_t *value_p = json_array_get (array_p, i);

									if (json_is_string (value_p))
										{
											const char *src_s = json_string_value (value_p);
											StringListNode *node_p = AllocateStringListNode (src_s, MF_DEEP_COPY);

											if (node_p)
												{
													LinkedListAddTail (values_list_p, (ListItem *) node_p);
													++ i;

													if (i == num_values)
														{
															loop_flag = false;
														}

												}		/* if (node_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy \"%s\"", src_s);
													success_flag = false;
												}

										}		/* if (json_is_string (value_p)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "value is %d not a string", json_typeof (value_p));
											success_flag = false;
										}

								}		/* while (loop_flag && success_flag) */

							if (!success_flag)
								{
									FreeLinkedList (values_list_p);
								}

						}		/* if (values_list_p) */

				}		/* if (num_values > 0) */

		}		/* if (json_is_array (array_p)) */

	return values_list_p;
}



bool AddStringListToJSON (json_t *parent_p, LinkedList *values_p, const char * const child_key_s)
{
	bool success_flag = false;

	if (values_p && (values_p -> ll_size > 0))
		{
			json_t *child_p = json_array ();

			if (child_p)
				{
					StringListNode *node_p = (StringListNode *) (values_p -> ll_head_p);
					bool loop_flag = (node_p != NULL);
					bool add_flag = true;

					/* Populate the child array */
					while (loop_flag && add_flag)
						{
							if (json_array_append_new (child_p, json_string (node_p -> sln_string_s)) == 0)
								{
									node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
									loop_flag = (node_p != NULL);
								}		/* if (json_array_append_new (child_p, json_string (node_p -> sln_string_s)) == 0) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add value \"%s\" to array", node_p -> sln_string_s);
									add_flag = false;
								}

						}		/* while (loop_flag && add_flag) */

					if (add_flag)
						{
							/* Is there anything to add? */
							if (json_array_size (child_p) > 0)
								{
									if (json_object_set_new (parent_p, child_key_s, child_p) == 0)
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add array for \"%s\" to drmaa tool json", child_key_s);
											json_decref (child_p);
											add_flag = false;
										}
								}		/* if (json_array_size (child_p) > 0) */
							else
								{
									json_decref (child_p);
								}
						}		/* if (add_flag) */

				}		/* if (child_p) */

		}		/* if (values_p && (values_p -> ll_size > 0)) */
	else
		{
			success_flag = true;
		}

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
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate header node for %s", value_s);
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
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate header node for %s", current_token_s);
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
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s to json rows", current_row_s);
														}

													#if JSON_UTIL_DEBUG >= STM_LEVEL_FINE
													PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "row: %s", current_row_s);
													PrintJSONToLog (row_p, "row_p ", STM_LEVEL_FINE, __FILE__, __LINE__);
													PrintJSONToLog (json_values_p, "json_values_p ", STM_LEVEL_FINE, __FILE__, __LINE__);
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
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s to json rows", current_row_s);
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

					if (GetValidInteger (&value_s, &i, NULL))
						{
							value_p = json_integer (i);
						}
				}
				break;

			case JSON_REAL:
				{
					double d;
					const char **value_ss= &value_s;

					if (GetValidRealNumber (value_ss, &d, NULL))
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

					header_p = (FieldNode *) (header_p -> fn_base_node.sln_node.ln_next_p);
				}		/* while (header_p) */

		}		/* if (row_json_p) */

	return row_json_p;
}



void PrintJSONRefCounts (const json_t * const value_p, const char *initial_s, const uint32 log_level, const char * const filename_s, const int line_number)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (128);

	if (buffer_p)
		{
			if (initial_s)
				{
					PrintLog (log_level, filename_s, line_number, "%s", initial_s);
				}
			PrintJSONRefCountsWithIndent (value_p, log_level, NULL, buffer_p, filename_s, line_number);
			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */
}



void PrintJSONToError (const json_t *json_p, const char * const prefix_s, const uint32 level, const char *filename_s, const int line_number)
{
	if (json_p)
		{
			char *json_s = json_dumps (json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (json_s)
				{
					if (prefix_s)
						{
							PrintErrors (level, filename_s, line_number, "%s %s", prefix_s, json_s);
						}
					else
						{
							PrintErrors (level, filename_s, line_number, "%s", json_s);
						}

					free (json_s);
				}
		}
}


void PrintJSONToLog (const json_t *json_p, const char * const prefix_s, const uint32 level, const char *filename_s, const int line_number)
{
	if (json_p)
		{
			char *json_s = json_dumps (json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (json_s)
				{
					if (prefix_s)
						{
							PrintLog (level, filename_s, line_number, "%s %s", prefix_s, json_s);
						}
					else
						{
							PrintLog (level, filename_s, line_number, "%s", json_s);
						}

					free (json_s);
				}
		}
}


bool IsJSONEmpty (const json_t *json_p)
{
	bool empty_flag = false;

	if (json_p)
		{
			switch (json_p -> type)
				{
					case JSON_ARRAY:
						{
							size_t i = json_array_size (json_p);
							empty_flag = (i == 0);
						}
						break;

					case JSON_OBJECT:
						{
							size_t i = json_object_size (json_p);
							empty_flag = (i == 0);
						}
						break;

					case JSON_STRING:
						{
							const char *value_s = json_string_value (json_p);
							empty_flag = IsStringEmpty (value_s);
						}
						break;

					default:
						break;
				}		/* switch (json_p -> type) */

		}		/* if (json_p) */
	else
		{
			empty_flag = true;
		}

	return empty_flag;
}



static void PrintJSONRefCountsWithIndent (const json_t * const value_p, const uint32 log_level, const char * key_s, ByteBuffer *buffer_p, const char * const filename_s, const int line_number)
{
	if (value_p)
		{
			const char * const indent_s = "\t";
			const char *buffer_data_s = GetByteBufferData (buffer_p);

			if (!key_s)
				{
					key_s = "";
				}

			if (json_is_object (value_p))
				{
					const char *element_key_s;
					json_t *element_value_p;
					size_t size = json_object_size (value_p);

					PrintLog (log_level, filename_s, line_number, "%s%s obj refcount " SIZET_FMT, buffer_data_s, key_s, value_p -> refcount);

					if (size > 0)
						{
							PrintLog (log_level, filename_s, line_number, "%s{", buffer_data_s);

							if (AppendStringToByteBuffer (buffer_p, "\t"))
								{
									json_object_foreach (value_p, element_key_s, element_value_p)
										{
											PrintJSONRefCountsWithIndent (element_value_p, log_level, element_key_s, buffer_p, filename_s, line_number);
										}

									RemoveFromByteBuffer (buffer_p, strlen (indent_s));
								}

							PrintLog (log_level, filename_s, line_number, "%s}", buffer_data_s);
						}		/* if (json_object_size (value_p) > 0) */
				}
			else if (json_is_array (value_p))
				{
					size_t i;
					json_t *element_p;
					size_t size = json_array_size (value_p);

					PrintLog (log_level, filename_s, line_number, "%s%s array refcount " SIZET_FMT, buffer_data_s, key_s, value_p -> refcount);

					if (size > 0)
						{
							PrintLog (log_level, filename_s, line_number, "%s[", buffer_data_s);

							if (AppendStringToByteBuffer (buffer_p, indent_s))
								{
									json_array_foreach (value_p, i, element_p)
										{
											PrintJSONRefCountsWithIndent (element_p, log_level, NULL,  buffer_p, filename_s, line_number);
										}

									RemoveFromByteBuffer (buffer_p, strlen (indent_s));
								}

							PrintLog (log_level, filename_s, line_number, "%s]", buffer_data_s);
						}		/* if (json_object_size (value_p) > 0) */
				}
			else
				{

				}
		}
}

