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
#ifndef ZIP1_H
#define ZIP1_H


#include "zlib.h"
#include "handler.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

COMPRESS_SERVICE_API int CompressAsZipNoHeader (Handler*in_p, Handler *out_p, int level);

COMPRESS_SERVICE_API int CompressAsZip (Handler*in_p, Handler *out_p, int level);

#endif

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef ZIP1_H */
