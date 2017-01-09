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

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
		#ifndef HAVE__BOOL
			#ifdef __cplusplus
				typedef bool _Bool;
			#else
				#define _Bool signed char
			#endif
		#endif
		#define bool _Bool
	#endif		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

	/** The value to use to signify false for bool datatypes. */
	#define false (0)

	/** The value to use to signify true for bool datatypes. */
	#define true (1)

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
		#define __bool_true_false_are_defined 1
	#endif

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

	/** The C I/O length field for uint32 datatypes. */
	#define UINT32_FMT_IDENT "u"

	/** The C I/O length field for int32 datatypes. */
	#define INT32_FMT_IDENT "d"

	/** The C I/O length field for double64 datatypes. */
	#define DOUBLE64_FMT_IDENT "lf"

	/** The C I/O length field for uint8 datatypes. */
	#define UINT8_FMT_IDENT "cu"

	/** The C I/O length field for int8 datatypes. */
	#define INT8_FMT_IDENT "c"

	/** The C I/O length field for uint16 datatypes. */
	#define UINT16_FMT_IDENT "hu"

	/** The C I/O length field for int16 datatypes. */
	#define INT16_FMT_IDENT "hd"

	/** The C I/O length field for sizet datatypes. */
	#define SIZET_FMT_IDENT "lu"
#endif

/** The C I/O format specifier for uint32 datatypes. */
#define UINT32_FMT "%" UINT32_FMT_IDENT

/** The C I/O format specifier for int32 datatypes. */
#define INT32_FMT "%" INT32_FMT_IDENT

/** The C I/O format specifier for double64 datatypes. */
#define DOUBLE64_FMT "%" DOUBLE64_FMT_IDENT

/** The C I/O format specifier for uint8 datatypes. */
#define UINT8_FMT "%" UINT8_FMT_IDENT

/** The C I/O format specifier for int8 datatypes. */
#define INT8_FMT "%" INT8_FMT_IDENT

/** The C I/O format specifier for sizet datatypes. */
#define SIZET_FMT "%" SIZET_FMT_IDENT




/**
 * An enum specifying whether a file is located on a
 * local or remote filesystem.
 * @ingroup utility_group
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


/*
 * These next definitions are applying the technique in the fantastic article on how
 * to suppress unused parameter warnings by Julio Merino at
 * http://julipedia.meroh.net/2015/02/unused-parameters-in-c-and-c.html.
 * Thanks Julio!
 */
#ifndef UNUSED_PARAM

#ifdef __GNUC__
	#define ATTR_UNUSED __attribute__((unused))
#else
	#define ATTR_UNUSED(x) x
#endif

#define UNUSED_PARAM(name) unused_ ## name ATTR_UNUSED

#endif		/* #ifndef UNUSED_PARAM */

#endif		/* #ifndef TYPEDEFS_H */
