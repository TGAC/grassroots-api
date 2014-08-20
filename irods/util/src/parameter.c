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

static bool AddParameterOptionsToJSON (const Parameter * const param_p, json_t *json_p);

static bool AddParameterBoundsToJSON (const Parameter * const param_p, json_t *json_p);



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


bool SetParameter (Parameter * const param_p, const void *value_p)
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


json_t *GetParameterAsJSON (const Parameter * const parameter_p)
{
	json_t *root_p = json_object ();
	
	if (root_p)
		{
			bool success_flag = false;
			
			if (AddParameterNameToJSON (parameter_p, root_p))
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
	bool success_flag = (json_object_set_new (root_p, "name", json_string (param_p -> pa_name_s)) == 0);

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddParameterNameToJSON - root_p :: ");
	#endif
	
	return success_flag;
}


static bool AddParameterDescriptionToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = (json_object_set_new (root_p, "description", json_string (param_p -> pa_name_s)) == 0);

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
				success_flag = (json_object_set_new (root_p, "type", json_string ("boolean")) == 0);
				break;
				
			case PT_SIGNED_INT:
			case PT_UNSIGNED_INT:
				success_flag = (json_object_set_new (root_p, "type", json_string ("integer")) == 0);
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				success_flag = (json_object_set_new (root_p, "type", json_string ("number")) == 0);
				break;

			case PT_STRING:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				success_flag = (json_object_set_new (root_p, "type", json_string ("string")) == 0);
				break;

			case PT_FILE_TO_READ:
				success_flag = (json_object_set_new (root_p, "type", json_string ("string")) == 0);
				break;
				
			default:
				break;
		}		/* switch (param_p -> pa_type) */

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddParameterTypeToJSON - root_p :: ");
	#endif

	return success_flag;
}


static bool AddDefaultValueToJSON (const Parameter * const param_p, json_t *root_p)
{
	bool success_flag = false;
	
	/* Set the parameter's default value */
	json_t *default_value_p = NULL;
	
	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				default_value_p = (param_p -> pa_default.st_boolean_value == true) ? json_true () : json_false ();
				break;
				
			case PT_SIGNED_INT:
				default_value_p = json_integer (param_p -> pa_default.st_long_value);
				break;
				
			case PT_UNSIGNED_INT:
				default_value_p = json_integer (param_p -> pa_default.st_ulong_value);
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				default_value_p = json_real (param_p -> pa_default.st_data_value);
				break;
				
			case PT_STRING:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				default_value_p = json_string (param_p -> pa_default.st_string_value_s);
				break;
				
			default:
				break;
		}		/* switch (param_p -> pa_type) */					

	if (default_value_p)
		{
			success_flag = (json_object_set (root_p, "defaultValue", default_value_p) == 0);
			json_decref (default_value_p);
		}		/* if (default_value_p) */
	else
		{
			success_flag = true;
		}

	#ifdef _DEBUG
	PrintJSON (stderr, root_p, "AddDefaultValueToJSON - root_p :: ");
	#endif

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

									default:
										break;
								}		/* switch (param_p -> pa_type) */					
						
							if (value_s)
								{
									success_flag = (json_array_append_new (json_options_p, json_string (value_s)) == 0); 
									
									if (alloc_flag)
										{
											FreeCopiedString (value_s);
										}
								}						
						}			
					
					if (success_flag)
						{
							success_flag = (json_object_set (json_p, "enum", json_options_p) == 0);
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
					success_flag = (json_array_append (json_p, min_p) == 0);
					json_decref (min_p);					
				}

			if (max_p)
				{
					if (success_flag)
						{
							success_flag = (json_array_append (json_p, max_p) == 0);
						}
					else
						{
							json_array_append (json_p, max_p);
						}
						
					json_decref (max_p);							
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

