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
#include "math_utils.h"
#include "string_utils.h"
#include "string_hash_table.h"
#include "parameter_set.h"

#include "json_tools.h"
#include "json_util.h"

#ifdef _DEBUG
	#define PARAMETER_DEBUG	(STM_LEVEL_FINE)
#else
	#define PARAMETER_DEBUG	(STM_LEVEL_NONE)
#endif


static ParameterMultiOptionArray *AllocateEmptyParameterMultiOptionArray (const uint32 num_options);

static bool AddParameterNameToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterDisplayNameToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterTagToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterTypeToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddCurrentValueToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p);

static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p);

static bool AddParameterGroupToJSON (const Parameter * const param_p, json_t *json_p);

static bool AddParameterStoreToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterLevelToJSON (const Parameter * const param_p, json_t *root_p);

static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p);

static bool AddValueToJSON (json_t *root_p, const ParameterType pt, const SharedType *val_p, const char *key_s);

static bool GetParameterBoundsFromJSON (const json_t * const json_p, ParameterBounds **bounds_pp);

static bool GetParameterTagFromJSON (const json_t * const json_p, Tag *tag_p);

static bool GetParameterLevelFromJSON (const json_t * const json_p, ParameterLevel *level_p);

static bool InitParameterStoreFromJSON (const json_t *root_p, HashTable *store_p);


static LinkedList *CreateSharedTypesList (const SharedType *values_p);


static LinkedList *CopySharedTypesList (const LinkedList *source_p);


static bool SetParameterValueFromBoolean (Parameter * const param_p, const bool b, const bool current_flag);


static bool SetParameterValueFromChar (Parameter * const param_p, const char c, const bool current_flag);


static bool SetParameterValueFromSignedInt (Parameter * const param_p, const int32 i, const bool current_flag);


static bool SetParameterValueFromUnsignedInt (Parameter * const param_p, const uint32 i, const bool current_flag);


static bool SetParameterValueFromReal (Parameter * const param_p, const double64 d, const bool current_flag);


static bool SetParameterValueFromString (Parameter * const param_p, const char * const src_s, const bool current_flag);


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


/******************************************************/


Parameter *AllocateParameter (ParameterType type, bool multi_valued_flag, const char * const name_s, const char * const display_name_s, const char * const description_s, Tag tag, ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
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
											param_p -> pa_tag = tag;
											param_p -> pa_store_p = store_p;
											param_p -> pa_group_p = NULL;

											memset (& (param_p -> pa_current_value), 0, sizeof (SharedType));
											memset (& (param_p -> pa_default), 0, sizeof (SharedType));

											if (multi_valued_flag)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Multi-valued parameters not yet implemented");
												}
											else
												{
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

	FreeMemory (param_p);
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


ParameterMultiOptionArray *AllocateParameterMultiOptionArray (const uint32 num_options, const char ** const descriptions_pp, SharedType *values_p, ParameterType pt)
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

					if (!SetParameterMultiOption (array_p, i, description_s, *value_p))
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

			if ((options_p -> pmoa_values_type == PT_STRING) || (options_p -> pmoa_values_type == PT_PASSWORD) || (options_p -> pmoa_values_type == PT_LARGE_STRING) || (options_p -> pmoa_values_type == PT_TABLE))
				{
					FreeCopiedString (option_p -> pmo_value.st_string_value_s);
				}

		}		/* for ( ; i > 0; -- i, ++ option_p) */

	FreeMemory (options_p -> pmoa_options_p);
	FreeMemory (options_p);
}


bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value)
{
	ParameterMultiOption *option_p = (options_p -> pmoa_options_p) + index;
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
			if ((options_p -> pmoa_values_type == PT_STRING) || (options_p -> pmoa_values_type == PT_PASSWORD) || (options_p -> pmoa_values_type == PT_LARGE_STRING) || (options_p -> pmoa_values_type == PT_TABLE))
				{
					char *value_s = CopyToNewString (value.st_string_value_s, 0, false);

					if (value_s)
						{
							option_p -> pmo_value.st_string_value_s = value_s;
						}
					else
						{
							success_flag = false;
						}
				}
			else
				{
					option_p -> pmo_value = value;
					success_flag = true;
				}
		}

	return success_flag;
}



const char *CheckForSignedReal (const Parameter * const parameter_p, const void *value_p)
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


const char *CheckForNotNull (const Parameter * const parameter_p, const void *value_p)
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
				success_flag = SetParameterValueFromString (param_p, src_p -> st_string_value_s, current_value_flag);
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
					success_flag = SetParameterValueFromString (param_p, value_s, current_value_flag);
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


json_t *GetParameterAsJSON (const Parameter * const parameter_p, const bool full_definition_flag)
{
	json_t *root_p = json_object ();

	if (root_p)
		{
			bool success_flag = false;

			if (AddParameterNameToJSON (parameter_p, root_p))
				{
					if (AddCurrentValueToJSON (parameter_p, root_p))
						{
							if (AddParameterTagToJSON (parameter_p, root_p))
								{
									if (AddParameterTypeToJSON (parameter_p, root_p))
										{
											if (AddParameterStoreToJSON (parameter_p, root_p))
												{
													if (AddParameterLevelToJSON (parameter_p, root_p))
														{
															if (full_definition_flag)
																{
																	if (AddParameterDescriptionToJSON (parameter_p, root_p))
																		{
																			if (AddParameterDisplayNameToJSON (parameter_p, root_p))
																				{
																					if (AddDefaultValueToJSON (parameter_p, root_p))
																						{
																							if (AddParameterOptionsToJSON (parameter_p, root_p))
																								{
																									if (AddParameterBoundsToJSON (parameter_p, root_p))
																										{
																											if (AddParameterGroupToJSON (parameter_p, root_p))
																												{
																													success_flag = true;
																												}
																										}
																								}
																						}
																				}
																		}
																}
															else
																{
																	success_flag = (json_object_set_new (root_p, PARAM_CONCISE_DEFINITION_S, json_true ()) == 0);
																}
														}
												}		/* if (full_definition_flag) */
										}
								}													
						}
				}

			if (!success_flag)
				{
					if (json_object_clear (root_p) == 0)
						{
							json_decref (root_p);
							root_p = NULL;
						}
					else
						{
							/* @TODO: handle this error */
						}
				}
		}

	return root_p;
}


static bool AddParameterNameToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = (json_object_set_new (root_p, PARAM_NAME_S, json_string (param_p -> pa_name_s)) == 0);

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterNameToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterDisplayNameToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = true;
	
	if (param_p -> pa_display_name_s)
		{
			success_flag = (json_object_set_new (root_p, PARAM_DISPLAY_NAME_S, json_string (param_p -> pa_display_name_s)) == 0);
		}
		
	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterDisplayNameToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = true;

	if (param_p -> pa_description_s)
		{
			success_flag = (json_object_set_new (root_p, PARAM_DESCRIPTION_S, json_string (param_p -> pa_description_s)) == 0);
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterDescriptionToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterTagToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = (json_object_set_new (root_p, PARAM_TAG_S, json_integer (param_p -> pa_tag)) == 0);

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterTagToJSON - root_p :: ");
	#endif

	return success_flag;	
}


static bool AddParameterLevelToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = (json_object_set_new (root_p, PARAM_LEVEL_S, json_integer (param_p -> pa_level)) == 0);

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterTagToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterStoreToJSON (const Parameter * const param_p, json_t *root_p)
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


static bool AddParameterTypeToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = false;

	/* Set the parameter type */
	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("boolean")) == 0);
				break;

			case PT_CHAR:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("character")) == 0);
				break;

			case PT_SIGNED_INT:
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

	if (success_flag)
		{
			success_flag = (json_object_set_new (root_p, PARAM_GRASSROOTS_TYPE_INFO_S, json_integer (param_p -> pa_type)) == 0);
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSON (stderr, root_p, "AddParameterTypeToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p)
{
	return AddValueToJSON (root_p, param_p -> pa_type, & (param_p -> pa_default), PARAM_DEFAULT_VALUE_S);
}


static bool AddCurrentValueToJSON (const Parameter * const param_p, json_t *root_p)
{
	return AddValueToJSON (root_p, param_p -> pa_type, & (param_p -> pa_current_value), PARAM_CURRENT_VALUE_S);
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
	PrintJSON (stderr, root_p, "AddValueToJSON - root_p :: ");
	#endif

	return success_flag;

}


static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p)
{
	bool success_flag = false;

	/* Get the parameter's value */
	json_t *json_value_p = json_object_get (root_p, key_s);

	if (json_value_p)
		{			
			#if PARAMETER_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (json_value_p, key_s, STM_LEVEL_FINE, __FILE__, __LINE__);
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
								char *value_s = CopyToNewString (json_string_value (json_value_p), 0, false);

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
						break;

					case PT_JSON:
						{
							json_t *copied_value_p = json_deep_copy (json_value_p);

							if (copied_value_p)
								{
									value_p -> st_json_p = copied_value_p;
									success_flag = true;
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




static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p)
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
							char *value_s = NULL;
							bool alloc_flag = false;
							/*
							 * Swagger's schema requires that these values must be strings
							 * and one of them must be the default value for this parameter.
							 */
							switch (param_p -> pa_type)
								{
									case PT_BOOLEAN:
										value_s = (option_p -> pmo_value.st_boolean_value) ? "true" : "false";
										break;

									case PT_CHAR:
										value_s = (char *) AllocMemory (2 * sizeof (char));

										if (value_s)
											{
												*value_s = option_p -> pmo_value.st_char_value;
												* (value_s + 1) = '\0';

												alloc_flag = true;
											}
										break;

									case PT_SIGNED_INT:
										value_s = ConvertIntegerToString (option_p -> pmo_value.st_long_value);
										alloc_flag = (value_s != NULL);
										break;

									case PT_UNSIGNED_INT:
										value_s = ConvertIntegerToString (option_p -> pmo_value.st_ulong_value);
										alloc_flag = (value_s != NULL);
										break;

									case PT_SIGNED_REAL:
									case PT_UNSIGNED_REAL:
										value_s = ConvertDoubleToString (option_p -> pmo_value.st_data_value);
										alloc_flag = (value_s != NULL);
										break;

									case PT_TABLE:
									case PT_STRING:
									case PT_LARGE_STRING:
									case PT_PASSWORD:
									case PT_KEYWORD:
										value_s = option_p -> pmo_value.st_string_value_s;
										break;

									default:
										break;
								}		/* switch (param_p -> pa_type) */

							if (value_s)
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
													if (json_object_set_new (item_p, SHARED_TYPE_VALUE_S, json_string (value_s)) == 0)
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

									if (alloc_flag)
										{
											FreeCopiedString (value_s);
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


static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p)
{
	bool success_flag = false;
	const ParameterBounds * const bounds_p = param_p -> pa_bounds_p;

	if (bounds_p)
		{
			json_t *min_p = NULL;
			json_t *max_p = NULL;
			char *value_s = NULL;

			switch (param_p -> pa_type)
				{
					case PT_BOOLEAN:
						min_p = (bounds_p -> pb_lower.st_boolean_value == true) ? json_true () : json_false ();
						min_p = (bounds_p -> pb_upper.st_boolean_value == true) ? json_true () : json_false ();
						break;

					case PT_SIGNED_INT:
						value_s = ConvertIntegerToString (bounds_p -> pb_lower.st_long_value);

						if (value_s)
							{
								min_p = json_string (value_s);
								FreeCopiedString (value_s);
							}

						value_s = ConvertIntegerToString (bounds_p -> pb_upper.st_long_value);
						if (value_s)
							{
								max_p = json_string (value_s);
								FreeCopiedString (value_s);
							}
						break;

					case PT_UNSIGNED_INT:
						value_s = ConvertIntegerToString (bounds_p -> pb_lower.st_ulong_value);

						if (value_s)
							{
								min_p = json_string (value_s);
								FreeCopiedString (value_s);
							}

						value_s = ConvertIntegerToString (bounds_p -> pb_upper.st_ulong_value);
						if (value_s)
							{
								max_p = json_string (value_s);
								FreeCopiedString (value_s);
							}
						break;

					case PT_SIGNED_REAL:
					case PT_UNSIGNED_REAL:
						value_s = ConvertDoubleToString (bounds_p -> pb_lower.st_data_value);

						if (value_s)
							{
								min_p = json_string (value_s);
								FreeCopiedString (value_s);
							}

						value_s = ConvertDoubleToString (bounds_p -> pb_upper.st_data_value);
						if (value_s)
							{
								max_p = json_string (value_s);
								FreeCopiedString (value_s);
							}
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


static const char *GetStringValue (const json_t * const json_p, const char * const key_s)
{
	const char *name_s = NULL;
	json_t *value_p = json_object_get (json_p, key_s);

	if (value_p && (json_is_string (value_p)))
		{
			name_s = json_string_value (value_p);
		}

	return name_s;
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
	json_t *value_p = json_object_get (json_p, PARAM_GRASSROOTS_TYPE_INFO_S);

	if (value_p && json_is_integer (value_p))
		{
			json_int_t subtype = json_integer_value (value_p);

			if ((subtype >= 0) && (subtype < PT_NUM_TYPES))
				{
					*param_type_p = subtype;
					success_flag = true;
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
											ParameterMultiOptionArray *options_array_p = AllocateParameterMultiOptionArray (num_options, descriptions_ss, values_p, pt);
											
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


static bool GetParameterBoundsFromJSON (const json_t * const json_p, ParameterBounds **bounds_pp)
{
	bool success_flag = true;

	return success_flag;
}


static bool GetParameterTagFromJSON (const json_t * const json_p, Tag *tag_p)
{
	bool success_flag = false;
	json_t *tag_json_p = json_object_get (json_p, PARAM_TAG_S);

	if (tag_json_p)
		{
			if (json_is_integer (tag_json_p))
				{
					*tag_p = json_integer_value (tag_json_p);
					success_flag = true;
				}
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
						json_decref (st_p -> st_json_p);
						st_p -> st_json_p = NULL;
					}
			break;

			default:
				break;
		}
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

Parameter *CreateParameterFromJSON (const json_t * const root_p)
{
	Parameter *param_p = NULL;
	const char *name_s = GetStringValue (root_p, PARAM_NAME_S);

	#if SERVER_DEBUG >= STM_LEVEL_FINE
	char *root_s = json_dumps (root_p, JSON_INDENT (2));
	#endif

	if (name_s)
		{
			ParameterType pt;

			if (GetParameterTypeFromJSON (root_p, &pt))
				{
					Tag tag;
					
					if (GetParameterTagFromJSON (root_p, &tag))
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

									if (!IsJSONParameterConcise (root_p))
										{
											description_s = GetStringValue (root_p, PARAM_DESCRIPTION_S);
											display_name_s = GetStringValue (root_p, PARAM_DISPLAY_NAME_S);

											if (GetValueFromJSON (root_p, PARAM_DEFAULT_VALUE_S, pt, &def))
												{
													if (GetParameterOptionsFromJSON (root_p, &options_p, pt))
														{
															if (GetParameterBoundsFromJSON (root_p, &bounds_p))
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
											param_p = AllocateParameter (pt, multi_valued_flag, name_s, display_name_s, description_s, tag, options_p, def, &current_value, bounds_p, level, NULL);

											if (param_p)
												{

													if (!InitParameterStoreFromJSON (root_p, param_p -> pa_store_p))
														{
															FreeParameter (param_p);
															param_p = NULL;
														}
												}
										}
									
								}		/* if (GetParameterCurrentValueFromJSON (root_p, &tag)) */
					
						}		/* if (GetParameterTagFromJSON (root_p, &tag)) */
				
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



static bool AddParameterGroupToJSON (const Parameter * const param_p, json_t *json_p)
{
	bool success_flag = true;

	if (param_p -> pa_group_p)
		{
			const char *group_name_s = param_p -> pa_group_p -> pg_name_s;

			if (group_name_s)
				{
					success_flag = (json_object_set_new (json_p, PARAM_GROUP_S, json_string (group_name_s)) == 0);
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



static bool SetParameterValueFromString (Parameter * const param_p, const char * const src_s, const bool current_flag)
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
			if ((i >= bounds_p -> pb_lower.st_data_value) &&
					(i <= bounds_p -> pb_upper.st_data_value))
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
			json_error_t err;
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


