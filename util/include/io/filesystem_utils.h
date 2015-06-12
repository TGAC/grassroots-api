/**@file filesystem_utils.h
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <time.h>

#include "typedefs.h"
#include "linked_list.h"
#include "wheatis_util_library.h"


/**
 * A datatype to hold a collection of metadata about a file.

 */
typedef struct FileInformation
{
	/** The timestamp of the last modification to a file */
	time_t fi_last_modified;

	/** The size of the file in bytes */
	size_t fi_size;
} FileInformation;



#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Get the filename only from a given path.
 *
 * @param full_path_to_file_s The full path.
 * @return A newly-allocated string containing the filename only. This will
 * need to be freed with FreeCopiedString to avoid a memory leak. If there was an
 * error, then this will be <code>NULL</code>.
 */
WHEATIS_UTIL_API char *GetFilenameOnly (const char * const full_path_to_file_s);


/**
 * Get the path to the parent directory only from a given path.
 *
 * @param full_path_to_file_s The full path.
 * @return A newly-allocated string containing the full path to the parent
 * directory only. This will need to be freed with FreeCopiedString to
 * avoid a memory leak. If there was an error, then this will be <code>NULL</code>.
 */
WHEATIS_UTIL_API char *GetPathOnly (const char * const full_path_to_file_s);


/**
 * @brief Construct a filename
 *
 * This will concatenate the path and filename together inserting a platform-specific
 * file separator if needed.
 *
 * @param path_s The path to append the filename to.
 * @param filename_s The filename to append.
 * @return A newly-allocated string containing the concatenated value which will
 * need to be freed with FreeCopiedString to avoid a memory leak. If there was an
 * error, then this will be <code>NULL</code>.
 */
WHEATIS_UTIL_API char *MakeFilename (const char * const path_s, const char * const filename_s);



/**
 * @brief Extract the parent path and filename only from a given path
 *
 * This will extract out the parts of path. For example this would split "foo/bar/my_file"
 * into "foo/bar" and "my_file".
 *
 * @param full_path_s The value to extract the path and filename from.
 * @param path_ss Pointer to where a newly-allocated string containing the extracted path
 * will be stored. This will need to be freed with FreeCopiedString to avoid a memory leak
 * @param filename_ss Pointer to where a newly-allocated string containing the extracted filename
 * will be stored. This will need to be freed with FreeCopiedString to avoid a memory leak
 * @return <code>true</code> if the extraction was successful <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool DeterminePathAndFile (const char * const full_path_s, char ** const path_ss, char ** const filename_ss);


/**
 * @brief Generate a new filename by adding an extension to a filename.
 *
 * This will create a new filename by adding/replacing an extension on
 * a given filename. The extension will be placed either after the last
 * '.'in the filename or at the end of the filename if this could not be found.
 * This behaviour is also determined by replace_flag.
 *
 * @param filename_s The filename to add the extension to.
 * @param extension_s The extension to add.
 * @param replace_flag If this is <code>true</code> then any extension found on
 * the filename will be replaced. If this is <code>false</code> then the extension
 * will get concatenated to the end of the filename.
 * @return A newly-allocated string containing the new value which will
 * need to be freed with FreeCopiedString to avoid a memory leak. If there was an
 * error, then this will be <code>NULL</code>.
 */
WHEATIS_UTIL_API char *SetFileExtension (const char * const filename_s, const char * const extension_s, bool replace_flag);


/**
 * @brief Replace any occurrences of // in a string with the platform-specific
 * file separator.
 *
 * @param value_s The string to be amended.
 */
WHEATIS_UTIL_API void UsePlatformFileSeparator (char *value_s);


/**
 * Check whether a given file/directory exists.

 * @param path_s The path to check.
 * @return <code>true</code> if the object referred to by the path
 * exists, <code>false</code> otherwise.
 */
WHEATIS_UTIL_API bool IsPathValid (const char * const path_s);


/**
 * @brief Check whether a given path is absolute.
 *
 * @param path_s The path to check.
 * @return <code>true</code> if the path is absolute, <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool IsPathAbsolute (const char * const path_s);


/**
 * @brief Get files that match a given pattern.
 *
 * @param pattern_s The path including the pattern.
 * @param full_path_flag If this is <code>true</code> then the full path to matching files will
 * be stored. If it is <code>false</code> then only the local filenames will be stored.
 * @return A LinkedList of StringListNode containing the matching filenames or <code>NULL</code>
 * if no matches could be found.
 */
WHEATIS_UTIL_API LinkedList *GetMatchingFiles (const char * const pattern_s, const bool full_path_flag);


/**
 * @brief Get the platform-specific file separator.
 *
 * Get the character used between directories in a path e.g
 * // on Unix, Amiga, etc., \ on Windows.
 *
 * @return The separator
 */
WHEATIS_UTIL_API char GetFileSeparatorChar (void);


/**
 * @brief Get the platform-specific string for the current directory
 *
 * For example
 * .// on Unix, .\ on Windows.
 *
 * @return The string for the current directory.
 */
WHEATIS_UTIL_API const char *GetCurrentDirectoryString (void);


/**
 * @brief Get the platform-specific string for the parent directory
 *
 * For example
 * ..// on Unix, ..\ on Windows.
 *
 * @return The string for the parent directory.
 */
WHEATIS_UTIL_API const char *GetParentDirectory (void);


/**
 * @brief Get the platform-specific string for the a plugin
 *
 * For example
 * lib*.so on Unix, *.dll on Windows.
 *
 * @return The string for the plugin pattern.
 */
WHEATIS_UTIL_API const char *GetPluginPattern (void);


 /**
  * Makes sure that a given directory path exists.
  *
  * @param path_s The path to check. It can be an arbitrary number of levels deep.
  * @return <code>true</code> if the path already exists. If not, the all of directories
  * will attempt to be created. If this succeed then <code>true</code> will be returned, <code>false</code>
  * otherwise.
  */
WHEATIS_UTIL_API bool EnsureDirectoryExists (const char * const path_s);


/**
 * Copy the content of one file to another.
 *
 * @param src_filename_s The name of the file to copy from.
 * @param dest_filename_s The name of the file to copy to.
 * @param callback_fn An optional callback function for monitoring
 * progress with.
 * @return <code>true</code> if the copy was successful <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool CopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) ());


/**
 * Set the current working directory.
 *
 * @param path_s The path to set.
 * @return <code>true</code> if the change was successful <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool SetCurrentWorkingDirectory (const char * const path_s);


/**
 * Get the current working directory.
 *
 * @return The current working directory.
 */
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
 * Calculate the FileInformation for a given filename.
 *
 * @param path_s The path to the file/directory to get the information for.
 * @param info_p Where the FileInformation will be stored.
 * @return <code>true</code> if the information was calculated successfully, <code>false</code>
 * otherwise.
 * @memberof FileInformation
 */
WHEATIS_UTIL_API bool CalculateFileInformation (const char * const path_s, FileInformation *info_p);


/**
 * Delete a file.
 *
 * @param path_s The name of the file to delete.
 * @return <code>true</code> if the file was deleted successfully, <code>false</code>
 * otherwise.
 */
WHEATIS_UTIL_API bool RemoveFile (const char * const path_s);


#ifdef __cplusplus
}
#endif

#endif	/* FILESYSTEM_H */
