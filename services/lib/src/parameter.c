#include <math.h>
#include <stddef.h>
#include <string.h>

#include "memory_allocations.h"
#include "parameter.h"
#include "math_utils.h"
#include "string_utils.h"

#include "json_util.h"


static ParameterMultiOptionArray *AllocateEmptyParameterMultiOptionArray (const uint32 num_options);

static bool AddParameterNameToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterTypeToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddCurrentValueToJSON (const Parameter * const param_p, json_t *root_p);

static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p);

static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p);


static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p);

static bool AddValueToJSON (json_t *root_p, const ParameterType pt, const SharedType *val_p);



Parameter *AllocateParameter (ParameterType type, const char * const name_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	char *new_name_s = CopyToNewString (name_s, 0, true);

	if (new_name_s)
		{
			char *new_description_s = CopyToNewString (description_s, 0, true);

			if (new_description_s)
				{
					Parameter *param_p = (Parameter *) AllocMemory (sizeof (Parameter));

					if (param_p)
						{
							param_p -> pa_type = type;
							param_p -> pa_name_s = new_name_s;
							param_p -> pa_description_s = new_description_s;
							param_p -> pa_options_p = options_p;
							param_p -> pa_check_value_fn = check_value_fn;
							param_p -> pa_default = default_value;
							param_p -> pa_bounds_p = bounds_p;
							param_p -> pa_level = level;
							
							//memset (& (param_p -> pa_current_value), 0, sizeof (SharedType));
							memcpy (& (param_p -> pa_current_value), & (param_p -> pa_default), sizeof (SharedType));
							
							
							return param_p;
						}		/* if (param_p) */

					FreeCopiedString (new_description_s);
				}		/* if (new_description_s) */

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

	if (param_p -> pa_options_p)
		{
			FreeParameterMultiOptionArray (param_p -> pa_options_p);
		}

	if (param_p -> pa_bounds_p)
		{
			FreeParameterBounds (param_p -> pa_bounds_p, param_p -> pa_type);
		}

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
					case PT_STRING:
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
	if ((pt == PT_STRING) || (pt == PT_FILE_TO_READ) || (pt == PT_FILE_TO_WRITE) || (pt == PT_DIRECTORY))
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

			for (i = 0; i < num_options; ++ i, ++ option_p, ++ description_pp, ++ value_p)
				{
					if (!SetParameterMultiOption (array_p, i, *description_pp, *value_p))
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

			if (options_p -> pmoa_values_type == PT_STRING)
				{
					FreeCopiedString (option_p -> pmo_value.st_string_value_s);
				}

		}		/* for ( ; i > 0; -- i, ++ option_p) */

	FreeMemory (options_p);
}


bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value)
{
	ParameterMultiOption *option_p = (options_p -> pmoa_options_p) + index;
	char *new_description_s = CopyToNewString (description_s, 0, true);
	bool success_flag = false;

	if (new_description_s)
		{
			if (option_p -> pmo_description_s)
				{
					FreeCopiedString (option_p -> pmo_description_s);
				}

			if (options_p -> pmoa_values_type == PT_STRING)
				{
					char *value_s = CopyToNewString (value.st_string_value_s, 0, false);

					if (value_s)
						{
							option_p -> pmo_value.st_string_value_s = value_s;
							success_flag = true;
						}
				}
			else
				{
					option_p -> pmo_value = value;
					success_flag = true;
				}

			if (success_flag)
				{
					option_p -> pmo_description_s = new_description_s;
				}
			else
				{
					FreeCopiedString (new_description_s);
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



int CompareParameterLevels (const ParameterLevel pl0, const ParameterLevel pl1)
{
	return pl0 - pl1;
}


bool SetParameterValue (Parameter * const param_p, const void *value_p)
{
	bool success_flag = false;
	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				{
					bool b = * ((bool *) value_p);
					param_p -> pa_current_value.st_boolean_value = b;
					success_flag = true;
				}
				break;

			case PT_SIGNED_INT:
				{
					int32 i = * ((int32 *) value_p);

					if (param_p -> pa_bounds_p)
						{
							const ParameterBounds * const bounds_p = param_p -> pa_bounds_p;

							if ((i >= bounds_p -> pb_lower.st_long_value) &&
									(i <= bounds_p -> pb_upper.st_long_value))
								{
									param_p -> pa_current_value.st_long_value = i;
									success_flag = true;
								}
						}
					else
						{
							param_p -> pa_current_value.st_long_value = i;
							success_flag = true;
						}
				}
				break;

			case PT_UNSIGNED_INT:
				{
					uint32 i = * ((uint32 *) value_p);

					if (param_p -> pa_bounds_p)
						{
							const ParameterBounds * const bounds_p = param_p -> pa_bounds_p;

							if ((i >= bounds_p -> pb_lower.st_ulong_value) &&
									(i <= bounds_p -> pb_upper.st_ulong_value))
								{
									param_p -> pa_current_value.st_ulong_value = i;
									success_flag = true;
								}
						}
					else
						{
							param_p -> pa_current_value.st_ulong_value = i;
							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					double d = * ((double *) value_p);

					if (param_p -> pa_bounds_p)
						{
							const ParameterBounds * const bounds_p = param_p -> pa_bounds_p;

							if ((d >= bounds_p -> pb_lower.st_data_value) &&
									(d <= bounds_p -> pb_upper.st_data_value))
								{
									param_p -> pa_current_value.st_data_value = d;
									success_flag = true;
								}
						}
					else
						{
							param_p -> pa_current_value.st_long_value = d;
							success_flag = true;
						}
				}
				break;


			case PT_STRING:
			case PT_FILE_TO_WRITE:
			case PT_FILE_TO_READ:
			case PT_DIRECTORY:
				{
					char *value_s = (char *) value_p;

					if (value_s)
						{
							char *copied_value_s = strdup (value_s);

							if (copied_value_s)
								{
									/* If we have a previous value, delete it */
									if (param_p -> pa_current_value.st_string_value_s)
										{
											free (param_p -> pa_current_value.st_string_value_s);
										}

									param_p -> pa_current_value.st_string_value_s = copied_value_s;
									success_flag = true;
								}
						}
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
							if (full_definition_flag)
								{
									if (AddParameterDescriptionToJSON (parameter_p, root_p))
										{
											if (AddParameterTypeToJSON (parameter_p, root_p))
												{
													if (AddDefaultValueToJSON (parameter_p, root_p))
														{
															if (AddParameterOptionsToJSON (parameter_p, root_p))
																{
																	if (AddParameterBoundsToJSON (parameter_p, root_p))
																		{
																			success_flag = true;
																		}
																}
														}
												}
										}									
								}		/* if (full_definition_flag) */
							else
								{
									success_flag = true;
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

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddParameterNameToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = (json_object_set_new (root_p, PARAM_DESCRIPTION_S, json_string (param_p -> pa_description_s)) == 0);

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddParameterDescriptionToJSON - root_p :: ");
	#endif

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

			case PT_SIGNED_INT:
			case PT_UNSIGNED_INT:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("integer")) == 0);
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("number")) == 0);
				break;

			case PT_STRING:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("string")) == 0);
				break;

			case PT_FILE_TO_READ:
				success_flag = (json_object_set_new (root_p, PARAM_TYPE_S, json_string ("string")) == 0);
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */

	if (success_flag)
		{
			success_flag = (json_object_set_new (root_p, PARAM_WHEATIS_TYPE_INFO_S, json_integer (param_p -> pa_type)) == 0);
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddParameterTypeToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p)
{
	return AddValueToJSON (root_p, param_p -> pa_type, & (param_p -> pa_default));
}


static bool AddCurrentValueToJSON (const Parameter * const param_p, json_t *root_p)
{
	return AddValueToJSON (root_p, param_p -> pa_type, & (param_p -> pa_current_value));
}


static bool AddValueToJSON (json_t *root_p, const ParameterType pt, const SharedType *val_p)
{
	bool success_flag = false;
	json_t *value_p = NULL;

	switch (pt)
		{
			case PT_BOOLEAN:
				value_p = (val_p -> st_boolean_value == true) ? json_true () : json_false ();
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
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				value_p = json_string (val_p -> st_string_value_s);
				break;

			default:
				break;
		}		/* switch (pt) */

	if (value_p)
		{
			success_flag = (json_object_set_new (root_p, PARAM_DEFAULT_VALUE_S, value_p) == 0);
		}		/* if (default_value_p) */
	else
		{
			success_flag = true;
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddValueToJSON - root_p :: ");
	#endif

	return success_flag;

}


static bool GetValueFromJSON (const json_t * const root_p, const char *key_s, const ParameterType param_type, SharedType *value_p)
{
	bool success_flag = false;

	/* Set the parameter's value */
	json_t *json_value_p = json_object_get (root_p, key_s);

	if (json_value_p)
		{
			switch (param_type)
				{
					case PT_BOOLEAN:
						if (json_is_boolean (json_value_p))
							{
								value_p -> st_boolean_value = (json_is_true (json_value_p)) ? true : false;
								success_flag = true;
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

					case PT_STRING:
					case PT_FILE_TO_READ:
					case PT_FILE_TO_WRITE:
					case PT_DIRECTORY:
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

					default:
						break;
				}		/* switch (param_p -> pa_type) */

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

									case PT_STRING:
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
											if (json_object_set_new (item_p, SHARED_TYPE_DESCRIPTION_S, json_string (option_p -> pmo_description_s)) == 0)
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
							success_flag = (json_object_set (json_p, PARAM_OPTIONS_S, json_options_p) == 0);
							json_decref (json_options_p);
						}

				}		/* if (json_options_p) */

		}
	else
		{
			/* nothing to do */
			success_flag = true;
		}

	#ifdef _DEBUG
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

	#ifdef _DEBUG
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


static bool GetParameterTypeFromJSON (const json_t * const json_p, ParameterType *param_type_p)
{
	bool success_flag = false;
	json_t *value_p = json_object_get (json_p, PARAM_WHEATIS_TYPE_INFO_S);

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
															else
																{
																	success_flag = false;
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


Parameter *CreateParameterFromJSON (const json_t * const root_p)
{
	Parameter *param_p = NULL;

	const char *name_s = GetStringValue (root_p, PARAM_NAME_S);

	if (name_s)
		{
			const char *description_s = GetStringValue (root_p, PARAM_DESCRIPTION_S);

			if (description_s)
				{
					ParameterType pt;

					if (GetParameterTypeFromJSON (root_p, &pt))
						{
							/*
							 * The default, options and bounds are optional
							 */
							SharedType def;
							ParameterMultiOptionArray *options_p = NULL;
							ParameterBounds *bounds_p = NULL;
							ParameterLevel level = PL_BASIC;

							memset (&def, 0, sizeof (SharedType));
							
							bool default_value_flag = GetValueFromJSON (root_p, PARAM_DEFAULT_VALUE_S, pt, &def);

							if (GetParameterOptionsFromJSON (root_p, &options_p, pt))
								{
									if (GetParameterBoundsFromJSON (root_p, &bounds_p))
										{
											param_p = AllocateParameter (pt, name_s, description_s, options_p, def, bounds_p, level, NULL);
										}
								}

						}		/* if (GetParameterTypeFromJSON (root_p, &pt)) */

				}		/* if (description_s) */

		}		/* if (name_s) */

	return param_p;
}

