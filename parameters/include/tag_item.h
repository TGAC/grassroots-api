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
/**
 * @file
 * @addtogroup Parameters
 * @{
 */
#ifndef TAG_ITEM_H
#define TAG_ITEM_H

#include "typedefs.h"
#include "parameter.h"
#include "tags.h"

#include "grassroots_params_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Find the matching Tag within a TagItem.
 *
 * @param tags_p The TagItem to search.
 * @param tag The Tag to search for.
 * @return The matching TagItem or <code>NULL</code> if it could not be found.
 * @memberof TagItem
 */
GRASSROOTS_PARAMS_API TagItem *FindMatchingTag (TagItem *tags_p, Tag tag);


/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef TAG_ITEM_H */
