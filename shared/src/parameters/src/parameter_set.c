/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <string.h>

#include "memory_allocations.h"
#include "parameter_set.h"
#include "parameter.h"
#include "parameter_group.h"
#include "string_utils.h"
#include "service.h"

#include "streams.h"
#include "json_util.h"
#include "json_tools.h"
#include "tag_item.h"
#include "service.h"

#ifdef _DEBUG
	#define PARAMETER_SET_DEBUG	(STM_LEVEL_INFO)
#else
	#define PARAMETER_SET_DEBUG	(STM_LEVEL_NONE)
#endif


/****************************************/
/********** STATIC PROTOTYPES ***********/
/****************************************/



static bool AddAllParametersToParameterSetJSON (const Parameter *param_p, void *data_p);


/****************************************/
/********** PUBLIC FUNCTIONS ************/
/****************************************/

ParameterSet *AllocateParameterSet (const char *name_s, const char *description_s)
{
	ParameterSet *set_p = AllocMemory (sizeof (ParameterSet));
	
	if (set_p)
		{
			LinkedList *params_list_p = AllocateLinkedList (FreeParameterNode);
			
			if (params_list_p)
				{
					LinkedList *groups_list_p = AllocateLinkedList (FreeParameterGroupNode);

					if (groups_list_p)
						{
							set_p -> ps_params_p = params_list_p;
							set_p -> ps_name_s = name_s;
							set_p -> ps_description_s = description_s;
							set_p -> ps_grouped_params_p = groups_list_p;

							return set_p;
						}
					
					FreeLinkedList (params_list_p);
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

	if (params_p -> ps_grouped_params_p)
		{
			FreeLinkedList (params_p -> ps_grouped_params_p);
		}


	FreeMemory (params_p);
}



Parameter *CreateAndAddParameterToParameterSet (const ServiceData *service_data_p, ParameterSet *params_p, ParameterGroup *group_p, ParameterType type, bool multi_valued_flag,
	const char * const name_s, const char * const display_name_s, const char * const description_s, ParameterMultiOptionArray *options_p,
	SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, uint8 level,
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	Parameter *param_p = AllocateParameter (service_data_p, type, multi_valued_flag, name_s, display_name_s, description_s, options_p, default_value, current_value_p, bounds_p, level, check_value_fn);
	
	if (param_p)
		{
			if (group_p)
				{
					/*
					 * If the parameter fails to get added to the group, it's
					 * not a terminal error so still carry on
					 */
					if (!AddParameterToParameterGroup (group_p, param_p))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add param \"%s\" to group \"%s\"", param_p -> pa_name_s, group_p -> pg_name_s);
						}
				}

			if (!AddParameterToParameterSet (params_p, param_p))
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add param \"%s\" to set \"%s\"", param_p -> pa_name_s, params_p -> ps_name_s);
					FreeParameter (param_p);
					param_p = NULL;
				}
				
		}		/* if (param_p) */

	return param_p;
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


static bool AddAllParametersToParameterSetJSON (const Parameter * UNUSED_PARAM (param_p), void * UNUSED_PARAM (data_p))
{
	return true;
}


json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p, const SchemaVersion * const sv_p, const bool full_definition_flag)
{
	return GetParameterSetSelectionAsJSON (param_set_p, sv_p, full_definition_flag, NULL, AddAllParametersToParameterSetJSON);
}



json_t *GetParameterSetSelectionAsJSON (const ParameterSet * const param_set_p, const SchemaVersion * const sv_p, const bool full_definition_flag,  void *data_p, bool (*add_param_fn) (const Parameter *param_p, void *data_p))
{
	json_t *param_set_json_p = json_object ();

	if (param_set_json_p)
		{
			json_t *params_p = json_array ();

			if (params_p)
				{
					ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
					bool success_flag = true;

					while (success_flag && node_p)
						{
							Parameter *param_p = node_p -> pn_parameter_p;

							if (add_param_fn (param_p, data_p))
								{
									json_t *param_json_p = GetParameterAsJSON (param_p, sv_p, full_definition_flag);

									if (param_json_p)
										{
											#if PARAMETER_SET_DEBUG >= STM_LEVEL_FINER
											PrintJSONToLog (param_json_p, "GetParameterSetAsJSON - param_json_p :: ", STM_LEVEL_FINER, __FILE__, __LINE);
											#endif

											success_flag = (json_array_append_new (params_p, param_json_p) == 0);
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to JSON", param_p -> pa_name_s);
											success_flag = false;
										}
								}		/* if (add_param_fn (param_p)) */

							if (success_flag)
								{
									node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
								}

						}		/* while (success_flag && node_p) */

					if (success_flag)
						{
							if (json_object_set_new (param_set_json_p, PARAM_SET_PARAMS_S, params_p) == 0)
								{
									ParameterGroupNode *group_node_p = (ParameterGroupNode *) (param_set_p -> ps_grouped_params_p -> ll_head_p);
									json_t *group_names_p = json_array ();

									if (group_names_p)
										{
											while (success_flag && group_node_p)
												{
													json_t *param_group_json_p = GetParameterGroupAsJSON (group_node_p -> pgn_param_group_p);

													if (param_group_json_p)
														{
															if (json_array_append_new (group_names_p, param_group_json_p) == 0)
																{
																	group_node_p = (ParameterGroupNode *) (group_node_p -> pgn_node.ln_next_p);
																}
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to append \"%s\" to JSON group names", group_node_p -> pgn_param_group_p -> pg_name_s);
																	success_flag = false;
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get ParameterGroup \"%s\" as JSON", group_node_p -> pgn_param_group_p -> pg_name_s);
															success_flag = false;
														}

												}		/* while (success_flag && group_node_p) */

											if (success_flag)
												{
													int res = json_object_set_new (param_set_json_p, PARAM_SET_GROUPS_S, group_names_p);

													if (res != 0)
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set \"%s\" for JSON group names", PARAM_SET_GROUPS_S);
															success_flag = false;
														}
												}

											if (!success_flag)
												{
													json_decref (group_names_p);
												}

										}		/* if (group_names_p) */
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate JSON array for group names");
										}

								}		/* if (json_object_set_new (param_set_json_p, PARAM_SET_PARAMS_S, params_p) == 0) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_object_set_new (param_set_json_p, PARAM_SET_PARAMS_S, params_p) failed");
									success_flag = false;
								}

						}		/* if (success_flag) */


					if (!success_flag)
						{
							WipeJSON (param_set_json_p);
							param_set_json_p = NULL;
						}

				}		/* if (param_set_json_p) */

		}		/* if (param_set_json_p) */

	return param_set_json_p;

}




//uint32 GetCurrentParameterValues (const ParameterSet * const params_p, TagItem *tags_p)
//{
//	uint32 matched_count = 0;
//	ParameterNode *node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);
//
//	while (node_p)
//		{
//			Parameter *param_p = node_p -> pn_parameter_p;
//			TagItem *tag_p = FindMatchingTag (tags_p, param_p -> pa_tag);
//
//			if (tag_p)
//				{
//					tag_p -> ti_value = param_p -> pa_current_value;
//					++ matched_count;
//				}
//
//			node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
//		}		/* while (node_p) */
//
//	return matched_count;
//}


ParameterNode *GetParameterNodeFromParameterSetByName (const ParameterSet * const params_p, const char * const name_s)
{
	ParameterNode *node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);

	while (node_p)
		{
			Parameter *param_p = node_p -> pn_parameter_p;

			if (strcmp (param_p -> pa_name_s, name_s) == 0)
				{
					return node_p;
				}
			else
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}		/* while (node_p) */

	return NULL;

}

Parameter *GetParameterFromParameterSetByName (const ParameterSet * const params_p, const char * const name_s)
{
	ParameterNode *node_p = GetParameterNodeFromParameterSetByName (params_p, name_s);

	if (node_p)
		{
			return node_p -> pn_parameter_p;
		}

	return NULL;
}


bool GetParameterValueFromParameterSet (const ParameterSet * const params_p, const char * const name_s, SharedType *value_p, const bool current_value_flag)
{
	bool success_flag = false;
	Parameter *param_p = GetParameterFromParameterSetByName (params_p, name_s);

	if (param_p)
		{
			*value_p = current_value_flag ? param_p -> pa_current_value : param_p -> pa_default;
			success_flag = true;
		}
		
	return success_flag;
	
}


ParameterGroup *AddParameterGroupToParameterSet (ParameterSet *param_set_p, const char *group_name_s, const char *group_key_s, ServiceData *service_data_p, ParameterSet *params_p)
{
	ParameterGroup *group_p = NULL;
	ParameterGroupNode *param_group_node_p = AllocateParameterGroupNode (group_name_s, group_key_s, service_data_p);

	if (param_group_node_p)
		{
			group_p = param_group_node_p -> pgn_param_group_p;
		}

	return group_p;
}


ParameterGroup *AddParameterGroupToParameterSetByName (ParameterSet *param_set_p, const char *group_name_s, const char *group_key_s, const char ** const param_names_ss, const uint32 num_params, ServiceData *service_data_p)
{
	ParameterGroup *group_p = NULL;
	Parameter **params_pp = AllocMemoryArray (num_params, sizeof (Parameter *));

	/* Get the parameters for the given names */
	if (params_pp)
		{
			const char **param_name_ss = param_names_ss;
			Parameter ** param_pp = params_pp;
			uint32 i = num_params;

			bool success_flag = true;

			while (success_flag && (i > 0))
				{
					ParameterNode *param_node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
					const char * const param_name_s = *param_name_ss;

					success_flag = false;

					while (param_node_p)
						{
							Parameter *param_p = param_node_p -> pn_parameter_p;

							if (strcmp (param_name_s, param_p -> pa_name_s) == 0)
								{
									*param_pp = param_p;
									success_flag = true;
									param_node_p = NULL;
								}
							else
								{
									param_node_p = (ParameterNode *) (param_node_p -> pn_node.ln_next_p);
								}

						}		/* while (param_node_p) */

					if (success_flag)
						{
							-- i;
							++ param_name_ss;
							++ param_pp;
						}		/* if (success_flag) */
				}

			if (success_flag)
				{
					group_p = AddParameterGroupToParameterSet (param_set_p, group_name_s, group_key_s, params_pp, num_params, service_data_p);
				}

			if (!group_p)
				{
					FreeMemory (params_pp);
				}

		}		/* if (params_pp) */


	return group_p;
}



ParameterSet *CreateParameterSetFromJSON (const json_t * const op_p, const bool concise_flag)
{
	ParameterSet *params_p =  NULL;

	if (op_p)
		{
			const char *name_s = NULL;
			const char *description_s = NULL;

			#if PARAMETER_SET_DEBUG >= STM_LEVEL_FINER
			PrintJSONToLog (op_p, "CreateParameterSetFromJSON op:\n", PARAMETER_SET_DEBUG, __FILE__, __LINE__);
			#endif
			
			/* Get the name */
			json_t *json_p = json_object_get (op_p, PARAM_SET_NAME_S);
			if (json_p && json_is_string (json_p))
				{
					name_s = json_string_value (json_p);
				}

			/* Get the description */
			json_p = json_object_get (op_p, PARAM_SET_DESCRIPTION_S);
			if (json_p && json_is_string (json_p))
				{
					description_s = json_string_value (json_p);
				}

			params_p = AllocateParameterSet (name_s, description_s);
			
			if (params_p)
				{
					bool success_flag = true;

					/* Get the parameters array */
					json_t *param_set_json_p = json_object_get (op_p, PARAM_SET_KEY_S);

					if (param_set_json_p)
						{
							json_t *params_json_p = json_object_get (param_set_json_p, PARAM_SET_PARAMS_S);

							if (params_json_p && json_is_array (params_json_p))
								{
									size_t num_params = json_array_size (params_json_p);
									size_t i = 0;
									
									/* Loop through the params */
									while ((i < num_params) && success_flag)
										{
											json_t *param_json_p = json_array_get (params_json_p, i);
											Parameter *param_p = NULL;

											#if PARAMETER_SET_DEBUG >= STM_LEVEL_FINER
											PrintJSONToLog (param_json_p, "param_json_p: ", PARAMETER_SET_DEBUG, __FILE__, __LINE__);
											#endif

											param_p = CreateParameterFromJSON (param_json_p, concise_flag);
											
											if (param_p)
												{
													success_flag = AddParameterToParameterSet (params_p, param_p);
												}
											else
												{
													char *dump_s = json_dumps (param_json_p, JSON_INDENT (2));

													if (dump_s)
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "failed to create param from:\n%s\n", dump_s);
															free (dump_s);
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "failed to create param\n");
														}

													success_flag = false;
												}
												
											if (success_flag)
												{
													++ i;
												}
										}		/* while ((i < num_params) && success_flag) */


									if (success_flag)
										{
											/* Get the groups */
											json_t *groups_json_p = json_object_get (param_set_json_p, PARAM_SET_GROUPS_S);

											if (groups_json_p && json_is_array (groups_json_p))
												{
													/* assign the params to their groups and vice versa */
													size_t num_groups = json_array_size (groups_json_p);

													for (i = 0; i < num_groups; ++ i)
														{
															size_t num_group_params = 0;
															size_t j = 0;
															json_t *group_json_p = json_array_get (groups_json_p, i);

															const char *group_name_s = GetJSONString (group_json_p, PARAM_GROUP_S);

															/* Get the number of Parameters needed */
															for (j = 0; j < num_params; ++ j)
																{
																	json_t *param_json_p = json_array_get (params_json_p, j);
																	const char *param_group_name_s = GetJSONString (param_json_p, PARAM_GROUP_S);

																	if (param_group_name_s && (strcmp (param_group_name_s, group_name_s) == 0))
																		{
																			++ num_group_params;
																		}
																}		/* for (j = 0; j < num_params; ++ j) */

															if (num_group_params > 0)
																{
																	Parameter **params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));

																	if (params_pp)
																		{
																			Parameter **param_pp = params_pp;
																			ParameterNode *param_node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);
																			const char *group_key_s = NULL;
																			ParameterGroup *group_p = NULL;

																			/* Get the number of Parameters needed */
																			for (j = 0; j < num_params; ++ j)
																				{
																					json_t *param_json_p = json_array_get (params_json_p, j);
																					const char *param_group_name_s = GetJSONString (param_json_p, PARAM_GROUP_S);

																					if ((param_group_name_s) && (strcmp (param_group_name_s, group_name_s) == 0))
																						{
																							*param_pp = param_node_p -> pn_parameter_p;
																							++ param_pp;
																						}

																					param_node_p = (ParameterNode *) (param_node_p -> pn_node.ln_next_p);

																				}		/* for (j = 0; j < num_params; ++ j) */

																			if ((group_p = AddParameterGroupToParameterSet (params_p, group_name_s, group_key_s, params_pp, num_group_params, NULL)) != NULL)
																				{
																					bool visible_flag = true;

																					if (GetJSONBoolean (group_json_p, PARAM_GROUP_VISIBLE_S, &visible_flag))
																						{
																							group_p -> pg_visible_flag = visible_flag;
																						}
																				}
																			else
																				{
																					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create parameter group \"%s\"", group_name_s);
																					FreeMemory (params_pp);
																				}

																		}		/* if (params_pp) */

																}		/* if (num_group_params > 0) */

														}		/* for (i = 0; i < num_groups; ++ i) */

												}		/* if (groups_json_p && json_is_array (groups_json_p)) */

										}

								}		/* if (json_p && json_is_array (json_p)) */

							if (!success_flag)
								{
									FreeParameterSet (params_p);
									params_p = NULL;
								}

						}		/* if (param_set_json_p) */

				}		/* if (params_p) */
			
		}		/* if (root_p) */
	
	return params_p;
}


ParameterSetNode *AllocateParameterSetNode (ParameterSet *params_p)
{
	ParameterSetNode *node_p = AllocMemory (sizeof (ParameterSetNode));
	
	if (node_p)
		{
			node_p -> psn_node.ln_prev_p = NULL;
			node_p -> psn_node.ln_next_p = NULL;
			node_p -> psn_param_set_p = params_p;
		}
	
	return node_p;
}


void FreeParameterSetNode (ListItem *node_p)
{
	ParameterSetNode *param_set_node_p = (ParameterSetNode *) node_p;

	if (param_set_node_p -> psn_param_set_p)
		{
			FreeParameterSet (param_set_node_p -> psn_param_set_p);
		}
	
	FreeMemory (param_set_node_p);
}





Parameter *DetachParameterByName (ParameterSet *params_p, const char * const name_s)
{
	Parameter *param_p = NULL;
	ParameterNode *node_p = GetParameterNodeFromParameterSetByName (params_p, name_s);

	if (node_p)
		{
			param_p = node_p -> pn_parameter_p;

			LinkedListRemove (params_p -> ps_params_p, (ListItem * const) node_p);

			node_p -> pn_parameter_p = NULL;
			FreeParameterNode ((ListItem * const) node_p);
		}

	return param_p;
}


ParameterGroup *GetParameterGroupFromParameterSetByGroupName (const ParameterSet * const params_p, const char * const name_s)
{
	if (params_p -> ps_grouped_params_p)
		{
			ParameterGroupNode *group_node_p = (ParameterGroupNode *) (params_p -> ps_grouped_params_p -> ll_head_p);

			while (group_node_p)
				{
					if (strcmp (group_node_p -> pgn_param_group_p -> pg_name_s, name_s) == 0)
						{
							return group_node_p -> pgn_param_group_p;
						}

					group_node_p = (ParameterGroupNode *) (group_node_p -> pgn_node.ln_next_p);
				}
		}

	return NULL;
}



/****************************************/
/********** STATIC FUNCTIONS ************/
/****************************************/

