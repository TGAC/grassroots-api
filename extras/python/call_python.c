#include <stdio.h>
#include <Python.h>

static int CallPython (int argc, char *argv []);

int main (int argc, char *argv [])
{
  if (argc < 3)
  	{
      fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
      return 1;
  	}
  else
  	{
  		return CallPython (argc, argv);
  	}
}


static int CallPython (int argc, char *argv [])
{
  PyObject *python_name_p, *python_module_p;
  char *value_s = NULL;
	int as_list_flag = 1;
	int result = -1;

  Py_Initialize ();
  python_name_p = PyString_FromString (argv [1]);
  /* Error checking of pName left out */

	value_s = PyString_AsString (python_name_p);
//	printf ("python_name_p: \"%s\"\n", value_s);

	python_module_p = PyImport_Import (python_name_p);
  Py_DECREF (python_name_p);

  if (python_module_p != NULL)
  	{
  		PyObject *python_function_p = PyObject_GetAttrString (python_module_p, argv [2]);
      /* pFunc is a new reference */

      if (python_function_p)
      	{
      		if (PyCallable_Check(python_function_p))
						{
							PyObject *python_args_p = NULL;
							int i;
							const int num_python_args = argc - 3;
							char **python_args_ss = & (argv [3]);

							if (as_list_flag)
								{
									python_args_p = PyList_New (num_python_args);
								}
							else
								{
									python_args_p = PyTuple_New (num_python_args);
								}

							if (python_args_p)
								{
									for (i = 0; i < num_python_args; ++ i, ++ python_args_ss)
										{
											PyObject *python_value_p = PyString_FromString (*python_args_ss);
											int res;

											if (python_value_p)
												{
													/* python_value_p reference stolen here: */
													if (as_list_flag)
														{
															res = PyList_SetItem (python_args_p, i, python_value_p);
														}
													else
														{
															res = PyTuple_SetItem (python_args_p, i, python_value_p);
														}

													if (res != 0)
														{
															fprintf (stderr, "Failed to set item %d to \"%s\"\n", i, *python_args_ss);
															Py_DECREF (python_args_p);
															python_args_p = NULL;
															i = num_python_args;		/* force exit from loop */
														}
												}
											else
												{
													fprintf (stderr, "Cannot convert argument \"%s\"\n", *python_args_ss);
													Py_DECREF (python_args_p);
													python_args_p = NULL;
													i = num_python_args;		/* force exit from loop */
												}

										}		/* for (i = 0; i < num_python_args; ++ i, ++ python_args_ss) */

								}		/* if (python_args_p) */

							if (python_args_p)
								{
									PyObject *python_result_p = NULL;

									if (as_list_flag)
										{
											PyObject *python_args_list_p = Py_BuildValue ("(O)", python_args_p);

											if (python_args_list_p)
												{
													python_result_p = PyObject_CallObject (python_function_p, python_args_list_p);
													Py_DECREF (python_args_list_p);
												}
											else
												{
													PyErr_Print ();
													fprintf (stderr, "Failed to build args list\n");
												}
										}
									else
										{
											python_result_p = PyObject_CallObject (python_function_p, python_args_p);
										}

									Py_DECREF (python_args_p);

									if (python_result_p != NULL)
										{
											value_s = PyString_AsString (python_result_p);
											printf ("Result of call: \n%s\n", value_s);
											Py_DECREF (python_result_p);
											result = 0;
										}
									else
										{
											PyErr_Print ();
											fprintf (stderr,"Call failed\n");
										}

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

          fprintf (stderr, "Cannot find function \"%s\"\n", argv [2]);
      	}

      Py_DECREF(python_module_p);
  	}		/*  if (python_module_p != NULL) */
  else
  	{
      PyErr_Print ();
      fprintf (stderr, "Failed to load \"%s\"\n", argv [1]);
  	}

  Py_Finalize ();

  return result;
}

