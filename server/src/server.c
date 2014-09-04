#include "server.h"



char *ProcessMessage (const char * const request_s)
{
	char *response_s = NULL;
	json_error_t error;
	json_t *json_req_p = NULL;
	

	json_t *json_req_p = json_loads (request_s, 0, &error);
	
	if (json_req_p)
		{
			
		}
	else
		{
			/* error decoding the request */
			
		}
	
	
	return response_s;
}
