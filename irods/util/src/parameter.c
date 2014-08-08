#include <math.h>
#include <stddef.h>
#include <string.h>

#include "math_utils.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "string_utils.h"


static ParameterMultiOptionArray *AllocateEmptyParameterMultiOptionArray (const uint32 num_options);


Parameter *AllocateParameter (ParameterType type, const char * const name_s, const char * const key_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	char *new_name_s = CopyToNewString (name_s, 0, TRUE);

	if (new_name_s)
		{
			char *new_key_s = CopyToNewString (key_s, 0, TRUE);

			if (new_key_s)
				{
					char *new_description_s = CopyToNewString (description_s, 0, TRUE);

					if (new_description_s)
						{
							Parameter *param_p = (Parameter *) AllocMemory (sizeof (Parameter));

							if (param_p)
								{
									param_p -> pa_type = type;
									param_p -> pa_name_s = new_name_s;
									param_p -> pa_key_s = new_key_s;
									param_p -> pa_description_s = new_description_s;
									param_p -> pa_options_p = options_p;
									param_p -> pa_check_value_fn = check_value_fn;
									param_p -> pa_default = default_value;
									param_p -> pa_bounds_p = bounds_p;
									param_p -> pa_level = level;

									return param_p;
								}		/* if (param_p) */

							FreeCopiedString (new_description_s);
						}		/* if (new_description_s) */

					FreeCopiedString (new_key_s);
				}		/* if (new_key_s) */

			FreeCopiedString (new_name_s);
		}		/* if (new_name_s) */

	return NULL;
}


void FreeParameter (Parameter *param_p)
{
	if (param_p -> pa_key_s)
		{
			FreeCopiedString (param_p -> pa_key_s);
		}

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


ParameterNode *AllocateParameterNode (Parameter *parameter_p)
{
	ParameterNode *node_p = (ParameterNode *) AllocMemory (sizeof (ParameterNode));

	if (node_p)
		{
			node_p -> pn_parameter_p = parameter_p;

			return node_p;
		}		/* if (node_p) */

	return NULL;
}


void FreeParameterNode (ListNode * const node_p)
{
	ParameterNode *param_node_p = (ParameterNode *) node_p;

	//FreeParameter (param_node_p -> pn_parameter_p);
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
					case PT_STRING:
							{
								bounds_p -> pb_lower.st_string_value_s = CopyToNewString (src_p -> pb_lower.st_string_value_s, 0, FALSE);
								bounds_p -> pb_upper.st_string_value_s = CopyToNewString (src_p -> pb_upper.st_string_value_s, 0, FALSE);

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


ParameterNode *GetParameterNode (ParameterType type, const char * const name_s, const char * const key_s, const char * const description_s, ParameterMultiOptionArray *options_p, SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p))
{
	Parameter *param_p = AllocateParameter (type, name_s, key_s, description_s, options_p, default_value, bounds_p, level, check_value_fn);

	if (param_p)
		{
			ParameterNode *node_p = AllocateParameterNode (param_p);

			if (node_p)
				{
					return node_p;
				}		/* if (node_p) */

			FreeParameter (param_p);
		}		/* if (param_p) */

	return NULL;
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


BOOLEAN SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 index, const char * const description_s, SharedType value)
{
	ParameterMultiOption *option_p = (options_p -> pmoa_options_p) + index;
	char *new_description_s = CopyToNewString (description_s, 0, TRUE);
	BOOLEAN success_flag = FALSE;

	if (new_description_s)
		{
			if (option_p -> pmo_description_s)
				{
					FreeCopiedString (option_p -> pmo_description_s);
				}

			if (options_p -> pmoa_values_type == PT_STRING)
				{
					char *value_s = CopyToNewString (value.st_string_value_s, 0, FALSE);

					if (value_s)
						{
							option_p -> pmo_value.st_string_value_s = value_s;
							success_flag = TRUE;
						}
				}
			else
				{
					option_p -> pmo_value = value;
					success_flag = TRUE;
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


void PrintParameter (const Parameter * const param_p, OutputStream * const stream_p)
{
	char *type_s = NULL;
	char *default_s = "";
	char *level_s = NULL;
	char *lower_bound_s = NULL;
	char *upper_bound_s = NULL;

	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				type_s = "Boolean";
				default_s = (param_p -> pa_default.st_boolean_value)	? "TRUE" : "FALSE";

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = (param_p -> pa_bounds_p -> pb_lower.st_boolean_value)	? "TRUE" : "FALSE";
						upper_bound_s = (param_p -> pa_bounds_p -> pb_upper.st_boolean_value)	? "TRUE" : "FALSE";
					}

				break;

			case PT_SIGNED_INT:
				type_s = "Integer";
				default_s = ConvertIntegerToString (param_p -> pa_default.st_long_value);

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = ConvertIntegerToString (param_p -> pa_bounds_p -> pb_lower.st_long_value);
						upper_bound_s = ConvertIntegerToString (param_p -> pa_bounds_p -> pb_upper.st_long_value);
					}

				break;

			case PT_UNSIGNED_INT:
				type_s = "Non-negative integer";
				default_s = ConvertIntegerToString (param_p -> pa_default.st_ulong_value);

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = ConvertIntegerToString (param_p -> pa_bounds_p -> pb_lower.st_ulong_value);
						upper_bound_s = ConvertIntegerToString (param_p -> pa_bounds_p -> pb_upper.st_ulong_value);
					}

				break;

			case PT_SIGNED_REAL:
				type_s = "Number";
				default_s = ConvertDoubleToString (param_p -> pa_default.st_data_value);

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = ConvertDoubleToString (param_p -> pa_bounds_p -> pb_lower.st_data_value);
						upper_bound_s = ConvertDoubleToString (param_p -> pa_bounds_p -> pb_upper.st_data_value);
					}
				break;

			case PT_UNSIGNED_REAL:
				type_s = "Non-negative number";
				default_s = ConvertDoubleToString (param_p -> pa_default.st_data_value);

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = ConvertDoubleToString (param_p -> pa_bounds_p -> pb_lower.st_data_value);
						upper_bound_s = ConvertDoubleToString (param_p -> pa_bounds_p -> pb_upper.st_data_value);
					}
				break;

			case PT_STRING:
				type_s = "Text";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}

				break;

			case PT_FILE_TO_WRITE:
				type_s = "Filename to save to";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}
				break;

			case PT_FILE_TO_READ:
				type_s = "Filename to load from";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}
				break;

			case PT_DIRECTORY:
				type_s = "Directory";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}
				break;

			case PT_ATOM_IDS:
				type_s = "List of atom ids";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}
				break;

			case PT_DIHEDRAL_IDS:
				type_s = "List of dihedral ids";
				default_s = param_p -> pa_default.st_string_value_s;

				if (param_p -> pa_bounds_p)
					{
						lower_bound_s = param_p -> pa_bounds_p -> pb_lower.st_string_value_s;
						upper_bound_s = param_p -> pa_bounds_p -> pb_upper.st_string_value_s;
					}
				break;

			default:
				break;
		}

	switch (param_p -> pa_level)
		{
			case PL_BASIC:
				level_s = "Basic";
				break;

			case PL_INTERMEDIATE:
				level_s = "Intermediate";
				break;

			case PL_ADVANCED:
				level_s = "Advanced";
				break;

			default:
				break;
		}

	if (type_s && level_s)
		{
			PrintToOutputStream (stream_p, "NAME: \"%s\" ", param_p -> pa_name_s);
			PrintToOutputStream (stream_p, "KEY: \"%s\" ", param_p -> pa_key_s);
			PrintToOutputStream (stream_p, "TYPE: \"%s\" ", type_s);
			PrintToOutputStream (stream_p, "DESCRIPTION: \"%s\" ", param_p -> pa_description_s);
			PrintToOutputStream (stream_p, "DEFAULT: \"%s\" ", default_s);
			PrintToOutputStream (stream_p, "LEVEL: \"%s\" ", level_s);

			if (lower_bound_s && upper_bound_s)
				{
					PrintToOutputStream (stream_p, "LOWER: \"%s\" ", lower_bound_s);
					PrintToOutputStream (stream_p, "UPPER: \"%s\" ", upper_bound_s);
				}		/* if (lower_bound_s && upper_bound_s) */

			if (param_p -> pa_options_p)
				{
					const uint32 num_options = param_p -> pa_options_p -> pmoa_num_options;
					const ParameterMultiOption *option_p = param_p -> pa_options_p -> pmoa_options_p;
					const ParameterType param_type = param_p -> pa_type;
					uint32 i;
					char *option_value_s = NULL;
					BOOLEAN copied_option_flag = FALSE;

					PrintToOutputStream (stream_p, "OPTIONS: { ");
					for (i = 0; i < num_options; ++ i, ++ option_p)
						{
							PrintToOutputStream (stream_p, "\"%s\"", option_p -> pmo_description_s);

							switch (param_type)
								{
									case PT_BOOLEAN:
										option_value_s = (option_p -> pmo_value.st_boolean_value) ? "TRUE" : "FALSE";
										break;

									case PT_SIGNED_INT:
										option_value_s = ConvertIntegerToString (option_p -> pmo_value.st_long_value);
										copied_option_flag = TRUE;
										break;

									case PT_UNSIGNED_INT:
										option_value_s = ConvertIntegerToString (option_p -> pmo_value.st_ulong_value);
										copied_option_flag = TRUE;
										break;

									case PT_SIGNED_REAL:
										option_value_s = ConvertDoubleToString (option_p -> pmo_value.st_data_value);
										copied_option_flag = TRUE;
										break;

									case PT_UNSIGNED_REAL:
										option_value_s = ConvertDoubleToString (option_p -> pmo_value.st_data_value);
										copied_option_flag = TRUE;
										break;

									case PT_STRING:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									case PT_FILE_TO_WRITE:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									case PT_FILE_TO_READ:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									case PT_DIRECTORY:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									case PT_ATOM_IDS:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									case PT_DIHEDRAL_IDS:
										option_value_s = option_p -> pmo_value.st_string_value_s;
										break;

									default:
										break;
								}		/* switch (param_type) */

							if (option_value_s)
								{
									PrintToOutputStream (stream_p, ":\"%s\" ", option_value_s);

									if (copied_option_flag)
										{
											FreeCopiedString (option_value_s);
										}
								}

						}		/* for (i = 0; i < num_options; ++ i, ++ option_p) */

					PrintToOutputStream (stream_p, "}");
				}		/* if (pa_options_p) */

		}		/* if (type_s && level_s && default_s) */

}


