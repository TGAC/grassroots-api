

#include "services_manager.h"
#include "service.h"
#include "plugin.h"
#include "filesystem_utils.h"
#include "string_linked_list.h"
#include "string_utils.h"




json_t *GetServicesListAsJSON (LinkedList *services_list_p)
{
	json_t *services_list_json_p = json_array ();
			
	if (services_list_json_p)
		{
			bool success_flag = true;
						
			if (services_list_json_p)
				{
					if (services_list_p)
						{
							ServiceNode *service_node_p = (ServiceNode *) (services_list_p -> ll_head_p);

							while (success_flag && service_node_p)
								{
									json_t *service_json_p = GetServiceAsJSON (service_node_p -> sn_service_p);
									
									if (service_json_p)
										{
											success_flag = (json_array_append_new (services_list_json_p, service_json_p) == 0);
											
											service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
										}
									else
										{
											success_flag = false;
										}
								}

						}		/* if (services_list_p) */

					if (!success_flag)
						{
							json_array_clear (services_list_json_p);
							json_decref (services_list_json_p);
							services_list_json_p = NULL;
						}
															
				}		/* if (operations_p) */
				
		}		/* if (services_list_json_p) */

	return services_list_json_p;
}


void FreeServicesString (char *services_json_s)
{
	
}
