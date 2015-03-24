/*
 * python_call.c
 *
 *  Created on: 24 Mar 2015
 *      Author: tyrrells
 */


#include <Python.h>

#include "reDataObjOpr.h"




/*
micro service call
*/
int Fire (msParam_t *collection_p, msParam_t *out_p, ruleExecInfo_t *rei)
{
	int result = 0;

	/* The	next line is needed	for	loop back	testing	using	the irule	-i option */
	RE_TEST_MACRO	("				Calling	PythonTool");

	/* Get the input parameters */
	if (strcmp (collection_p -> type, STR_MS_T) == 0)
		{
			char *collection_s = (char *) (collection_p -> inOutStruct);

			if (collection_s)
				{
					blast_p = CreateBlastTool ();

					if (blast_p)
						{
							bool success_flag = false;
							msParamArray_t params;
							memset (&params, 0, sizeof (msParamArray_t));

							/* is it a list of key-value pairs? */
							if (strstr (input_param_s, MS_INP_SEP_STR))
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: \"%s\" contains %s", __FILE__, __LINE__, input_param_s, MS_INP_SEP_STR);
									success_flag = ConvertKeyValueArgument (blast_p, in_p);
								}
							else
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: \"%s\" does not contain %s", __FILE__, __LINE__, input_param_s, MS_INP_SEP_STR);

									/* it appears to be a single argument so assume it is the
									 * input filename.
									 */
									addMsParamToArray (&params, in_p -> label, STR_MS_T, input_param_s, NULL, 1);

									success_flag = ConvertArgumentsArray (blast_p, &params);
								}


							if (success_flag)
								{
									bool success_flag = RunBlast (blast_p);

									if (success_flag)
										{

										}
								}
							else
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: Failed to convert input parameter \"%s\"", __FILE__, __LINE__, input_param_s);
								}


							FreeBlastTool (blast_p);
						}		/* if (blast_p) */
					else
						{

						}


				}		/* if (input_param_s) */

		}		/* if (strcmp (in_p -> type, STR_MS_T) == 0) */

	return result;
}



bool ConvertArgumentsArray (BlastTool *tool_p, msParamArray_t *params_p)
{
	bool success_flag = false;

	if (params_p)
		{
			if (tool_p)
				{
					int i;
					msParam_t **param_pp = params_p -> msParam;

					success_flag = true;

					for (i = params_p -> len; i > 0; -- i, ++ param_pp)
						{
							const msParam_t *param_p = *param_pp;

							if (param_p -> label)
								{
									size_t size = 2 + strlen (param_p -> label);

									char *key_s = (char *) malloc (size * sizeof (char));

									if (key_s)
										{
											*key_s = '-';
											strcpy (key_s + 1, param_p -> label);


											if (strcmp (param_p -> type, STR_MS_T) == 0)
												{
													tool_p -> AddArgument (key_s, true);
													tool_p -> AddArgument ((char *) (param_p -> inOutStruct), false);
												}
											else if (strcmp (param_p -> type, INT_MS_T) == 0)
												{
													char *value_s = GetIntAsString (* ((int *) (param_p -> inOutStruct)));

													if (value_s)
														{
															tool_p -> AddArgument (key_s, true);
															tool_p -> AddArgument (value_s, true);
														}
													else
														{
															/* not enough memory */
															free (key_s);
															success_flag = false;

															/* force exit from loop */
															i = -1;
														}

												}
											else if (strcmp (param_p -> type, BOOL_MS_T) == 0)
												{
													tool_p -> AddArgument (key_s, true);
												}

										}		/* if (key_s) */
								}

						}		/* for (i = params_p -> len; i > 0; -- i, ++ param_pp) */

				}		/* if (tool_p) */

		}		/* if (params_p) */


	return success_flag;
}

static int CallPython (const char * const module_name_s, const char * const function_name_s, const char *value_s, ...)
{
  PyObject *python_name_p, *python_module_p;
  char *value_s = NULL;
	int result = 0;

  Py_Initialize ();

  /* Convert the module name from C to Python */
  /* Error checking of python_name_p left out */
  python_name_p = PyString_FromString (module_name_s);
	value_s = PyString_AsString (python_name_p);
//	printf ("python_name_p: \"%s\"\n", value_s);
	python_module_p = PyImport_Import (python_name_p);
  Py_DECREF (python_name_p);

  if (python_module_p != NULL)
  	{
  		/* Get the Python function that we are going to call */
  		PyObject *python_function_p = PyObject_GetAttrString (python_module_p, function_name_s);

      if (python_function_p)
      	{
      		if (PyCallable_Check (python_function_p))
						{
							PyObject *python_args_p = PyDict_New ();

							if (python_args_p)
								{
									int i;
									int success_flag = 1;
									int loop_flag = 1;
									va_list args;
									va_start (args, value_s);
									const char *arg_s = value_s;

									/*
									 * Loop through all of the arguments pushed onto the stack in
									 * key-value pairs and a terminating NULL
									 */
									while (success_flag && loop_flag)
										{
											if (arg_s)
												{
													const char *key_s = arg_s;

													arg_s = va_arg (args, char *);

													if (arg_s)
														{
															PyObject *python_value_p = PyString_FromString (arg_s);

															if (python_value_p)
																{
																	if (PyDict_SetItemString (python_args_p, key_s, python_value_p) == 0)
																		{
																			arg_s = va_arg (args, char *);
																		}
																	else
																		{
																			success_flag = 0;
																		}
																}		/* if (python_value_p) */
															else
																{
																	success_flag = 0;
																}

														}		/* if (arg_s) */
													else
														{
															success_flag = 0;
														}
												}		/* if (arg_s) */
											else
												{
													loop_flag = 0;
												}

										}		/* while (success_flag && loop_flag) */

									va_end (args);

									/* Did we get all of the arguments successfully? */
									if (success_flag)
										{
											PyObject *python_result_p = PyObject_CallObject (python_function_p, python_args_p);

											if (python_result_p)
												{

													Py_DECREF (python_result_p);
													python_result_p = NULL;
												}		/* if (python_result_p) */

										}		/* if (success_flag) */

									Py_DECREF (python_args_p);
									python_args_p = NULL;

								}		/* if (python_args_p) */

						}		/* if (PyCallable_Check(python_function_p)) */

					Py_DECREF (python_function_p);
      	}		/* if (python_function_p) */
      else
      	{
          if (PyErr_Occurred ())
          	{
              PyErr_Print ();
          	}

          fprintf (stderr, "Cannot find function \"%s\"\n", function_name_s);
      	}

      Py_DECREF(python_module_p);
  	}		/*  if (python_module_p != NULL) */
  else
  	{
      PyErr_Print ();
      fprintf (stderr, "Failed to load \"%s\"\n", module_name_s);
  	}

  Py_Finalize ();

  return result;
}


