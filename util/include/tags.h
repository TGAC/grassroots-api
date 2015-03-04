#ifndef TAGS_H
#define TAGS_H

#include "typedefs.h"

typedef uint32 Tag;

#define TAG_DONE 						(0L)

#ifndef MAKE_TAG
#define MAKE_TAG(a,b,c,d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))
#endif

#endif		/* #ifndef TAGS_H */
