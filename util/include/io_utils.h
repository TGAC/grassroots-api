/**
 * @file io_utils.h
 * @defgroup io_util Miscellaneous I/O routines
 * 
 * 
 * @brief Various functions to do stuff
 * @details More details here
 */


#ifndef IO_UTILS_H
#define IO_UTILS_H


#include <stdio.h>

/* 
 * irods includes
 */
#include "putUtil.h"


 /*
  * wheatis includes
  */
#include "typedefs.h"
#include "wheatis_util_library.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Determine whether a file is on the local filesystem or not.
 * 
 * @param filename_s The file to check.
 * @return The type of location of the file or FILE_LOCATION_UNKNOWN if it could
 * not be determined.
 * @ingroup io_util
 */
WHEATIS_UTIL_API FileLocation GetFileLocation (const char * const filename_s);


/**
 * Copy a file to the local filesystem.
 * 
 * @param filename_s The name of the the source file.
 * @return true on success, false on error with errno set to the appropriate value. 
 * @ingroup io_util 
 */ 
WHEATIS_UTIL_API char *CopyFileToLocalFilesystem (const char * const filename_s);


/**
 * Copy a file chunk to the local filesystem.
 * 
 * @param filename_s The name of the the source file.
 * @param offset The byte offset to start copying from.
 * @param chunk_size The size of the chunk to copy.
 * @return true on success, false on error with errno set to the appropriate value. 
 * @ingroup io_util 
 */ 
WHEATIS_UTIL_API char *CopyFileChunkToLocalFilesystem (const char * const filename_s, size_t offset, size_t chunk_size);




/**
 * Copy the contents of one named file to another.
 * 
 * @param src_filename_s The name of the the source file.
 * @param dest_filename_s The name of the the source file.
 * @param callback_fn The callback_fn to denote progress (currently unused).
 * @return true on success, false on error with errno set to the appropriate value.
 * @ingroup io_util 
 */ 
WHEATIS_UTIL_API bool CopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) ());



/**
 * Get a newly allocated char * with the given value.
 * 
 * @param value The value to print into the the newly allocated string.
 * @return The string with the given value or <code>NULL</code> upon error.
 * @ingroup io_util 
 */
WHEATIS_UTIL_API char *GetIntAsString (int value);


/**
 * Write a logging message to the given logfile.
 * 
 * @param log_ident_s The logfile identifer to use. If this is NULL then "slog" will be used.
 * @param log_level One of the levels defined in syslog.h:
 * 
 * - LOG_EMERG: The message says the system is unusable.
 * - LOG_ALERT: Action on the message must be taken immediately.
 * - LOG_CRIT: The message states a critical condition.
 * - LOG_ERR: The message describes an error.
 * - LOG_WARNING: The message is a warning.
 * - LOG_NOTICE: The message describes a normal but important event.
 * - LOG_INFO: The message is purely informational.
 * - LOG_DEBUG
 * 
 * @param message_s The string to write using standard c printf syntax.
 * @ingroup io_util
 */
WHEATIS_UTIL_API void WriteToLog (const char *log_ident_s, const int log_level, const char *message_s, ...);



WHEATIS_UTIL_API int PutAndCheckForServices (rcComm_t **connection_pp, rodsEnv *env_p, rodsArguments_t *args_p, rodsPathInp_t *path_inp_p);


WHEATIS_UTIL_LOCAL int PrintRodsPath (FILE *out_f, const rodsPath_t * const rods_path_p, const char * const description_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IO_UTILS_H */
