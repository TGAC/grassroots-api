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
#ifndef TAGS_H
#define TAGS_H

#include "typedefs.h"

typedef uint32 Tag;

#define TAG_DONE 						(0L)

#ifndef MAKE_TAG
#define MAKE_TAG(a,b,c,d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))
#endif

#endif		/* #ifndef TAGS_H */
