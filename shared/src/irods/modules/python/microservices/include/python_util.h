/*
** Copyright 2014-2016 The Earlham Institute
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

/**
 * @file
 * @brief
 */
/*
 * python_util.h
 *
 *  Created on: 25 Mar 2015
 *      Author: billy
 */

#ifndef PYTHON_UTIL_H_
#define PYTHON_UTIL_H_

#include <Python.h>

#include "reDataObjOpr.h"


#ifdef __cplusplus
extern "C"
{
#endif

int InitPython (const char * const module_name_s, const char * const function_name_s, PyObject **module_pp, PyObject **function_pp, ruleExecInfo_t *rei_p);

void ExitPython (PyObject *module_p, PyObject *function_p);

int AddParamToDictionary (PyObject *dict_p, const char * const key_s, msParam_t * const value_p, ruleExecInfo_t *rei_p);

#ifdef __cplusplus
}
#endif

#endif /* PYTHON_UTIL_H_ */
