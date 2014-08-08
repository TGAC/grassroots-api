#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifdef HAVE_STDBOOL_H
	#include <stdbool.h>
#else
	#ifndef HAVE__BOOL
		#ifdef __cplusplus
			typedef bool _Bool;
		#else
			#define _Bool signed char
		#endif
	#endif
	#define bool _Bool
	#define false (0)
	#define true (1)
	#define __bool_true_false_are_defined 1
#endif
     

/* the datatypes that we will use */
#ifdef UNIX
	#include <inttypes.h>

	typedef uint32_t uint32;
	typedef int32_t int32;
	typedef double double64;
	typedef uint8_t uint8;
	typedef int8_t int8;
	typedef int16_t int16;
	typedef uint16_t uint16;


	#define UINT32_FMT_IDENT "u"
	#define INT32_FMT_IDENT "d"
	#define DOUBLE64_FMT_IDENT "lf"
	#define UINT8_FMT_IDENT "cu"
	#define INT8_FMT_IDENT "c"
	#define UINT16_FMT_IDENT "hu"
	#define INT16_FMT_IDENT "hd"
#endif
	   
   
     
/**
 * An enum specifying whether a file is located on a 
 * local or remote filesystem.
 */
typedef enum FileLocation
{
	/** The file is on a local filesystem */
	FILE_LOCATION_LOCAL,

	/** The file is on a remote filesystem */
	FILE_LOCATION_REMOTE,

	/** The file location is unknown */
	FILE_LOCATION_UNKNOWN
} FileLocation;


/**
 * An enum specifying the particular status of a piece of dynamically allocated memory for
 * a particular object. Its use is to allow many objects to have a pointer to some memory
 * but only one takes care of deallocating it.
 */
typedef enum MEM_FLAG
{
	/** The memory has already been freed so no action need be taken. */
	MF_ALREADY_FREED,

	/** Make a separate copy of the object pointed at by the source pointer.  */
	MF_DEEP_COPY,

	/** Use the pointer value. We will call free on this used value. Nothing else may free this pointer. */
	MF_SHALLOW_COPY,

	/** Use the pointer value. We will not call free on this used value, another module must free the memory. */
	MF_SHADOW_USE
} MEM_FLAG;


#endif		/* #ifndef TYPEDEFS_H */
