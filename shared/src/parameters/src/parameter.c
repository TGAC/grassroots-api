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
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "memory_allocations.h"
#include "parameter.h"
#include "parameter_group.h"
#include "math_utils.h"
#include "string_utils.h"
#include "string_hash_table.h"
#include "parameter_set.h"
#include "service.h"
#include "schema_version.h"
#include "json_tools.h"
#include "json_util.h"


#ifdef _DEBUG
	#define PARAMETER_DEBUG	(STM_LEVEL_FINE)
#else
	#define PARAMETER_DEBUG	(STM_LEVEL_NONE)
#endif


static const char *S_PARAM_TYPE_NAMES_SS [PT_NUM_TYPES] =
{
	"xsd:boolean",
	"params:signed_integer",
	"params:unsigned_integer",
	"params:negative_integer"
	"xsd:double",
	"params:unsigned_number",
	"xsd:string",
	"params:output_filename",
	"params:input_filename",
	"params:directory",
	"params:character",
	"params:password",
	"params:keyword",
	"params:large_string",
	"params:json",
	"params:tabular"
};



static ParameterMultiOptionArray *AllocateEmptyParameterMultiOptionArray (const uint32 num_options);

static bool AddParameterNameToJSON (const char *name_s, json_t *root_p, const SchemaVersion * const sv_p);

static bool AddParameterDisplayNameToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);

static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);


static bool AddParameterTypeToJSON (const ParameterType param_type, json_t *root_p, const SchemaVersion * const sv_p, const bool full_definition_flag);

static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);

static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p);

static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p);

static bool AddParameterGroupToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p);

static bool AddParameterStoreToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);

static bool AddParameterLevelToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);


static bool AddRemoteParameterDetailsToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p);


static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p);

static bool AddValueToJSON (json_t *root_p, const ParameterType pt, const SharedType *val_p, const char *key_s);


static bool AddCurrentValueToJSON (const ParameterType param_type, const SharedType * const value_p, json_t *root_p, const SchemaVersion * const sv_p);


static bool GetParameterBoundsFromJSON (const json_t * const json_p, ParameterBounds **bounds_pp, const ParameterType pt);


static bool GetParameterLevelFromJSON (const json_t * const json_p, ParameterLevel *level_p);

static bool InitParameterStoreFromJSON (const json_t *root_p, HashTable *store_p);


static LinkedList *CreateSharedTypesList (const SharedType *values_p);


static LinkedList *CopySharedTypesList (const LinkedList *source_p);


static bool SetRemoteParameterDetailsFromJSON (Parameter *param_p, const json_t * json_p);


static bool SetParameterValueFromBoolean (Parameter * const param_p, const bool b, const bool current_flag);


static bool SetParameterValueFromChar (Parameter * const param_p, const char c, const bool current_flag);


static bool SetParameterValueFromSignedInt (Parameter * const param_p, const int32 i, const bool current_flag);


static bool SetParameterValueFromUnsignedInt (Parameter * const param_p, const uint32 i, const bool current_flag);


static bool SetParameterValueFromReal (Parameter * const param_p, const double64 d, const bool current_flag);


static bool SetParameterValueFromStringValue (Parameter * const param_p, const char * const src_s, const bool current_flag);


static bool SetParameterValueFromResource (Parameter * const param_p, const Resource * const src_p, const bool current_flag);


static bool SetParameterValueFromJSON (Parameter * const param_p, const json_t * const src_p, const bool current_flag);


static bool SetSharedTypeBooleanValue (SharedType * value_p, const bool b);


static bool SetSharedTypeCharValue (SharedType * value_p, const char c);


static bool SetSharedTypeUnsignedIntValue (SharedType * value_p, const uint32 i, const ParameterBounds * const bounds_p);


static bool SetSharedTypeSignedIntValue (SharedType * value_p, const int32 i, const ParameterBounds * const bounds_p);


static bool SetSharedTypeRealValue (SharedType * value_p, const double64 d, const ParameterBounds * const bounds_p);


static bool SetSharedTypeStringValue (SharedType *value_p, const char * const src_s);


static bool SetSharedTypeResourceValue (SharedType *value_p, const Resource * const src_p);


static bool SetSharedTypeJSONValue (SharedType *value_p, const json_t * const src_p);


static const json_t *GetParameterFromConfig (const json_t *service_config_p, const char * const param_name_s);

static bool GetParameterStringFromConfig (const json_t *service_config_p, const char * const param_name_s, const char * const key_s, char **value_ss);



static bool AddCompoundGrassrootsType (json_t *value_p, const ParameterType pt);


static bool GetParameterTypeFromCompoundObject (const json_t *root_p, ParameterType *pt_p);

static bool AddSeparateGrassrootsTypes (json_t *value_p, const ParameterType pt);

static bool GetParameterTypeFromSeparateObjects (const json_t * const json_p, ParameterType *param_type_p);


/******************************************************/


Parameter *AllocateParameter (const ServiceData *service_data_p, ParameterType type, bool multi_valued_flag, const char * const name_s, const char * const display_name_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	char *new_name_s = CopyToNewString (name_s, 0, true);

	if (new_name_s)
		{
			bool success_flag = true;
			char *new_description_s = NULL;
			
			if (description_s)
				{
					new_description_s = CopyToNewString (description_s, 0, true);
					success_flag = (new_description_s != NULL);
				}

			if (success_flag)
				{
					char *new_display_name_s = NULL;
					
					if (display_name_s)
						{
							new_display_name_s = CopyToNewString (display_name_s, 0, true);
							success_flag = (new_display_name_s != NULL);
						}
					
					if (success_flag)
						{
							HashTable *store_p = GetHashTableOfStrings (8, 75);

							if (store_p)
								{
									LinkedList *remote_params_p = AllocateLinkedList (FreeRemoteParameterDetailsNode);

									if (remote_params_p)
										{
											Parameter *param_p = (Parameter *) AllocMemory (sizeof (Parameter));

											if (param_p)
												{
													param_p -> pa_type = type;
													param_p -> pa_multi_valued_flag = multi_valued_flag;
													param_p -> pa_name_s = new_name_s;
													param_p -> pa_display_name_s = new_display_name_s;
													param_p -> pa_description_s = new_description_s;
													param_p -> pa_options_p = options_p;
													param_p -> pa_check_value_fn = check_value_fn;
													param_p -> pa_bounds_p = bounds_p;
													param_p -> pa_level = level;
													param_p -> pa_store_p = store_p;
													param_p -> pa_group_p = NULL;

													param_p -> pa_remote_parameter_details_p = remote_params_p;

													memset (& (param_p -> pa_current_value), 0, sizeof (SharedType));
													memset (& (param_p -> pa_default), 0, sizeof (SharedType));

													if (multi_valued_flag)
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Multi-valued parameters not yet implemented");
														}
													else
														{
															/*
															 * Check for any values that have been overrode in
															 * the service configuration.
															 */
															if (service_data_p)
																{
																	GetParameterDefaultValueFromConfig (service_data_p, name_s, param_p -> pa_type, &default_value);

																	GetParameterLevelFromConfig (service_data_p, name_s, & (param_p -> pa_level));

																	GetParameterDescriptionFromConfig (service_data_p, name_s, & (param_p -> pa_description_s));
																}

															if (SetParameterValueFromSharedType (param_p, current_value_p ? current_value_p : &default_value, true))
																{
																	if (SetParameterValueFromSharedType (param_p, &default_value, false))
																		{
																			return param_p;
																		}		/* if (SetParameterValueFromSharedType (param_p, &default_value, false)) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set param current value for %s", param_p -> pa_name_s);
																		}
																}		/* if (SetParameterValueFromSharedType (param_p, current_value_p ? current_value_p : &default_value, true)) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set param default value for %s", param_p -> pa_name_s);
																}
														}

													FreeMemory (param_p);
												}		/* if (param_p) */

											FreeLinkedList (remote_params_p);
										}		/* if (remote_params_p) */

									FreeHashTable (store_p);
								}		/* if (store_p) */

						}
						
					if (new_display_name_s)
						{
							FreeCopiedString (new_display_name_s);
						}		/* if (new_description_s) */	
						
						
					if (new_description_s)
						{
							FreeCopiedString (new_description_s);
						}		/* if (new_description_s) */	
						
				}		/* if (success_flag) */

			FreeCopiedString (new_name_s);
		}		/* if (new_name_s) */

	return NULL;
}


void FreeParameter (Parameter *param_p)
{
	#if PARAMETER_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Freeing parameter %s", param_p -> pa_name_s);
	#endif


	if (param_p -> pa_description_s)
		{
			FreeCopiedString (param_p -> pa_description_s);
		}

	if (param_p -> pa_name_s)
		{
			FreeCopiedString (param_p -> pa_name_s);
		}

	if (param_p -> pa_display_name_s)
		{
			FreeCopiedString (param_p -> pa_display_name_s);
		}

	if (param_p -> pa_options_p)
		{
			FreeParameterMultiOptionArray (param_p -> pa_options_p);
		}

	if (param_p -> pa_bounds_p)
		{
			FreeParameterBounds (param_p -> pa_bounds_p, param_p -> pa_type);
		}

	ClearSharedType (& (param_p -> pa_current_value), param_p -> pa_type);
	ClearSharedType (& (param_p -> pa_default), param_p -> pa_type);

	FreeHashTable (param_p -> pa_store_p);

	FreeLinkedList (param_p -> pa_remote_parameter_details_p);

	FreeMemory (param_p);
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

	if (param_node_p -> pn_parameter_p)
		{
			FreeParameter (param_node_p -> pn_parameter_p);
		}

	FreeMemory (param_node_p);
}


ParameterBounds *AllocateParameterBounds (void)
{
	ParameterBounds *bounds_p = (ParameterBounds *) AllocMemory (sizeof (ParameterBounds));

	if (bounds_p)
		{
			memset (bounds_p, 0, sizeof (ParameterBounds));
			return bounds_p;
		}		/* if (bounds_p) */

	return NULL;
}


ParameterBounds *CopyParameterBounds (const ParameterBounds * const src_p, const ParameterType pt)
{
	ParameterBounds *bounds_p = AllocateParameterBounds ();

	if (bounds_p)
		{
			switch (pt)
				{
					case PT_DIRECTORY:
					case PT_FILE_TO_READ:
					case PT_FILE_TO_WRITE:
					
						break;
					
					case PT_STRING:
					case PT_LARGE_STRING:
					case PT_PASSWORD:
					case PT_KEYWORD:
						{
							bounds_p -> pb_lower.st_string_value_s = CopyToNewString (src_p -> pb_lower.st_string_value_s, 0, false);
							bounds_p -> pb_upper.st_string_value_s = CopyToNewString (src_p -> pb_upper.st_string_value_s, 0, false);

							if (! ((bounds_p -> pb_lower.st_string_value_s) && (bounds_p -> pb_upper.st_string_value_s)))
								{
									FreeParameterBounds (bounds_p, pt);
									bounds_p = NULL;
								}
						}
						break;

					default:
						memcpy (& (bounds_p -> pb_lower), & (src_p -> pb_lower), sizeof (SharedType));
						memcpy (& (bounds_p -> pb_upper), & (src_p -> pb_upper), sizeof (SharedType));
						break;
				}

			return bounds_p;
		}

	return NULL;
}


void FreeParameterBounds (ParameterBounds *bounds_p, const ParameterType pt)
{
	if ((pt == PT_STRING) || (pt == PT_LARGE_STRING) || (pt == PT_PASSWORD) || (pt == PT_FILE_TO_READ) || (pt == PT_FILE_TO_WRITE) || (pt == PT_DIRECTORY))
		{
			if (bounds_p -> pb_lower.st_string_value_s)
				{
					FreeCopiedString (bounds_p -> pb_lower.st_string_value_s);
				}

			if (bounds_p -> pb_lower.st_string_value_s)
				{
					FreeCopiedString (bounds_p -> pb_lower.st_string_value_s);
				}
		}

	FreeMemory (bounds_p);
}


static ParameterMultiOptionArray *AllocateEmptyParameterMultiOptionArray (const uint32 num_options)
{
	ParameterMultiOption *options_p = (ParameterMultiOption *) AllocMemoryArray (num_options, sizeof (ParameterMultiOption));

	if (options_p)
		{
			ParameterMultiOptionArray *array_p = (ParameterMultiOptionArray *) AllocMemory (sizeof (ParameterMultiOptionArray));

			if (array_p)
				{
					array_p -> pmoa_num_options = num_options;
					array_p -> pmoa_options_p = options_p;

					return array_p;
				}		/* if (array_p) */

			FreeMemory (options_p);
		}		/* if (options_p) */

	return NULL;
}


ParameterMultiOptionArray *AllocateParameterMultiOptionArray (const uint32 num_options, const char ** const descriptions_pp, SharedType *values_p, ParameterType pt, bool copy_values_flag)
{
	ParameterMultiOptionArray *array_p = AllocateEmptyParameterMultiOptionArray (num_options);

	if (array_p)
		{
			uint32 i;
			ParameterMultiOption *option_p = array_p -> pmoa_options_p;
			const char **description_pp = descriptions_pp;
			SharedType *value_p = values_p;

			array_p -> pmoa_values_type = pt;

			for (i = 0; i < num_options; ++ i, ++ option_p, ++ value_p)
				{
					const char *description_s = NULL;

					if (description_pp)
						{
							description_s = *description_pp;
							++ description_pp;
						}

					if (!SetParameterMultiOption (array_p, i, description_s, *value_p, copy_values_flag))
						{
							FreeParameterMultiOptionArray (array_p);
							array_p = NULL;
							i = num_options;
						}		/* if (!SetParameterMultiOption (array_p, i, *description_pp, *value_pp)) */

				}		/* for ( ; i > 0; -- i, ++ option_p) */

			return array_p;
		}		/* if (array_p) */

	return NULL;
}


void FreeParameterMultiOptionArray (ParameterMultiOptionArray *options_p)
{
	uint32 i = options_p -> pmoa_num_options;
	ParameterMultiOption *option_p = options_p -> pmoa_options_p;

	for ( ; i > 0; -- i, ++ option_p)
		{
			if (option_p -> pmo_description_s)
				{
					FreeCopiedString (option_p -> pmo_description_s);
				}

			ClearSharedType (& (option_p -> pmo_value), options_p -> pmoa_values_type);
		}		/* for ( ; i > 0; -- i, ++ option_p) */

	FreeMemory (options_p -> pmoa_options_p);
	FreeMemory (options_p);
}


bool CopyRemoteParameterDetails (const Parameter * const src_p, Parameter *dest_p)
{
	bool success_flag = true;

	if (src_p -> pa_remote_parameter_details_p -> ll_size)
		{
			const RemoteParameterDetailsNode *src_node_p = (const RemoteParameterDetailsNode *) (src_p -> pa_remote_parameter_details_p -> ll_head_p);

			while (src_node_p && success_flag)
				{
					const RemoteParameterDetails *src_details_p = src_node_p -> rpdn_details_p;

					if (AddRemoteDetailsToParameter (dest_p, src_details_p -> rpd_server_uri_s, src_details_p -> rpd_tag))
						{
							src_node_p = (const RemoteParameterDetailsNode *) (src_node_p -> rpdn_node.ln_next_p);
						}
					else
						{
							success_flag = false;
						}
				}
		}
	else
		{
			ClearLinkedList (dest_p -> pa_remote_parameter_details_p);
		}

	return success_flag;
}


bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 i, const char * const description_s, SharedType value, bool copy_value_flag)
{
	ParameterMultiOption *option_p = (options_p -> pmoa_options_p) + i;
	bool success_flag = true;

	if (description_s)
		{
			char *new_description_s = CopyToNewString (description_s, 0, true);

			if (new_description_s)
				{
					if (option_p -> pmo_description_s)
						{
							FreeCopiedString (option_p -> pmo_description_s);
						}

					option_p -> pmo_description_s = new_description_s;
				}
			else
				{
					success_flag = false;
				}
		}

	if (success_flag)
		{
			if (copy_value_flag)
				{
					if (!CopySharedType (value, & (option_p -> pmo_value), options_p -> pmoa_values_type))
						{
							success_flag = false;
						}
				}
			else
				{
					option_p -> pmo_value = value;
				}
		}

	return success_flag;
}



const char *CheckForSignedReal (const Parameter * const UNUSED_PARAM (parameter_p), const void *value_p)
{
	const char *error_s = NULL;

	if (value_p)
		{
			if (CompareDoubles (* ((double64 *) value_p), 0.0) < 0)
				{
					error_s = "The value must be non-negative";
				}
		}
	else
		{
			error_s = "The value is required";
		}

	return error_s;
}


const char *CheckForNotNull (const Parameter * const UNUSED_PARAM (parameter_p), const void *value_p)
{
	if (!value_p)
		{
			return "The value is required";
		}
	else
		{
			return NULL;
		}
}



bool CompareParameterLevels (const ParameterLevel param_level, const ParameterLevel threshold)
{
	return ((param_level & threshold) == threshold);
}


bool AddParameterKeyValuePair (Parameter * const parameter_p, const char *key_s, const char *value_s)
{
	return PutInHashTable (parameter_p -> pa_store_p, key_s, value_s);
}


void RemoveParameterKeyValuePair (Parameter * const parameter_p, const char *key_s)
{
	RemoveFromHashTable (parameter_p -> pa_store_p, key_s);
}


const char *GetParameterKeyValue (const Parameter * const parameter_p, const char *key_s)
{
	return ((const char *) GetFromHashTable (parameter_p -> pa_store_p, key_s));
}




bool SetParameterValueFromSharedType (Parameter * const param_p, const SharedType * src_p, const bool current_value_flag)
{
	bool success_flag = false;

	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				success_flag = SetParameterValueFromBoolean (param_p, src_p -> st_boolean_value, current_value_flag);
				break;

			case PT_CHAR:
				success_flag = SetParameterValueFromChar (param_p, src_p -> st_char_value, current_value_flag);
				break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				success_flag = SetParameterValueFromSignedInt (param_p, src_p -> st_long_value, current_value_flag);
				break;

			case PT_UNSIGNED_INT:
				success_flag = SetParameterValueFromUnsignedInt (param_p, src_p -> st_ulong_value, current_value_flag);
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				success_flag = SetParameterValueFromReal (param_p, src_p -> st_data_value, current_value_flag);
				break;

			case PT_LARGE_STRING:
			case PT_STRING:
			case PT_TABLE:
			case PT_PASSWORD:
			case PT_KEYWORD:
				success_flag = SetParameterValueFromStringValue (param_p, src_p -> st_string_value_s, current_value_flag);
				break;

			case PT_FILE_TO_WRITE:
			case PT_FILE_TO_READ:
			case PT_DIRECTORY:
				success_flag = SetParameterValueFromResource (param_p, src_p -> st_resource_value_p, current_value_flag);
			break;

			case PT_JSON:
				success_flag = SetParameterValueFromJSON (param_p, src_p -> st_json_p, current_value_flag);
				break;

			default:
				break;
		}

	return success_flag;

}


bool SetParameterValue (Parameter * const param_p, const void *value_p, const bool current_value_flag)
{
	bool success_flag = false;

	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				{
					const bool b = * ((bool *) value_p);
					success_flag = SetParameterValueFromBoolean (param_p, b, current_value_flag);
				}
				break;

			case PT_CHAR:
				{
					const char c = * ((char *) value_p);
					success_flag = SetParameterValueFromChar (param_p, c, current_value_flag);
				}
				break;


			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				{
					const int32 i = * ((int32 *) value_p);
					success_flag = SetParameterValueFromSignedInt (param_p, i, current_value_flag);
				}
				break;

			case PT_UNSIGNED_INT:
				{
					const uint32 i = * ((uint32 *) value_p);
					success_flag = SetParameterValueFromUnsignedInt (param_p, i, current_value_flag);
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					const double d = * ((double *) value_p);
					success_flag = SetParameterValueFromReal (param_p, d, current_value_flag);
				}
				break;

			case PT_LARGE_STRING:
			case PT_STRING:
			case PT_TABLE:
			case PT_PASSWORD:
			case PT_KEYWORD:
				{
					const char * const value_s = (const char *) value_p;
					success_flag = SetParameterValueFromStringValue (param_p, value_s, current_value_flag);
				}
				break;

			case PT_FILE_TO_WRITE:
			case PT_FILE_TO_READ:
			case PT_DIRECTORY:
				{
					const Resource * const res_p = (const Resource *) value_p;
					success_flag = SetParameterValueFromResource (param_p, res_p, current_value_flag);
				}
			break;

			case PT_JSON:
				{
					const json_t * const src_p = (const json_t *) value_p;
					success_flag = SetParameterValueFromJSON (param_p, src_p, current_value_flag);
				}
				break;

			default:
				break;
		}

	return success_flag;
}



json_t *GetRunnableParameterAsJSON (const char * const name_s, const SharedType * const value_p, const ParameterType param_type, const SchemaVersion * const sv_p, const bool full_definition_flag)
{
	json_t *root_p = json_object ();

	if (root_p)
		{
			if (AddParameterNameToJSON (name_s, root_p, sv_p))
				{
					if (AddCurrentValueToJSON (param_type, value_p, root_p, sv_p))
						{
							if (AddParameterTypeToJSON (param_type, root_p, sv_p, full_definition_flag))
								{
									return root_p;
								}
						}
				}

			json_decref (root_p);
		}

	return NULL;
}


json_t *GetParameterAsJSON (const Parameter * const param_p, const SchemaVersion * const sv_p, const bool full_definition_flag)
{
	json_t *root_p = GetRunnableParameterAsJSON (param_p -> pa_name_s, & (param_p -> pa_current_value), param_p -> pa_type, sv_p, full_definition_flag);

	if (root_p)
		{
			bool success_flag = false;

			if (AddParameterStoreToJSON (param_p, root_p, sv_p))
				{
					if (AddRemoteParameterDetailsToJSON (param_p, root_p, sv_p))
						{
							if (full_definition_flag)
								{
									if (AddParameterLevelToJSON (param_p, root_p, sv_p))
										{
											if (AddParameterDescriptionToJSON (param_p, root_p, sv_p))
												{
													if (AddParameterDisplayNameToJSON (param_p, root_p, sv_p))
														{
															if (AddDefaultValueToJSON (param_p, root_p, sv_p))
																{
																	if (AddParameterOptionsToJSON (param_p, root_p, sv_p))
																		{
																			if (AddParameterBoundsToJSON (param_p, root_p, sv_p))
																				{
																					if (AddParameterGroupToJSON (param_p, root_p, sv_p))
																						{
																							success_flag = true;
																						}		/* if (AddParameterGroupToJSON (param_p, root_p)) */
																					else
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterGroupToJSON for \"%s\"", param_p -> pa_name_s);
																						}

																				}		/* if (AddParameterBoundsToJSON (param_p, root_p)) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterBoundsToJSON for \"%s\"", param_p -> pa_name_s);
																				}

																		}		/* if (AddParameterOptionsToJSON (param_p, root_p)) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterOptionsToJSON for \"%s\"", param_p -> pa_name_s);
																		}

																}		/* if (AddDefaultValueToJSON (param_p, root_p)) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddDefaultValueToJSON for \"%s\"", param_p -> pa_name_s);
																}

														}		/* if (AddParameterDisplayNameToJSON (param_p, root_p)) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterDisplayNameToJSON for \"%s\"", param_p -> pa_name_s);
														}

												}		/* if (AddParameterDescriptionToJSON (param_p, root_p)) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterDescriptionToJSON for \"%s\"", param_p -> pa_name_s);
												}
										}		/* if (AddParameterLevelToJSON (param_p, root_p)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterLevelToJSON for \"%s\"", param_p -> pa_name_s);
										}
								}		/* if (full_definition_flag) */
							else
								{
									success_flag = true;
								}

						}		/* if (AddParameterRemoteDetailsToJSON (param_p, root_p)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterRemoteDetailsToJSON for \"%s\"", param_p -> pa_name_s);
						}

				}		/* if (AddParameterStoreToJSON (param_p, root_p)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed: AddParameterStoreToJSON for \"%s\"", param_p -> pa_name_s);
				}

			if (!success_flag)
				{
					json_decref (root_p);
					root_p = NULL;
				}

		}		/* if (root_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate JSON for parameter");
		}

	return root_p;
}


static bool AddParameterNameToJSON (const char *name_s, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = (json_object_set_new (root_p, PARAM_NAME_S, json_string (name_s)) == 0);

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddParameterNameToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}



static bool AddParameterDisplayNameToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = true;
	
	if (param_p -> pa_display_name_s)
		{
			success_flag = (json_object_set_new (root_p, PARAM_DISPLAY_NAME_S, json_string (param_p -> pa_display_name_s)) == 0);
		}
		
	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddParameterDisplayNameToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}


static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = true;

	if (param_p -> pa_description_s)
		{
			success_flag = (json_object_set_new (root_p, PARAM_DESCRIPTION_S, json_string (param_p -> pa_description_s)) == 0);
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddParameterDescriptionToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}


static bool AddRemoteParameterDetailsToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = true;

	if (param_p -> pa_remote_parameter_details_p -> ll_size > 0)
		{
			json_t *remote_details_list_json_p = json_array ();

			if (remote_details_list_json_p)
				{
					RemoteParameterDetailsNode *node_p = (RemoteParameterDetailsNode *) (param_p -> pa_remote_parameter_details_p -> ll_head_p);

					while (node_p && success_flag)
						{
							json_t *remote_details_json_p = GetRemoteParameterDetailsAsJSON (node_p -> rpdn_details_p);

							if (remote_details_json_p)
								{
									if (json_array_append_new (remote_details_list_json_p, remote_details_json_p) == 0)
										{
											node_p = (RemoteParameterDetailsNode *) (node_p -> rpdn_node.ln_next_p);
										}
									else
										{
											json_decref (remote_details_json_p);
											success_flag = false;
										}

								}		/* if (remote_details_json_p) */
							else
								{
									success_flag = false;
								}

						}		/* while (node_p && success_flag) */

					if (!success_flag)
						{
							json_decref (remote_details_list_json_p);
						}

				}		/* if (remote_details_list_json_p) */
			else
				{
					success_flag = false;
				}

		}		/* if (param_p -> pa_remote_parameter_details_p -> ll_size > 0) */


	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddRemoteParameterDetailsToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}


static bool AddParameterLevelToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = false;

	if ((param_p -> pa_level & PL_ALL) == PL_ALL)
		{
			if (json_object_set_new (root_p, PARAM_LEVEL_S, json_string (PARAM_LEVEL_TEXT_ALL_S)) == 0)
				{
					success_flag = true;
				}
		}
	else
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (256);

			if (buffer_p)
				{
					int append_status = 0;

					if ((param_p -> pa_level & PL_BASIC) == PL_BASIC)
						{
							append_status = AppendStringToByteBuffer (buffer_p, PARAM_LEVEL_TEXT_BASIC_S) ? 1 : -1;
						}

					if (append_status != -1)
						{
							if ((param_p -> pa_level & PL_INTERMEDIATE) == PL_INTERMEDIATE)
								{
									if (append_status == 1)
										{
											append_status = AppendStringsToByteBuffer (buffer_p, "|", PARAM_LEVEL_TEXT_INTERMEDIATE_S, NULL) ? 1 : -1;
										}
									else
										{
											append_status = AppendStringToByteBuffer (buffer_p, PARAM_LEVEL_TEXT_INTERMEDIATE_S) ? 1 : -1;
										}
								}
						}

					if (append_status != -1)
						{
							if ((param_p -> pa_level & PL_ADVANCED) == PL_ADVANCED)
								{
									if (append_status == 1)
										{
											append_status = AppendStringsToByteBuffer (buffer_p, "|", PARAM_LEVEL_TEXT_ADVANCED_S, NULL) ? 1 : -1;
										}
									else
										{
											append_status = AppendStringToByteBuffer (buffer_p, PARAM_LEVEL_TEXT_ADVANCED_S) ? 1 : -1;
										}
								}
						}

					if (append_status == 1)
						{
							const char *data_s = GetByteBufferData (buffer_p);

							if (json_object_set_new (root_p, PARAM_LEVEL_S, json_string (data_s)) == 0)
								{
									success_flag = true;
								}
						}

					success_flag = (append_status != -1);

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddParameterLevelToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}


static bool AddParameterStoreToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const UNUSED_PARAM (sv_p))
{
	bool success_flag = true;
	uint32 i = GetHashTableSize (param_p -> pa_store_p);

	if (i > 0)
		{
			void **keys_pp = GetKeysIndexFromHashTable (param_p -> pa_store_p);

			if (keys_pp)
				{
					json_t *store_json_p = json_object ();

					if (store_json_p)
						{
							void **key_pp = keys_pp;

							while (success_flag && (i > 0))
								{
									const char *key_s = *key_pp;
									const char *value_s = GetParameterKeyValue (param_p, key_s);

									if (json_object_set_new (store_json_p, key_s, json_string (value_s)) == 0)
										{
											++ key_pp;
											-- i;
										}
									else
										{
											success_flag = false;
										}
								}

							if (success_flag)
								{
									if (json_object_set_new (root_p, PARAM_STORE_S, store_json_p) == 0)
										{
											success_flag = true;
										}
								}

							if (!success_flag)
								{
									json_object_clear (store_json_p);
									json_decref (store_json_p);
								}

						}		/* if (store_json_p) */

					FreeKeysIndex (keys_pp);
				}
		}

	return success_flag;
}


static bool AddParameterTypeToJSON (const ParameterType param_type, json_t *root_p, const SchemaVersion * const sv_p, const bool full_definition_flag)
{
	bool success_flag = false;

	if (full_definition_flag)
		{
			/* Set the parameter type */
			switch (param_type)
				{
					case PT_BOOLEAN:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("boolean")) == 0);
						break;

					case PT_CHAR:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("character")) == 0);
						break;

					case PT_SIGNED_INT:
					case PT_NEGATIVE_INT:
					case PT_UNSIGNED_INT:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("integer")) == 0);
						break;

					case PT_SIGNED_REAL:
					case PT_UNSIGNED_REAL:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("number")) == 0);
						break;

					case PT_STRING:
					case PT_TABLE:
					case PT_LARGE_STRING:
					case PT_PASSWORD:
					case PT_FILE_TO_WRITE:
					case PT_DIRECTORY:
					case PT_KEYWORD:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("string")) == 0);
						break;

					case PT_FILE_TO_READ:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("string")) == 0);
						break;

					case PT_JSON:
						success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("json")) == 0);
						break;

					default:
						break;
				}		/* switch (param_p -> pa_type) */

		}		/* if (full_definition_flag) */
	else
		{
			success_flag = true;
		}

	if (success_flag)
		{
			if ((sv_p -> sv_major == 0) && (sv_p -> sv_minor == 1))
				{
					success_flag = AddSeparateGrassrootsTypes (root_p, param_type);
				}
			else if ((sv_p -> sv_major == 0) && (sv_p -> sv_minor <= 9))
				{
					success_flag = AddCompoundGrassrootsType (root_p, param_type);
				}
			else
				{
					const char *param_type_s = * (S_PARAM_TYPE_NAMES_SS + param_type);

					success_flag = (json_object_set_new (root_p, PARAM_GRASSROOTS_TYPE_INFO_TEXT_S, json_string (param_type_s)) == 0);
				}

			if (!success_flag)
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, root_p, "Failed to add grassroots type for %d", param_type);
				}		/* if (!AddCompoundGrassrootsType (root_p, param_p -> pa_type)) */

		}		/* if (success_flag) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, root_p, "Failed to add type for %d", param_type);
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddParameterTypeToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;
}


static bool AddCompoundGrassrootsType (json_t *value_p, const ParameterType pt)
{
	bool success_flag = false;
	json_t *grassroots_p = json_object ();

	if (grassroots_p)
		{
			if (json_object_set_new (grassroots_p, PARAM_COMPOUND_VALUE_S, json_integer (pt)) == 0)
				{
					const char *type_s = GetGrassrootsTypeAsString (pt);

					if (type_s)
						{
							if (json_object_set_new (grassroots_p, PARAM_COMPOUND_TEXT_S, json_string (type_s)) == 0)
								{
									if (json_object_set_new (value_p, PARAM_GRASSROOTS_S, grassroots_p) == 0)
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add compound %s: %s to the json definition", PARAM_COMPOUND_TEXT_S, type_s);
										}

								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add compound %s: %s to the json definition", PARAM_COMPOUND_TEXT_S, type_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get human-readable type string for %d", pt);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add compound %s: %d to the json definition", PARAM_GRASSROOTS_TYPE_INFO_S, pt);
				}

			if (!success_flag)
				{
					json_decref (grassroots_p);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate json object for comppond grassroots type");
		}

	return success_flag;
}


static bool GetParameterTypeFromCompoundObject (const json_t *root_p, ParameterType *pt_p)
{
	bool success_flag = false;

	const json_t *grassroots_p = json_object_get (root_p, PARAM_GRASSROOTS_S);

	if (grassroots_p)
		{
			const json_t *value_p = json_object_get (grassroots_p, PARAM_COMPOUND_VALUE_S);

			if (value_p)
				{
					if (json_is_integer (value_p))
						{
							json_int_t subtype = json_integer_value (value_p);

							if ((subtype >= 0) && (subtype < PT_NUM_TYPES))
								{
									*pt_p = subtype;
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get grassroots type value from " JSON_INTEGER_FORMAT, subtype);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "json value for %s is %d not a int ", PARAM_COMPOUND_VALUE_S, json_typeof (value_p));
						}
				}
			else if ((value_p = json_object_get (value_p, PARAM_COMPOUND_TEXT_S)) != NULL)
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);

							if (value_s)
								{
									if (GetGrassrootsTypeFromString (value_s, pt_p))
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get grassroots type from \"%s\"", value_s);
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get string from json value for %s", PARAM_COMPOUND_TEXT_S);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "json value for %s is %d not a string ", PARAM_COMPOUND_TEXT_S, json_typeof (value_p));
						}
				}

				}		/* if (grassroots_p) */


	return success_flag;
}


static bool AddSeparateGrassrootsTypes (json_t *value_p, const ParameterType pt)
{
	bool success_flag = false;

	if (json_object_set_new (value_p, PARAM_GRASSROOTS_TYPE_INFO_S, json_integer (pt)) == 0)
		{
			const char *type_s = GetGrassrootsTypeAsString (pt);

			if (type_s)
				{
					if (json_object_set_new (value_p, PARAM_GRASSROOTS_TYPE_INFO_TEXT_S, json_string (type_s)) != 0)
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s: %s to the json defintiion", PARAM_GRASSROOTS_TYPE_INFO_TEXT_S, type_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get human-readable type string for %d", pt);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s: %d to the json defintiion", PARAM_GRASSROOTS_TYPE_INFO_S, pt);
			success_flag = false;
		}


	return success_flag;
}


static bool GetParameterTypeFromSeparateObjects (const json_t * const json_p, ParameterType *param_type_p)
{
	bool success_flag = false;
	json_t *value_p = json_object_get (json_p, PARAM_GRASSROOTS_TYPE_INFO_S);

	if (value_p)
		{
			if (json_is_integer (value_p))
				{
					json_int_t subtype = json_integer_value (value_p);

					if ((subtype >= 0) && (subtype < PT_NUM_TYPES))
						{
							*param_type_p = subtype;
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get grassroots type value from " JSON_INTEGER_FORMAT, subtype);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "json value for %s is %d not a int ", PARAM_GRASSROOTS_TYPE_INFO_S, json_typeof (value_p));
				}
		}
	else if ((value_p = json_object_get (json_p, PARAM_GRASSROOTS_TYPE_INFO_TEXT_S)) != NULL)
		{
			if (json_is_string (value_p))
				{
					const char *value_s = json_string_value (value_p);

					if (value_s)
						{
							if (GetGrassrootsTypeFromString (value_s, param_type_p))
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get grassroots type from \"%s\"", value_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get string from json value for %s", PARAM_GRASSROOTS_TYPE_INFO_TEXT_S);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "json value for %s is %d not a string ", PARAM_GRASSROOTS_TYPE_INFO_TEXT_S, json_typeof (value_p));
				}
		}

	return success_flag;
}


static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p, const SchemaVersion * const sv_p)
{
	return AddValueToJSON (root_p, param_p -> pa_type, & (param_p -> pa_default), PARAM_DEFAULT_VALUE_S);
}


static bool AddCurrentValueToJSON (const ParameterType param_type, const SharedType * const value_p, json_t *root_p, const SchemaVersion * const sv_p)
{
	return AddValueToJSON (root_p, param_type, value_p, PARAM_CURRENT_VALUE_S);
}


static bool AddValueToJSON (json_t *root_p, const ParameterType pt, const SharedType *val_p, const char *key_s)
{
	bool success_flag = false;
	bool null_flag = false;
	json_t *value_p = NULL;

	switch (pt)
		{
			case PT_BOOLEAN:
				value_p = (val_p -> st_boolean_value == true) ? json_true () : json_false ();
				break;

			case PT_CHAR:
				{
					char buffer_s [2];

					*buffer_s = val_p -> st_char_value;
					* (buffer_s + 1) = '\0';

					value_p = json_string (buffer_s);
				}
				break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				value_p = json_integer (val_p -> st_long_value);
				break;

			case PT_UNSIGNED_INT:
				value_p = json_integer (val_p -> st_ulong_value);
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				value_p = json_real (val_p -> st_data_value);
				break;

			case PT_STRING:
			case PT_TABLE:
			case PT_LARGE_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				if (val_p -> st_string_value_s)
					{
						value_p = json_string (val_p -> st_string_value_s);
					}
				else
					{
						value_p = json_string ("");
					}
				break;

			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				{
					value_p = json_object ();
					
					if (value_p)
						{
							char *protocol_s = NULL;
							char *value_s = NULL;

							success_flag = false;
							
							if (val_p -> st_resource_value_p)
								{ 		
									protocol_s = val_p -> st_resource_value_p -> re_protocol_s;
									value_s = val_p -> st_resource_value_p -> re_value_s;
								}			
							else
								{
									protocol_s = "";
									value_s = "";
								}

							if (json_object_set_new (value_p, RESOURCE_PROTOCOL_S, json_string (protocol_s)) == 0)
								{
									success_flag = (json_object_set_new (value_p, RESOURCE_VALUE_S, json_string (value_s)) == 0);
								}

							if (!success_flag)
								{
									json_object_clear (value_p);
									json_decref (value_p);
									value_p = NULL;
								}							
															
						}		/* if (val_p -> st_resource_value_p) */
				}
				break;

			/*
			 * A json value can legitimately be NULL
			 */
			case PT_JSON:
				if (val_p -> st_json_p)
					{
						value_p = json_deep_copy (val_p -> st_json_p);
					}
				else
					{
						null_flag = true;
					}
				break;

			default:
				break;
		}		/* switch (pt) */

	if (value_p)
		{
			success_flag = (json_object_set_new (root_p, key_s, value_p) == 0);
		}		/* if (default_value_p) */
	else
		{
			success_flag = null_flag;
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (root_p, "AddValueToJSON - root_p :: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return success_flag;

}



const char *GetGrassrootsTypeAsString (const ParameterType param_type)
{
	const char *res_s = NULL;

	if ((param_type >= 0) && (param_type < PT_NUM_TYPES))
		{
			return * (S_PARAM_TYPE_NAMES_SS + param_type);
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "param type %d is out of range", param_type);
		}

	return res_s;
}


bool GetGrassrootsTypeFromString (const char *param_type_s, ParameterType *param_type_p)
{
	const char **type_ss = S_PARAM_TYPE_NAMES_SS;
	ParameterType i;

	for (i = 0; i < PT_NUM_TYPES; ++ i, ++ type_ss)
		{
			if (strcmp (*type_ss, param_type_s) == 0)
				{
					*param_type_p = i;
					return true;
				}
		}

	return false;
}




static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p)
{
	bool success_flag = false;

	/* Get the parameter's value */
	json_t *json_value_p = json_object_get (root_p, key_s);

	if (json_value_p)
		{			
			#if PARAMETER_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, json_value_p, key_s);
			#endif

			switch (param_type)
				{
					case PT_BOOLEAN:
						if (json_is_boolean (json_value_p))
							{
								value_p -> st_boolean_value = (json_is_true (json_value_p)) ? true : false;
								success_flag = true;
							}
						break;

					case PT_CHAR:
						if (json_is_string (json_value_p))
							{
								const char *value_s = json_string_value (json_value_p);

								if (value_s && (strlen (value_s) == 1))
									{
										value_p -> st_char_value = *value_s;
										success_flag = true;
									}
							}
						break;

					case PT_SIGNED_INT:
					case PT_NEGATIVE_INT:
						if (json_is_integer (json_value_p))
							{
								value_p -> st_long_value = (int32) json_integer_value (json_value_p);
								success_flag = true;
							}
						break;

					case PT_UNSIGNED_INT:
						if (json_is_integer (json_value_p))
							{
								value_p -> st_ulong_value = (uint32) json_integer_value (json_value_p);
								success_flag = true;
							}
						break;

					case PT_SIGNED_REAL:
					case PT_UNSIGNED_REAL:
						if (json_is_real (json_value_p))
							{
								value_p -> st_data_value = (double64) json_real_value (json_value_p);
								success_flag = true;
							}
						break;

					
					case PT_DIRECTORY:
					case PT_FILE_TO_READ:
					case PT_FILE_TO_WRITE:
						{
							json_t *protocol_p = json_object_get (json_value_p, RESOURCE_PROTOCOL_S);
							
							if ((protocol_p) && (json_is_string (protocol_p)))
								{		
									json_t *res_value_p = json_object_get (json_value_p, RESOURCE_VALUE_S);									
									if (res_value_p && json_is_string (res_value_p))
										{
											const char *protocol_s = json_string_value (protocol_p);
											const char *value_s = json_string_value (res_value_p);

											value_p -> st_resource_value_p = AllocateResource (protocol_s, value_s, NULL);
											
											success_flag = (value_p -> st_resource_value_p != NULL);
										}
								}					
						}
						break;
					
					case PT_TABLE:
					case PT_STRING:
					case PT_LARGE_STRING:
					case PT_PASSWORD:
					case PT_KEYWORD:
						if (json_is_string (json_value_p))
							{
								const char * const src_s = json_string_value (json_value_p);

								if (strlen (src_s) > 0)
									{
										char *value_s = CopyToNewString (src_s, 0, false);

										if (value_s)
											{
												if (value_p -> st_string_value_s)
													{
														FreeCopiedString (value_p -> st_string_value_s);
													}

												value_p -> st_string_value_s = value_s;
												success_flag = true;
											}
									}
								else
									{
										if (value_p -> st_string_value_s)
											{
												FreeCopiedString (value_p -> st_string_value_s);
												value_p -> st_string_value_s = NULL;
											}

										success_flag = true;
									}
							}
						break;

					case PT_JSON:
						{
							if (json_value_p)
								{
									json_t *copied_value_p = json_deep_copy (json_value_p);

									if (copied_value_p)
										{
											if (value_p -> st_json_p)
												{
													WipeJSON (value_p -> st_json_p);
												}

											value_p -> st_json_p = copied_value_p;
											success_flag = true;
										}
								}
							else
								{
									if (value_p -> st_json_p)
										{
											WipeJSON (value_p -> st_json_p);
											value_p -> st_json_p = NULL;
										}
								}
						}
						break;

					default:
						break;
				}		/* switch (param_p -> pa_type) */
			

		}		/* if (json_value_p) */
	else
		{
			/* json params are allowed to be NULL */
			success_flag = (param_type == PT_JSON);
		}

	return success_flag;
}




static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p)
{
	bool success_flag = false;

	if (param_p -> pa_options_p)
		{
			json_t *json_options_p = json_array ();

			if (json_options_p)
				{
					uint32 i = param_p -> pa_options_p -> pmoa_num_options;
					const ParameterMultiOption *option_p = param_p -> pa_options_p -> pmoa_options_p;

					for ( ; i > 0; -- i, ++ option_p)
						{
							json_t *value_p = NULL;

							switch (param_p -> pa_type)
								{
									case PT_BOOLEAN:
										value_p = (option_p -> pmo_value.st_boolean_value) ? json_true () : json_false ();
										break;

									case PT_CHAR:
										{
											char buffer_s [2];

											*buffer_s = option_p -> pmo_value.st_char_value;
											* (buffer_s + 1) = '\0';

											value_p = json_string (buffer_s);
										}
										break;

									case PT_SIGNED_INT:
									case PT_NEGATIVE_INT:
										value_p = json_integer (option_p -> pmo_value.st_long_value);
										break;

									case PT_UNSIGNED_INT:
										value_p = json_integer (option_p -> pmo_value.st_ulong_value);
										break;

									case PT_SIGNED_REAL:
									case PT_UNSIGNED_REAL:
										value_p = json_real (option_p -> pmo_value.st_data_value);
										break;

									case PT_TABLE:
									case PT_STRING:
									case PT_LARGE_STRING:
									case PT_PASSWORD:
									case PT_KEYWORD:
										value_p = json_string (option_p -> pmo_value.st_string_value_s);
										break;

									case PT_FILE_TO_WRITE:
										break;

									case PT_FILE_TO_READ:
										break;

									case PT_DIRECTORY:
										break;

									case PT_JSON:
										break;

									default:
										break;
								}		/* switch (param_p -> pa_type) */

							if (value_p)
								{
									json_t *item_p = json_object ();
									
									success_flag = false;
									
									if (item_p)
										{
											bool res_flag = true;

											if (option_p -> pmo_description_s)
												{
													res_flag = (json_object_set_new (item_p, SHARED_TYPE_DESCRIPTION_S, json_string (option_p -> pmo_description_s)) == 0);
												}

											if (res_flag)
												{
													if (json_object_set_new (item_p, SHARED_TYPE_VALUE_S, value_p) == 0)
														{
															success_flag = (json_array_append_new (json_options_p, item_p) == 0);
														}												
												}
												
											if (!success_flag)
												{
													json_object_clear (item_p);
													json_decref (item_p);
												}													
										}
								}
						}

					if (success_flag)
						{
							if (json_object_set_new (json_p, PARAM_OPTIONS_S, json_options_p) == 0)
								{
									success_flag = true;
								}
							else
								{
									json_decref (json_options_p);
								}
						}

				}		/* if (json_options_p) */

		}
	else
		{
			/* nothing to do */
			success_flag = true;
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, json_p, "AddParameterOptionsToJSON - json_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p)
{
	bool success_flag = false;
	const ParameterBounds * const bounds_p = param_p -> pa_bounds_p;

	if (bounds_p)
		{
			json_t *min_p = NULL;
			json_t *max_p = NULL;

			switch (param_p -> pa_type)
				{
					case PT_BOOLEAN:
						min_p = (bounds_p -> pb_lower.st_boolean_value == true) ? json_true () : json_false ();
						max_p = (bounds_p -> pb_upper.st_boolean_value == true) ? json_true () : json_false ();
						break;

					case PT_SIGNED_INT:
					case PT_NEGATIVE_INT:
						min_p = json_integer (bounds_p -> pb_lower.st_long_value);
						max_p = json_integer (bounds_p -> pb_upper.st_long_value);
						break;

					case PT_UNSIGNED_INT:
						min_p = json_integer (bounds_p -> pb_lower.st_ulong_value);
						max_p = json_integer (bounds_p -> pb_upper.st_ulong_value);
						break;

					case PT_SIGNED_REAL:
					case PT_UNSIGNED_REAL:
						min_p = json_real (bounds_p -> pb_lower.st_data_value);
						max_p = json_real (bounds_p -> pb_upper.st_data_value);
						break;

					default:
						break;
				}		/* switch (param_p -> pa_type) */

			if (min_p)
				{
					success_flag = (json_object_set_new (json_p, PARAM_MIN_S, min_p) == 0);
				}

			if (max_p)
				{
					if (success_flag)
						{
							success_flag = (json_object_set_new (json_p, PARAM_MAX_S, max_p) == 0);
						}
					else
						{
							json_decref (max_p);
						}
				}
		}
	else
		{
			/* nothing to do */
			success_flag = true;
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, json_p, "AddParameterBoundsToJSON - json_p :: ");
	#endif

	return success_flag;
}


static bool InitParameterStoreFromJSON (const json_t *root_p, HashTable *store_p)
{
	bool success_flag = true;
	json_t *store_json_p = json_object_get (root_p, PARAM_STORE_S);

	if (store_json_p)
		{
			const char *key_s;
			const json_t *value_p;

			json_object_foreach (store_json_p, key_s, value_p)
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);

							if (!PutInHashTable (store_p, key_s, value_s))
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"=\"%s\" to parameter store\n", key_s, value_s);
									success_flag = false;
								}
						}
					else
						{
							char *dump_s = json_dumps (value_p, JSON_INDENT (2));
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json value is not a string \"%s\"\n", dump_s);
							free (dump_s);
							success_flag = false;
						}

				}		/* json_object_foreach (store_json_p, key_s, value_p) */

		}		/* if (store_json_p) */

	return success_flag;
}


static bool GetParameterLevelFromJSON (const json_t * const json_p, ParameterLevel *level_p)
{
	bool success_flag = false;
	json_t *value_p = json_object_get (json_p, PARAM_LEVEL_S);

	if (value_p && json_is_integer (value_p))
		{
			json_int_t subtype = json_integer_value (value_p);

			*level_p = (ParameterLevel) (subtype);
			success_flag = true;
		}


	return success_flag;
}


static bool GetParameterTypeFromJSON (const json_t * const json_p, ParameterType *param_type_p)
{
	bool success_flag = false;

	const char *value_s = GetJSONString (json_p, PARAM_GRASSROOTS_TYPE_INFO_TEXT_S);

	if (value_s)
		{
			if (GetGrassrootsTypeFromString (value_s, param_type_p))
				{
					success_flag = true;
				}
		}		/* if (value_s) */
	else
		{
			/* Backward compatibility */

			if (GetParameterTypeFromCompoundObject (json_p, param_type_p))
				{
					success_flag = true;
				}
			else
				{
					success_flag = GetParameterTypeFromSeparateObjects (json_p, param_type_p);
				}
		}

	return success_flag;
}


static bool GetParameterOptionsFromJSON (const json_t * const json_p, ParameterMultiOptionArray **options_pp, const ParameterType pt)
{
	bool success_flag = true;

	json_t *options_json_p = json_object_get (json_p, PARAM_OPTIONS_S);
	
	if (options_json_p)
		{
			success_flag = false;
				
			if (json_is_array (options_json_p))
				{
					const size_t num_options = json_array_size (options_json_p);
					
					const char **descriptions_ss = (const char **) AllocMemoryArray (num_options, sizeof (const char *));
					
					if (descriptions_ss)
						{
							SharedType *values_p = (SharedType *) AllocMemoryArray (num_options, sizeof (SharedType));
							
							if (values_p)
								{
									/* fill in the values */
									const char **description_ss = descriptions_ss;
									SharedType *value_p = values_p;
									size_t i = 0;
									
									success_flag = true;
															
									while (success_flag && (i < num_options))
										{
											json_t *json_value_p = json_array_get (options_json_p, i);
											
											if (json_value_p)
												{
													success_flag = GetValueFromJSON (json_value_p, SHARED_TYPE_VALUE_S, pt, value_p);

													if (success_flag)
														{
															json_t *desc_p = json_object_get (json_value_p, SHARED_TYPE_DESCRIPTION_S);
															
															if (desc_p && json_is_string (desc_p))
																{
																	*description_ss = json_string_value (desc_p);
																}
														}
												}
																						
											if (success_flag)
												{
													++ i;
													++ description_ss;
													++ value_p;
												}
												
										}		/* while (success_flag && (i > 0)) */
									
									if (success_flag)
										{
											ParameterMultiOptionArray *options_array_p = AllocateParameterMultiOptionArray (num_options, descriptions_ss, values_p, pt, false);
											
											if (options_array_p)
												{
													*options_pp = options_array_p;
												}
											else
												{
													success_flag = false;
												}
										}

									FreeMemory (values_p);
								}		/* if (values_p) */
							
							FreeMemory (descriptions_ss);
						}		/* if (descriptions_ss) */
				}
		}
	
	return success_flag;
}


static bool GetParameterBoundsFromJSON (const json_t * const json_p, ParameterBounds **bounds_pp, const ParameterType pt)
{
	bool success_flag = true;
	const json_t *min_p = json_object_get (json_p, PARAM_MIN_S);

	if (min_p)
		{
			const json_t *max_p = json_object_get (json_p, PARAM_MAX_S);

			if (max_p)
				{
					ParameterBounds *bounds_p = AllocateParameterBounds ();

					if (bounds_p)
						{
							success_flag = false;

							switch (pt)
								{
									case PT_BOOLEAN:
										{
											if (json_is_boolean (min_p))
												{
													bounds_p -> pb_lower.st_boolean_value = (min_p == json_true ());

													if (json_is_boolean (max_p))
														{
															bounds_p -> pb_upper.st_boolean_value = (max_p == json_true ());
															success_flag = true;
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "max_p is %d not boolean", json_typeof (max_p));
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "min_p is %d not boolean", json_typeof (min_p));
												}
										}
										break;

									case PT_SIGNED_INT:
									case PT_NEGATIVE_INT:
										{
											if (json_is_integer (min_p))
												{
													bounds_p -> pb_lower.st_long_value = json_integer_value (min_p);

													if (json_is_integer (max_p))
														{
															bounds_p -> pb_upper.st_long_value = json_integer_value (max_p);
															success_flag = true;
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "max_p is %d not integer", json_typeof (max_p));
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "min_p is %d not integer", json_typeof (min_p));
												}
										}
										break;

									case PT_UNSIGNED_INT:
										{
											if (json_is_integer (min_p))
												{
													bounds_p -> pb_lower.st_ulong_value = json_integer_value (min_p);

													if (json_is_integer (max_p))
														{
															bounds_p -> pb_upper.st_ulong_value = json_integer_value (max_p);
															success_flag = true;
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "max_p is %d not integer", json_typeof (max_p));
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "min_p is %d not integer", json_typeof (min_p));
												}
										}
										break;

									case PT_SIGNED_REAL:
									case PT_UNSIGNED_REAL:
										{
											if (json_is_real (min_p))
												{
													bounds_p -> pb_lower.st_data_value = json_integer_value (min_p);

													if (json_is_real (max_p))
														{
															bounds_p -> pb_upper.st_data_value = json_integer_value (max_p);
															success_flag = true;
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "max_p is %d not real", json_typeof (max_p));
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "min_p is %d not real", json_typeof (min_p));
												}
										}
										break;

									default:
										break;
								}

							if (success_flag)
								{
									*bounds_pp = bounds_p;
								}
							else
								{
									FreeParameterBounds (bounds_p, pt);
								}

						}		/* if (bounds_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate bounds");
							success_flag = false;
						}
				}		/* if (max_p) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get matching max value");
				}

		}		/* if (min_p) */

	return success_flag;
}


bool CopySharedType (const SharedType src, SharedType *dest_p, const ParameterType pt)
{
	bool success_flag = false;

	switch (pt)
		{
			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				{
					Resource *dest_res_p = CloneResource (src.st_resource_value_p);

					if (dest_res_p)
						{
							if (dest_p -> st_resource_value_p)
								{
									FreeResource (dest_p -> st_resource_value_p);
								}

							dest_p -> st_resource_value_p = dest_res_p;
							success_flag = true;
						}

				}
				break;

			case PT_TABLE:
			case PT_STRING:
			case PT_LARGE_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				{
					if (src.st_string_value_s)
						{
							char *copied_value_s = CopyToNewString (src.st_string_value_s, 0, false);

							if (copied_value_s)
								{
									if (dest_p -> st_string_value_s)
										{
											FreeCopiedString (dest_p -> st_string_value_s);
										}

									dest_p -> st_string_value_s = copied_value_s;
									success_flag = true;
								}
							else
								{

								}
						}
				}
				break;

			case PT_JSON:
				{
					if (src.st_json_p)
						{
							json_t *copied_value_p = json_deep_copy (src.st_json_p);

							if (copied_value_p)
								{
									if (dest_p -> st_json_p)
										{
											json_decref (dest_p -> st_json_p);
										}

									dest_p -> st_json_p = copied_value_p;
									success_flag = true;
								}
						}
				}
			break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				dest_p -> st_long_value = src.st_long_value;
				success_flag = true;
				break;

			case PT_UNSIGNED_INT:
				dest_p -> st_ulong_value = src.st_ulong_value;
				success_flag = true;
				break;

			case PT_UNSIGNED_REAL:
			case PT_SIGNED_REAL:
				dest_p -> st_data_value = src.st_data_value;
				success_flag = true;
				break;

			case PT_CHAR:
				dest_p -> st_char_value = src.st_char_value;
				success_flag = true;
				break;

			case PT_BOOLEAN:
				dest_p -> st_boolean_value = src.st_boolean_value;
				success_flag = true;
				break;

			default:
				PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Invalid Paramater Type %d", pt);
				break;
		}


	return success_flag;
}


void ClearSharedType (SharedType *st_p, const ParameterType pt)
{
	switch (pt)
		{
			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				if (st_p -> st_resource_value_p)
					{
						FreeResource (st_p -> st_resource_value_p);
						st_p -> st_resource_value_p = NULL;
					}
				break;

			case PT_TABLE:
			case PT_STRING:
			case PT_LARGE_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				if (st_p -> st_string_value_s)
					{
						FreeCopiedString (st_p -> st_string_value_s);
						st_p -> st_string_value_s = NULL;
					}
				break;

			case PT_JSON:
				if (st_p -> st_json_p)
					{
						#if PARAMETER_DEBUG >= STM_LEVEL_FINER
						PrintJSONRefCounts (st_p -> st_json_p, "freeing param json", STM_LEVEL_FINER, __FILE__, __LINE__);
						#endif

						json_decref (st_p -> st_json_p);
						st_p -> st_json_p = NULL;
					}
				break;

			default:
				break;
		}

	memset (st_p, 0, sizeof (SharedType));
}


bool IsJSONParameterConcise (const json_t * const json_p)
{
	bool concise_flag = false;
	json_t *value_p = json_object_get (json_p, PARAM_CONCISE_DEFINITION_S);
	
	concise_flag = (value_p && json_is_true (value_p));
	
	return concise_flag;
}


/*
 
 				if (AddCurrentValueToJSON (parameter_p, root_p))
						{
							if (AddParameterTagToJSON (parameter_p, root_p))
								{
									if (AddParameterTypeToJSON (parameter_p, root_p))
*/

Parameter *CreateParameterFromJSON (const json_t * const root_p, const bool concise_flag)
{
	Parameter *param_p = NULL;
	const char *name_s = GetJSONString (root_p, PARAM_NAME_S);

	#if SERVER_DEBUG >= STM_LEVEL_FINE
	char *root_s = json_dumps (root_p, JSON_INDENT (2));
	#endif

	if (name_s)
		{
			ParameterType pt;

			if (GetParameterTypeFromJSON (root_p, &pt))
				{
					SharedType current_value;

					memset (&current_value, 0, sizeof (SharedType));

					if (GetValueFromJSON (root_p, PARAM_CURRENT_VALUE_S, pt, &current_value))
						{
							/*
							 * The default, options, display name and bounds are optional
							 */
							const char *description_s = NULL;
							const char *display_name_s = NULL;
							bool multi_valued_flag = false;
							SharedType def;
							ParameterMultiOptionArray *options_p = NULL;
							ParameterBounds *bounds_p = NULL;
							ParameterLevel level = PL_ALL;
							bool success_flag = false;

							memset (&def, 0, sizeof (SharedType));

							if (GetParameterLevelFromJSON (root_p, &level))
								{

								}


							if (! (concise_flag || IsJSONParameterConcise (root_p)))
								{
									description_s = GetJSONString (root_p, PARAM_DESCRIPTION_S);
									display_name_s = GetJSONString (root_p, PARAM_DISPLAY_NAME_S);

									if (GetValueFromJSON (root_p, PARAM_DEFAULT_VALUE_S, pt, &def))
										{
											if (GetParameterOptionsFromJSON (root_p, &options_p, pt))
												{
													if (GetParameterBoundsFromJSON (root_p, &bounds_p, pt))
														{
															success_flag = true;
														}

												}		/* if (GetParameterOptionsFromJSON (root_p, &options_p, pt)) */

										}		/* if (GetValueFromJSON (root_p, PARAM_DEFAULT_VALUE_S, pt, &def)) */
								}
							else
								{
									success_flag = true;
								}

							if (success_flag)
								{
									param_p = AllocateParameter (NULL, pt, multi_valued_flag, name_s, display_name_s, description_s, options_p, def, &current_value, bounds_p, level, NULL);

									if (param_p)
										{
											/* AllocateParameter made a deep copy of the current and default values, so we can deallocate our cached copies */
											ClearSharedType (&current_value, pt);
											ClearSharedType (&def, pt);

											if (SetRemoteParameterDetailsFromJSON (param_p, root_p))
												{
													success_flag = InitParameterStoreFromJSON (root_p, param_p -> pa_store_p);
												}
											else
												{
													success_flag = false;
												}

											if (!success_flag)
												{
													FreeParameter (param_p);
													param_p = NULL;
												}
										}
								}

						}		/* if (GetParameterCurrentValueFromJSON (root_p, &tag)) */

				
				}		/* if (GetParameterTypeFromJSON (root_p, &pt)) */	
				
		}		/* if (name_s) */


	#if SERVER_DEBUG >= STM_LEVEL_FINE
	if (root_s)
		{
			free (root_s);
		}
	#endif


	return param_p;
}



const char *GetUIName (const Parameter * const parameter_p)
{
	if (parameter_p -> pa_display_name_s)
		{
			return (parameter_p -> pa_display_name_s);			
		}
	else
		{
			return (parameter_p -> pa_name_s);
		}
	
}



char *GetParameterValueAsString (const Parameter * const param_p, bool *alloc_flag_p)
{
	char *value_s = NULL;
	const SharedType * const value_p = & (param_p -> pa_current_value);

	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				{
					const char *src_s = (value_p -> st_boolean_value == true) ? "true" : "false";
					value_s = CopyToNewString (src_s, 0, false);
					*alloc_flag_p = true;
				}
				break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_long_value), 0);
				*alloc_flag_p = true;
				break;

			case PT_UNSIGNED_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_ulong_value), 0);
				*alloc_flag_p = true;
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				value_s = ConvertNumberToString (value_p -> st_data_value, 0);
				*alloc_flag_p = true;
				break;
			
			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				value_s = value_p -> st_resource_value_p -> re_value_s;
				*alloc_flag_p = false;
				break;

			case PT_TABLE:
			case PT_LARGE_STRING:
			case PT_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				value_s = value_p -> st_string_value_s;
				*alloc_flag_p = false;
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */


	return value_s;
}



bool SetParameterValueFromString (Parameter * const param_p, const char *value_s)
{
	bool success_flag = SetSharedTypeFromString (& (param_p -> pa_current_value), param_p -> pa_type, value_s);

	if (!success_flag)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set parameter \"%s\" with type " UINT32_FMT " to \"%s\"", param_p -> pa_name_s, param_p -> pa_type, value_s);
		}

	return success_flag;
}



bool SetSharedTypeFromString (SharedType * const value_p, const ParameterType pt, const char *value_s)
{
	bool success_flag = false;

	switch (pt)
		{
			case PT_BOOLEAN:
				{
					if (Stricmp (value_s, "true") == 0)
						{
							value_p -> st_boolean_value = true;
							success_flag = true;
						}
					else if (Stricmp (value_s, "false") == 0)
						{
							value_p -> st_boolean_value = false;
							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				{
					int32 value;

					if (sscanf (value_s, INT32_FMT, &value) > 0)
						{
							value_p -> st_long_value = value;
							success_flag = true;
						}
				}
				break;

			case PT_UNSIGNED_INT:
				{
					uint32 value;

					if (sscanf (value_s, UINT32_FMT, &value) > 0)
						{
							value_p -> st_ulong_value = value;
							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					double64 value;

					if (sscanf (value_s, DOUBLE64_FMT, &value) > 0)
						{
							value_p -> st_data_value = value;
							success_flag = true;
						}
				}
				break;

			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				{
					Resource *resource_p = ParseStringToResource (value_s);

					if (resource_p)
						{
							success_flag = SetSharedTypeResourceValue (value_p, resource_p);
							FreeResource (resource_p);
						}
				}
				break;

			case PT_TABLE:
			case PT_LARGE_STRING:
			case PT_STRING:
			case PT_PASSWORD:
			case PT_KEYWORD:
				success_flag = SetSharedTypeStringValue (value_p, value_s);
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */


	return success_flag;
}


SharedTypeNode *AllocateSharedTypeNode (SharedType value)
{
	SharedTypeNode *node_p = (SharedTypeNode *) AllocMemory (sizeof (SharedTypeNode));

	if (node_p)
		{
			node_p -> stn_node.ln_prev_p = NULL;
			node_p -> stn_node.ln_next_p = NULL;

			memcpy (& (node_p -> stn_value), &value, sizeof (SharedType));
		}

	return node_p;
}


void FreeSharedTypeNode (ListItem *node_p)
{
	FreeMemory (node_p);
}


bool AddRemoteDetailsToParameter (Parameter *param_p, const char * const uri_s, const Tag tag)
{
	bool success_flag = false;
	RemoteParameterDetailsNode *node_p = AllocateRemoteParameterDetailsNodeByParts (uri_s, tag);

	if (node_p)
		{
			LinkedListAddTail (param_p -> pa_remote_parameter_details_p, (ListItem *) node_p);
			success_flag = true;
		}

	return success_flag;
}



/*************************************************/
/************** STATIC FUNCTIONS *****************/
/*************************************************/


static bool AddParameterGroupToJSON (const Parameter * const param_p, json_t *json_p, const SchemaVersion * const sv_p)
{
	bool success_flag = true;

	if (param_p -> pa_group_p)
		{
			const char *group_name_s = param_p -> pa_group_p -> pg_name_s;

			if (group_name_s)
				{
					if (json_object_set_new (json_p, PARAM_GROUP_S, json_string (group_name_s)) != 0)
						{
							success_flag = false;
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set group name to \"%s\" for param \"%s\"", group_name_s, param_p -> pa_name_s);
						}
				}

		}		/* if (param_p -> pa_group_p) */

	return success_flag;
}



static LinkedList *CreateSharedTypesList (const SharedType *values_p)
{
	LinkedList *values_list_p = AllocateLinkedList (FreeSharedTypeNode);

	if (values_list_p)
		{
			bool success_flag = true;
			const SharedType *value_p = values_p;

			while (value_p && success_flag)
				{
					SharedTypeNode *node_p = AllocateSharedTypeNode (*value_p);

					if (node_p)
						{
							LinkedListAddTail (values_list_p, (ListItem * const) node_p);
							++ value_p;
						}
					else
						{
							success_flag = false;
						}
				}		/* while (value_p && success_flag) */

			if (success_flag)
				{
					return values_list_p;
				}

			FreeLinkedList (values_list_p);
		}

	return NULL;
}


static LinkedList *CopySharedTypesList (const LinkedList *source_p)
{
	LinkedList *dest_p = AllocateLinkedList (FreeSharedTypeNode);

	if (dest_p)
		{
			bool success_flag = true;
			const SharedTypeNode *src_node_p = (SharedTypeNode *) (source_p -> ll_head_p);

			while (src_node_p && success_flag)
				{
					SharedTypeNode *dest_node_p = AllocateSharedTypeNode (src_node_p -> stn_value);

					if (dest_node_p)
						{
							LinkedListAddTail (dest_p, (ListItem * const) dest_node_p);
							src_node_p = (SharedTypeNode *) (src_node_p -> stn_node.ln_next_p);
						}
					else
						{
							success_flag = false;
						}
				}		/* while (value_p && success_flag) */

			if (success_flag)
				{
					return dest_p;
				}

			FreeLinkedList (dest_p);
		}

	return NULL;
}


static bool SetParameterValueFromBoolean (Parameter * const param_p, const bool b, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeBooleanValue (& (param_p -> pa_current_value), b);
		}
	else
		{
			success_flag = SetSharedTypeBooleanValue (& (param_p -> pa_default), b);
		}

	return success_flag;
}


static bool SetParameterValueFromChar (Parameter * const param_p, const char c, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeCharValue (& (param_p -> pa_current_value), c);
		}
	else
		{
			success_flag = SetSharedTypeCharValue (& (param_p -> pa_default), c);
		}

	return success_flag;
}


static bool SetParameterValueFromSignedInt (Parameter * const param_p, const int32 i, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeSignedIntValue (& (param_p -> pa_current_value), i, param_p -> pa_bounds_p);
		}
	else
		{
			success_flag = SetSharedTypeSignedIntValue (& (param_p -> pa_default), i, param_p -> pa_bounds_p);
		}

	return success_flag;
}


static bool SetParameterValueFromUnsignedInt (Parameter * const param_p, const uint32 i, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeUnsignedIntValue (& (param_p -> pa_current_value), i, param_p -> pa_bounds_p);
		}
	else
		{
			success_flag = SetSharedTypeUnsignedIntValue (& (param_p -> pa_default), i, param_p -> pa_bounds_p);
		}

	return success_flag;
}


static bool SetParameterValueFromReal (Parameter * const param_p, const double64 d, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeRealValue (& (param_p -> pa_current_value), d, param_p -> pa_bounds_p);
		}
	else
		{
			success_flag = SetSharedTypeRealValue (& (param_p -> pa_default), d, param_p -> pa_bounds_p);
		}

	return success_flag;
}



static bool SetParameterValueFromStringValue (Parameter * const param_p, const char * const src_s, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeStringValue (& (param_p -> pa_current_value), src_s);
		}
	else
		{
			success_flag = SetSharedTypeStringValue (& (param_p -> pa_default), src_s);
		}

	return success_flag;
}


static bool SetParameterValueFromResource (Parameter * const param_p, const Resource * const src_p, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeResourceValue (& (param_p -> pa_current_value), src_p);
		}
	else
		{
			success_flag = SetSharedTypeResourceValue (& (param_p -> pa_default), src_p);
		}

	return success_flag;
}


static bool SetParameterValueFromJSON (Parameter * const param_p, const json_t * const src_p, const bool current_flag)
{
	bool success_flag = false;

	if (current_flag)
		{
			success_flag = SetSharedTypeJSONValue (& (param_p -> pa_current_value), src_p);
		}
	else
		{
			success_flag = SetSharedTypeJSONValue (& (param_p -> pa_default), src_p);
		}

	return success_flag;
}




static bool SetSharedTypeBooleanValue (SharedType * value_p, const bool b)
{
	value_p -> st_boolean_value = b;

	return true;
}


static bool SetSharedTypeCharValue (SharedType * value_p, const char c)
{
	value_p -> st_char_value = c;

	return true;
}



static bool SetSharedTypeUnsignedIntValue (SharedType * value_p, const uint32 i, const ParameterBounds * const bounds_p)
{
	bool success_flag = false;

	if (bounds_p)
		{
			if ((i >= bounds_p -> pb_lower.st_ulong_value) &&
					(i <= bounds_p -> pb_upper.st_ulong_value))
				{
					value_p -> st_ulong_value = i;
					success_flag = true;
				}
		}
	else
		{
			value_p -> st_ulong_value = i;
			success_flag = true;
		}

	return success_flag;
}


static bool SetSharedTypeSignedIntValue (SharedType * value_p, const int32 i, const ParameterBounds * const bounds_p)
{
	bool success_flag = false;

	if (bounds_p)
		{
			if ((i >= bounds_p -> pb_lower.st_long_value) &&
					(i <= bounds_p -> pb_upper.st_long_value))
				{
					value_p -> st_long_value = i;
					success_flag = true;
				}
		}
	else
		{
			value_p -> st_long_value = i;
			success_flag = true;
		}

	return success_flag;
}



static bool SetSharedTypeRealValue (SharedType * value_p, const double64 d, const ParameterBounds * const bounds_p)
{
	bool success_flag = false;

	if (bounds_p)
		{
			if ((d >= bounds_p -> pb_lower.st_data_value) &&
					(d <= bounds_p -> pb_upper.st_data_value))
				{
					value_p -> st_data_value = d;
					success_flag = true;
				}
		}
	else
		{
			value_p -> st_data_value = d;
			success_flag = true;
		}

	return success_flag;
}


static bool SetSharedTypeStringValue (SharedType *value_p, const char * const src_s)
{
	bool success_flag = false;

	if (src_s)
		{
			char *copied_value_s = CopyToNewString (src_s, 0, false);

			if (copied_value_s)
				{
					/* If we have a previous value, delete it */
					if (value_p -> st_string_value_s)
						{
							FreeCopiedString (value_p -> st_string_value_s);
						}

					value_p -> st_string_value_s = copied_value_s;
					success_flag = true;
				}
		}
	else
		{
			/* If we have a previous value, delete it */
			if (value_p -> st_string_value_s)
				{
					FreeCopiedString (value_p -> st_string_value_s);
					value_p -> st_string_value_s = NULL;
				}

			success_flag = true;
		}

	return success_flag;
}


static bool SetSharedTypeResourceValue (SharedType *value_p, const Resource * const src_p)
{
	bool success_flag = false;

	if (src_p)
		{
			if (value_p -> st_resource_value_p)
				{
					success_flag = CopyResource (src_p, value_p -> st_resource_value_p);
				}
			else
				{
					value_p -> st_resource_value_p = AllocateResource (src_p -> re_protocol_s, src_p -> re_value_s, src_p -> re_title_s);

					success_flag = (value_p -> st_resource_value_p != NULL);
				}
		}
	else
		{
			if (value_p -> st_resource_value_p)
				{
					FreeResource (value_p -> st_resource_value_p);
					value_p -> st_resource_value_p = NULL;
				}

			success_flag = true;
		}

	return success_flag;
}


static bool SetSharedTypeJSONValue (SharedType *value_p, const json_t * const src_p)
{
	bool success_flag = false;

	if (src_p)
		{
			json_t *json_value_p = json_deep_copy (src_p);

			if (json_value_p)
				{
					/* If we have a previous value, delete it */
					if (value_p -> st_json_p)
						{
							WipeJSON (value_p -> st_json_p);
						}

					value_p -> st_json_p = json_value_p;
					success_flag = true;
				}
		}
	else
		{
			/* If we have a previous value, delete it */
			if (value_p -> st_json_p)
				{
					WipeJSON (value_p -> st_json_p);
					value_p -> st_json_p = NULL;
				}

			success_flag = true;
		}

	return success_flag;
}



static bool SetRemoteParameterDetailsFromJSON (Parameter *param_p, const json_t * json_p)
{
	bool success_flag = true;
	const json_t *remote_p = json_object_get (json_p, PARAM_REMOTE_S);

	if (remote_p)
		{
			if (json_is_array (remote_p))
				{

				}
		}

	return success_flag;
}




bool SetSharedTypeFromJSON (SharedType *value_p, const json_t *json_p, const ParameterType pt)
{
	bool success_flag = false;

	switch (pt)
		{
			case PT_BOOLEAN:
				{
					success_flag = SetBooleanFromJSON (json_p, & (value_p -> st_boolean_value));
				}
				break;

			case PT_SIGNED_INT:
			case PT_NEGATIVE_INT:
				{
					success_flag = SetIntegerFromJSON (json_p, & (value_p -> st_long_value));
				}
				break;

			case PT_UNSIGNED_INT:
				{
					int32 i;

					if (SetIntegerFromJSON (json_p, &i))
						{
							value_p -> st_ulong_value = (uint32) i;
							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					success_flag = SetRealFromJSON (json_p, & (value_p -> st_data_value));
				}
				break;

			case PT_STRING:
			case PT_FILE_TO_WRITE:
			case PT_FILE_TO_READ:
			case PT_DIRECTORY:
			case PT_KEYWORD:
			case PT_LARGE_STRING:
			case PT_PASSWORD:
				{
					char *value_s = NULL;

					success_flag = SetStringFromJSON (json_p, &value_s);

					if (success_flag)
						{
							if (value_p -> st_string_value_s)
								{
									FreeCopiedString (value_p -> st_string_value_s);
								}

							value_p -> st_string_value_s = value_s;
						}
				}
				break;

			case PT_CHAR:
				{
					char *value_s = NULL;

					if (SetStringFromJSON (json_p, &value_s))
						{
							if (strlen (value_s) == 1)
								{
									if (value_p -> st_string_value_s)
										{
											FreeCopiedString (value_p -> st_string_value_s);
										}

									value_p -> st_string_value_s = value_s;
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "\"%s\" is too long for single char value", value_s);
								}
						}
				}
				break;

			case PT_JSON:
				break;

			case PT_TABLE:
				break;

		}		/* switch (pt) */

	return success_flag;
}



bool GetParameterGroupVisibility (const ServiceData *service_data_p, const char *group_name_s, bool *visibility_p)
{
	bool found_flag = false;

	if (service_data_p -> sd_config_p)
		{
			const json_t *groups_config_p = json_object_get (service_data_p -> sd_config_p, PARAM_SET_GROUPS_S);

			if (groups_config_p)
				{
					const json_t *group_config_p = json_object_get (groups_config_p, group_name_s);

					if (group_config_p)
						{
							found_flag = GetJSONBoolean (group_config_p, PARAM_GROUP_VISIBLE_S, visibility_p);
						}
				}
		}

	return found_flag;
}




bool GetParameterDefaultValueFromConfig (const ServiceData *service_data_p, const char *param_name_s, const ParameterType pt, SharedType *value_p)
{
	bool found_flag = false;

	const json_t *param_config_p = GetParameterFromConfig (service_data_p -> sd_config_p, param_name_s);

	if (param_config_p)
		{
			const json_t *param_default_value_p = json_object_get (param_config_p, PARAM_DEFAULT_VALUE_S);

			if (param_default_value_p)
				{
					if (SetSharedTypeFromJSON (value_p, param_default_value_p, pt))
						{
							found_flag = true;
						}
				}
		}

	return found_flag;
}



bool GetParameterLevelFromConfig (const ServiceData *service_data_p, const char *param_name_s, ParameterLevel *level_p)
{
	bool found_flag = false;

	const json_t *param_config_p = GetParameterFromConfig (service_data_p -> sd_config_p, param_name_s);

	if (param_config_p)
		{
			found_flag = GetParameterLevelFromJSON (param_config_p, level_p);
		}

	return found_flag;
}



bool GetParameterDescriptionFromConfig (const ServiceData *service_data_p, const char *param_name_s, char **description_ss)
{
	return GetParameterStringFromConfig (service_data_p -> sd_config_p, param_name_s, PARAM_DESCRIPTION_S, description_ss);
}


bool GetParameterDisplayNameFromConfig (const ServiceData *service_data_p, const char *param_name_s, char **display_name_ss)
{
	return GetParameterStringFromConfig (service_data_p -> sd_config_p, param_name_s, PARAM_DISPLAY_NAME_S, display_name_ss);
}




static const json_t *GetParameterFromConfig (const json_t *service_config_p, const char * const param_name_s)
{
	if (service_config_p)
		{
			const json_t *params_config_p = json_object_get (service_config_p, PARAM_SET_PARAMS_S);

			if (params_config_p)
				{
					const json_t *param_config_p = json_object_get (params_config_p, param_name_s);

					if (param_config_p)
						{
							return param_config_p;
						}
				}
		}

	return NULL;
}


static bool GetParameterStringFromConfig (const json_t *service_config_p, const char * const param_name_s, const char * const key_s, char **value_ss)
{
	bool found_flag = false;

	const json_t *param_config_p = GetParameterFromConfig (service_config_p, param_name_s);

	if (param_config_p)
		{
			const char *value_s = GetJSONString (param_config_p, key_s);

			if (value_s)
				{
					char *copied_value_s = CopyToNewString (value_s, 0, false);

					if (copied_value_s)
						{
							if (*value_ss)
								{
									FreeCopiedString (*value_ss);
								}

							*value_ss = copied_value_s;

							found_flag = true;
						}		/* if (copied_value_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to make copy of \"%s\" for param \"%s\":\"%s\"", value_s, param_name_s, key_s);
						}
				}
		}

	return found_flag;
}
