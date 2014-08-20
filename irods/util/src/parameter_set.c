#include "memory_allocations.h"
#include "parameter_set.h"
#include "parameter.h"

#include "json_util.h"

static ParameterNode *AllocateParameterNode (Parameter *param_p);
static void FreeParameterNode (ListNode *node_p);


ParameterSet *AllocateParameterSet (const char *name_s, const char *description_s)
{
	ParameterSet *set_p = AllocMemory (sizeof (ParameterSet));
	
	if (set_p)
		{
			LinkedList *params_list_p = AllocateLinkedList (FreeParameterNode);
			
			if (params_list_p)
				{
					set_p -> ps_params_p = params_list_p;
					set_p -> ps_name_s = name_s;
					set_p -> ps_description_s = description_s;
					
					return set_p;
				}		/* if (params_list_p) */

			FreeMemory (set_p);
		}		/* if (set_p) */
		
		
	return NULL;
}


void FreeParameterSet (ParameterSet *params_p)
{
	if (params_p -> ps_params_p)
		{
			FreeLinkedList (params_p -> ps_params_p);
		}
		
	FreeMemory (params_p);
}



bool CreateAndAddParameterToParameterSet (ParameterSet *params_p, ParameterType type, 
	const char * const name_s, const char * const description_s, ParameterMultiOptionArray *options_p, 
	SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, 
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	bool success_flag = false;
	Parameter *param_p = AllocateParameter (type, name_s, description_s, options_p, default_value, bounds_p, level, check_value_fn);
	
	if (param_p)
		{
			if (AddParameterToParameterSet (params_p, param_p))
				{				
					success_flag = true;
				}		/* if (AddParameterToParameterSet) */
			else
				{
					FreeParameter (param_p);
				}
				
		}		/* if (param_p) */

	return success_flag;
}



bool AddParameterToParameterSet (ParameterSet *params_p, Parameter *param_p)
{
	bool success_flag = false;
	ParameterNode *node_p = AllocateParameterNode (param_p); 
	
	if (node_p)
		{
			LinkedListAddTail (params_p -> ps_params_p, (ListNode *) node_p);
			success_flag = true;
		}		/* if (node_p) */
	
	return success_flag;
}


ParameterNode *AllocateParameterNode (Parameter *param_p)
{
	ParameterNode *node_p = (ParameterNode *) AllocMemory (sizeof (ParameterNode));
	
	if (node_p)
		{
			node_p -> pn_node.ln_prev_p = NULL;
			node_p -> pn_node.ln_next_p = NULL;
			
			node_p -> pn_parameter_p = param_p;
		}		/* if (node_p) */
		
	return node_p;
}


void FreeParameterNode (ListNode *node_p)
{
	ParameterNode *param_node_p = (ParameterNode *) node_p;
	
	FreeMemory (param_node_p -> pn_parameter_p);
	FreeMemory (param_node_p);
}


json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p)
{
	json_t *root_p = json_array ();

	if (root_p)
		{
			ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			bool success_flag = true;
			
			while (success_flag && node_p)
				{
					json_t *param_json_p = GetParameterAsJSON (node_p -> pn_parameter_p);
				
					if (param_json_p)
						{
							#ifdef _DEBUG
							PrintJSON (stderr, root_p, "GetParameterSetAsJSON - param_json_p :: ");
							#endif

							success_flag = (json_array_append (root_p, param_json_p) == 0);
							json_decref (param_json_p);
							
							node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
						}
					else
						{
							success_flag = false;
						}
				}
				
			
			if (!success_flag)
				{
					json_object_clear (root_p);
					json_decref (root_p);
					root_p = NULL;
				}
			
		}		/* if (root_p) */


	return root_p;
}
