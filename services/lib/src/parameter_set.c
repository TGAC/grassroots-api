#include "memory_allocations.h"
#include "parameter_set.h"
#include "parameter.h"

#include "json_util.h"
#include "tag_item.h"

static ParameterNode *AllocateParameterNode (Parameter *param_p);
static void FreeParameterNode (ListItem *node_p);


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
	const char * const name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, 
	SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, 
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	bool success_flag = false;
	Parameter *param_p = AllocateParameter (type, name_s, description_s, tag, options_p, default_value, bounds_p, level, check_value_fn);
	
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
			LinkedListAddTail (params_p -> ps_params_p, (ListItem *) node_p);
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


void FreeParameterNode (ListItem *node_p)
{
	ParameterNode *param_node_p = (ParameterNode *) node_p;
	
	FreeMemory (param_node_p -> pn_parameter_p);
	FreeMemory (param_node_p);
}


json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p, const bool full_definition_flag)
{
	json_t *root_p = json_array ();

	if (root_p)
		{
			ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			bool success_flag = true;
			
			while (success_flag && node_p)
				{
					json_t *param_json_p = GetParameterAsJSON (node_p -> pn_parameter_p, full_definition_flag);
				
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



uint32 GetCurrentParameterValues (const ParameterSet * const params_p, TagItem *tags_p)
{
	uint32 matched_count = 0;
	ParameterNode *node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);
	
	while (node_p)
		{
			Parameter *param_p = node_p -> pn_parameter_p;
			TagItem *tag_p = FindMatchingTag (tags_p, param_p -> pa_tag);
			
			if (tag_p)
				{
					tag_p -> ti_value = param_p -> pa_current_value;
					++ matched_count;
				}

			node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
		}		/* while (node_p) */
		
	return matched_count;
}


Parameter *GetParameterFromParameterSetByTag (const ParameterSet * const params_p, const Tag tag)
{
	ParameterNode *node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);
	
	while (node_p)
		{
			Parameter *param_p = node_p -> pn_parameter_p;
						
			if (param_p -> pa_tag == tag)
				{
					return param_p;
				}
			else
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}		/* while (node_p) */
		
	return NULL;	
}



bool GetParameterValueFromParameterSet (const ParameterSet * const params_p, const Tag tag, SharedType *value_p, const bool current_value_flag)
{
	bool success_flag = false;
	Parameter *param_p = GetParameterFromParameterSetByTag (params_p, tag);

	if (param_p)
		{
			*value_p = current_value_flag ? param_p -> pa_current_value : param_p -> pa_default;
			success_flag = true;
		}
		
	return success_flag;
	
}



ParameterSet *CreateParameterSetFromJSON (const json_t * const root_p)
{
	ParameterSet *params_p =  NULL;

	if (root_p)
		{
			const char *name_s = NULL;
			const char *description_s = NULL;
			#ifdef _DEBUG
			char *root_s = json_dumps (root_p, JSON_INDENT (2));
			#endif
			
			/* Get the name */
			json_t *json_p = json_object_get (root_p, PARAM_SET_NAME_S);
			if (json_p && json_is_string (json_p))
				{
					name_s = json_string_value (json_p);
				}

			/* Get the description */
			json_p = json_object_get (root_p, PARAM_SET_DESCRIPTION_S);
			if (json_p && json_is_string (json_p))
				{
					description_s = json_string_value (json_p);
				}

			params_p = AllocateParameterSet (name_s, description_s);
			
			if (params_p)
				{
					/* Get the parameters array */
					json_p = json_object_get (root_p, PARAM_SET_PARAMS_S);
					if (json_p && json_is_array (json_p))
						{
							size_t num_params = json_array_size (json_p);
							size_t i = 0;
							bool success_flag = true;
							
							/* Loop through the params */
							while ((i < num_params) && success_flag)
								{
									json_t *param_json_p = json_array_get (json_p, i);
									Parameter *param_p = CreateParameterFromJSON (param_json_p);
									
									if (param_p)
										{
											success_flag = AddParameterToParameterSet (params_p, param_p);
										}
									else
										{
											success_flag = false;
										}
										
									if (success_flag)
										{
											++ i;
										}									
								}		/* while ((i < num_params) && success_flag) */
							
							
							if (!success_flag)
								{
									FreeParameterSet (params_p);
									params_p = NULL;
								}
							
						}		/* if (json_p && json_is_array (json_p)) */
					
				}		/* if (params_p) */
			
			#ifdef DEBUG
			if (root_s)
				{
					free (root_s);
				}
			#endif
		
			
		}		/* if (root_p) */
	
	return params_p;
}
