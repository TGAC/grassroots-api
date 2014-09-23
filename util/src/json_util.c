#include "json_util.h"


int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s)
{
	int result = 0;
	char *json_s = json_dumps (json_p, JSON_INDENT (2));
	
	if (prefix_s)
		{
			fprintf (out_f, "%s", prefix_s);
		}
	
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
					
	return result;
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

