/*
 * python_microservices.c
 *
 *  Created on: 24 Mar 2015
 *      Author: tyrrells
 */


#include "python_util.h"
#include "python_micro_services.h"

/*
micro service calls
*/
int ObjectRegistered (msParam_t *user_p, msParam_t *resource_p, msParam_t *collection_p, msParam_t *data_object_p, ruleExecInfo_t *rei)
{
	int result = -1;
	PyObject *module_p = NULL;
	PyObject *function_p = NULL;

	/* The	next line is needed	for	loop back	testing	using	the irule	-i option */
	RE_TEST_MACRO	("				Calling	ObjectRegistered");

	if (InitPython ("irods_python", "ObjectRegistered", &module_p, &function_p, rei) == 0)
		{
			PyObject *params_p = PyDict_New ();

			if (params_p)
				{
					if (AddParamToDictionary (params_p, "user", user_p, rei) == 0)
						{
							if (AddParamToDictionary (params_p, "resource", resource_p, rei) == 0)
								{
									if (AddParamToDictionary (params_p, "collection", collection_p, rei) == 0)
										{
											if (AddParamToDictionary (params_p, "data", data_object_p, rei) == 0)
												{
													PyObject *result_p = PyObject_CallObject (function_p, params_p);

													if (result_p)
														{
															if (PyInt_Check (result_p))
																{
																	result = PyInt_AsLong (result_p);
																}

															Py_DECREF (result_p);
														}		/* if (result_p) */

												}		/* if (AddParamToDictionary (params_p, "user", user_p) == 0) */

										}		/* if (AddParamToDictionary (params_p, "user", user_p) == 0) */

								}		/* if (AddParamToDictionary (params_p, "user", user_p) == 0) */

						}		/* if (AddParamToDictionary (params_p, "user", user_p) == 0) */

					Py_DECREF (params_p);
				}		/* if (params_p) */

			ExitPython (module_p, function_p);
		}		/* if (InitPython ("irods_python", "ObjectRegistered", &module_p, &function_p) == 0) */

	rodsLog (LOG_NOTICE, "ObjectRegistered: result %d\n", result);

	return result;
}
