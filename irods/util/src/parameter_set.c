#include "parameter_set.h"


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
			node_p -> pn_node.ln_succ_p = NULL;
			
			node_p -> pn_parameter_p = param_p;
		}		/* if (node_p) */
		
	return node_p;
}


void FreeParameterNode (ListNode *node_p)
{
	ParameterNode *param_node_p = (ParameterNode *) node_p;
	
	FreeMemory (param_node_p -> param_p);
	FreeMemory (param_node_p);
}
