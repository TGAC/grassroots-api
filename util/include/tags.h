#ifndef TAGS_H
#define TAGS_H

#include "typedefs.h"

typedef uint32 Tag;


#define TAG_DONE 						(0L)

#define TAG_USER 						(0xFFFF0000)

#define TAG_INPUT_FILE 			(1)
#define TAG_INPUT_DIR 			(2)
#define TAG_OUTPUT_FILE 		(3)
#define TAG_KEYWORD					(4)


#ifndef MAKE_TAG
#define MAKE_TAG(a,b,c,d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))
#endif

#endif		/* #ifndef TAGS_H */
