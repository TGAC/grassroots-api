
#include "services_manager.h"
#include "service.h"

int main (int argc, char *argv [])
{
	int result = 0;
	const char * const path_s = "services";
	const char * const pattern_s = NULL;

	LinkedList *services_p = LoadMatchingServices (path_s, pattern_s);
	
	if (services_p)
		{
			json_t *services_json_p = GetServicesListAsJSON (services_p);
			
			if (services_json_p)
				{
					char *json_s = json_dumps (services_json_p, JSON_INDENT (2));
					
					if (json_s)
						{
							printf ("%s\n", json_s);
							json_free_t (json_s);
						}
					
				}		/* if (services_json_p) */
			 
			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return result;
}

