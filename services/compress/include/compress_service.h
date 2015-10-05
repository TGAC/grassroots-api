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
#ifndef COMPRESS_SERVICE_H
#define COMPRESS_SERVICE_H

#include "service.h"
#include "library.h"
#include "compress_service_library.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C"
{
#endif


COMPRESS_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


COMPRESS_SERVICE_API void ReleaseServices (ServicesArray *service_p);


COMPRESS_SERVICE_LOCAL bool CompressData (z_stream *strm_p, Bytef **output_buffer_pp, size_t *output_buffer_size_p, const int flush);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef COMPRESS_SERVICE_H */
