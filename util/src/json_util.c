
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
					json_free_t (json_s);
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



bool SetJSONHTML (json_t *json_p, const char *key_s, const char *html_s)
{
	bool success_flag = false;


	return success_flag;
}

