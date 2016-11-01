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
#ifndef PLATFORM_H
#define PLATFORM_H

#include "typedefs.h"
#include "grassroots_util_library.h"

#ifdef __cplusplus
	extern "C" {
#endif


GRASSROOTS_UTIL_API bool InitPlatform (void);


GRASSROOTS_UTIL_API void ExitPlatform (void);


GRASSROOTS_UTIL_API uint32 Snooze (uint32 milliseconds);


#ifdef __cplusplus
}
#endif



#endif



