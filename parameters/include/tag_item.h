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

#include "wheatis_params_library.h"

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
WHEATIS_PARAMS_API TagItem *FindMatchingTag (TagItem *tags_p, Tag tag);


/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef TAG_ITEM_H */
