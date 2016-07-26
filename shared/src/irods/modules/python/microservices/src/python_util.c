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
/*
 * python_call.c
 *
 *  Created on: 24 Mar 2015
 *      Author: tyrrells
 */


#include <Python.h>

#include "python_util.h"
#include "reDataObjOpr.h"


/*
**	STATIC PROTOTYPES
*/

static PyObject *GetModule (const char *module_name_s, ruleExecInfo_t *rei_p);

static PyObject *GetFunction (const char *function_name_s, PyObject *module_p, ruleExecInfo_t *rei_p);


/*
 *	API FUNCTIONS
 */

int InitPython (const char * const module_name_s, const char * const function_name_s, PyObject **module_pp, PyObject **function_pp, ruleExecInfo_t *rei_p)
{
	int result = -1;
	PyObject *module_p = NULL;

	Py_Initialize ();

	module_p = GetModule (module_name_s, rei_p);
	if (module_p)
		{
			PyObject *function_p = GetFunction (function_name_s, module_p, rei_p);

			if (function_p)
				{
					*module_pp = module_p;
					*function_pp = function_p;

					result = 0;
				}
			else
				{
					Py_DECREF (module_p);
				}

		}		/* if (module_p) */

	if (result != 0)
		{
			Py_Finalize ();
		}

	return result;
}


void ExitPython (PyObject *module_p, PyObject *function_p)
{
	Py_DECREF (module_p);
	Py_DECREF (function_p);

	Py_Finalize ();
}


int AddParamToDictionary (PyObject *dict_p, const char * const key_s, msParam_t * const value_p, ruleExecInfo_t *rei_p)
{
	int result = -1;
	char *value_s = NULL;

	if (strcmp (value_p -> type, STR_MS_T) == 0)
		{
			value_s = (char *) (value_p -> inOutStruct);
		}

	if (value_s)
		{
			PyObject *python_value_p = PyString_FromString (value_s);

			if (python_value_p)
				{
					if (PyDict_SetItemString (dict_p, key_s, python_value_p) == 0)
						{
							result = 0;
						}
					else
						{
							Py_DECREF (python_value_p);

							if (rei_p -> rsComm)
								{
									rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "AddParamToDictionary: Could not set python value for %s=%s\n", key_s, value_s);
								}
							else
								{
									rodsLog (LOG_ERROR, "AddParamToDictionary: Could not set python value for %s=%s\n", key_s, value_s);
								}

						}

				}		/* if (python_value_p) */
			else
				{
					if (rei_p -> rsComm)
						{
							rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "AddParamToDictionary: Could not create python value from %s\n", value_s);
						}
					else
						{
							rodsLog (LOG_ERROR, "AddParamToDictionary: Could not create python value from %s\n", value_s);
						}
				}

		}		/* if (value_s) */
	else
		{
			if (rei_p -> rsComm)
				{
					rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "AddParamToDictionary: Could not get irods param value for key %s\n", key_s);
				}
			else
				{
					rodsLog (LOG_ERROR, "AddParamToDictionary: Could not get irods param value for key %s\n", key_s);
				}
		}

	return result;
}


/*
 * STATIC FUNCTIONS
 */

static PyObject *GetModule (const char *module_name_s, ruleExecInfo_t *rei_p)
{
	PyObject *python_module_p = NULL;

	/* Convert the module name from C to Python */
	/* Error checking of python_name_p left out */
  PyObject *python_name_p = PyString_FromString (module_name_s);

	if (python_name_p)
		{
			python_module_p = PyImport_Import (python_name_p);

		  Py_DECREF (python_name_p);
		}
	else
		{
			if (rei_p -> rsComm)
				{
					rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "GetModule: Module %s is not available\n", module_name_s);
				}
			else
				{
					rodsLog (LOG_ERROR, "GetModule: Module %s is not available\n", module_name_s);
				}
		}

	return python_module_p;
}


static PyObject *GetFunction (const char *function_name_s, PyObject *module_p, ruleExecInfo_t *rei_p)
{
	/* Get the Python function that we are going to call */
	PyObject *python_function_p = PyObject_GetAttrString (module_p, function_name_s);

	if (python_function_p)
		{
			if (PyCallable_Check (python_function_p))
				{
					return python_function_p;
				}
			else
				{
					if (rei_p -> rsComm)
						{
							rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "GetFunction: Function %s is not callable\n", function_name_s);
						}
					else
						{
							rodsLog (LOG_ERROR, "GetFunction: Failed to get function %s from module\n", function_name_s);
						}
				}

			Py_DECREF (python_function_p);
		}
	else
		{
			if (rei_p -> rsComm)
				{
					rodsLogAndErrorMsg (LOG_ERROR, & (rei_p -> rsComm -> rError), rei_p -> status, "GetFunction: Failed to get function %s from module\n", function_name_s);
				}
			else
				{
					rodsLog (LOG_ERROR, "GetFunction: Failed to get function %s from module\n", function_name_s);
				}
		}

	return NULL;
}

