/**@file filesystem_utils.h
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <time.h>

#include "typedefs.h"
#include "linked_list.h"
#include "wheatis_util_library.h"



typedef struct FileInformation
{
	time_t fi_last_modified;
	size_t fi_size;
} FileInformation;



#ifdef __cplusplus
	extern "C" {
#endif


/** Generic functions */
WHEATIS_UTIL_API char *GetFilenameOnly (const char * const full_path_to_file);
WHEATIS_UTIL_API char *GetPathOnly (const char * const full_path_to_file);
WHEATIS_UTIL_API char *MakeFilename (const char * const path, const char * const filename);

WHEATIS_UTIL_API bool DeterminePathAndFile (const char * const full_path_s, char ** const path_ss, char ** const filename_ss);


WHEATIS_UTIL_API char *SetFileExtension (const char * const filename_s, const char * const extension_s, bool replace_flag);

WHEATIS_UTIL_API void UsePlatformFileSeparator (char *value_s);

/** Platform-specific functions */
WHEATIS_UTIL_API bool IsPathValid (const char * const path);
WHEATIS_UTIL_API bool IsPathAbsolute (const char * const path);
WHEATIS_UTIL_API LinkedList *GetMatchingFiles (const char * const pattern, const bool full_path_flag);

WHEATIS_UTIL_API char GetFileSeparatorChar (void);
WHEATIS_UTIL_API const char *GetCurrentDirectoryString (void);
WHEATIS_UTIL_API const char *GetParentDirectory (void);
WHEATIS_UTIL_API const char *GetPluginPattern (void);

WHEATIS_UTIL_API bool EnsureDirectoryExists (const char * const path_s);
WHEATIS_UTIL_API bool CopyToNewFile (const char * const src_filename, const char * const dest_filename, void (*callback_fn) ());


WHEATIS_UTIL_API bool SetCurrentWorkingDirectory (const char * const path);
WHEATIS_UTIL_API char *GetCurrentWorkingDirectory (void);

/**
 * Check whether the given path refers to a directory.
 *
 * @param path_s The path to check.
 * @return <code>true</code> if the path exists and is a directory, <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool IsDirectory (const char * const path_s);


/**
 * Makes sure that a given directory path exists.
 *
 * @param path_s The path to check. It can be an arbitrary number of levels deep.
 * @return <code>true</code> if the path already exists. If not, the all of directories
 * will attempt to be created. If this succeed then <code>true</code> will be returned, <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool EnsureDirectoryPath (const char * const path_s);


WHEATIS_UTIL_API bool CalculateFileInformation (const char * const path_s, FileInformation *info_p);


WHEATIS_UTIL_API bool RemoveFile (const char * const path_s);


#ifdef __cplusplus
}
#endif

#endif	/* FILESYSTEM_H */
