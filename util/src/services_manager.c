

#include "services_manager.h"
#include "service.h"

json_t *GetServicesListAsJSON (LinkedList *services_list_p)
{
	json_t *root_p = NULL;

	if (services_list_p)
		{
			root_p = json_object ();
			
			if (root_p)
				{
					json_t *operations_p = json_object ();
					
					if (operations_p)
						{
							ServiceNode *service_node_p = (ServiceNode *) (services_list_p -> ll_head_p);
							bool success_flag = true;

							success_flag = (json_object_set (root_p, "operations", operations_p) == 0);
							json_decref (operations_p);
							
							while (success_flag && service_node_p)
								{
									json_t *service_json_p = GetServiceAsJSON (service_node_p -> sn_service_p);
									
									if (service_json_p)
										{
											success_flag = (json_object_set (operations_p, "apis", service_json_p) == 0);
											json_decref (service_json_p);
											
											service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
										}
									else
										{
											success_flag = false;
										}
								}
							
							
							if (!success_flag)
								{
									
								}
						}		/* if (operations_p) */
					else
						{
							json_decref (root_p);
							root_p = NULL;
						}
											
				}		/* if (root_p) */
			
		}		/* if (services_list_p) */

	return root_p;
}


void FreeServicesString (char *services_json_s)
{
	
}
