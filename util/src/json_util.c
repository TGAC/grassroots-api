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
