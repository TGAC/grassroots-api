/*
 * typedefs.h
 *
 *  Created on: 7 Jun 2015
 *      Author: billy
 */
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifdef HAVE_STDBOOL_H
	#include <stdbool.h>
#elif !defined __bool_true_false_are_defined
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
	typedef uint64_t uint64;
	typedef int64_t int64;
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
	#define SIZET_FMT_IDENT "lu"
#endif


#define UINT32_FMT "%" UINT32_FMT_IDENT
#define INT32_FMT "%" INT32_FMT_IDENT
#define DOUBLE64_FMT "%" DOUBLE64_FMT_IDENT
#define UINT8_FMT "%" UINT8_FMT_IDENT
#define INT8_FMT "%" INT8_FMT_IDENT
#define SIZET_FMT "%" SIZET_FMT_IDENT




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


	FILE_LOCATION_IRODS,

	/** The file location is unknown */
	FILE_LOCATION_UNKNOWN
} FileLocation;


#endif		/* #ifndef TYPEDEFS_H */
