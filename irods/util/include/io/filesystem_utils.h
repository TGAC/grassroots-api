/**@file filesystem.h
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "typedefs.h"
#include "linked_list.h"
#include "irods_library.h"


#ifdef __cplusplus
	extern "C" {
#endif


/** Generic functions */
IRODS_LIB_API char *GetFilenameOnly (const char * const full_path_to_file);
IRODS_LIB_API char *GetPathOnly (const char * const full_path_to_file);
IRODS_LIB_API char *MakeFilename (const char * const path, const char * const filename);

IRODS_LIB_API bool DeterminePathAndFile (const char * const full_path_s, char ** const path_ss, char ** const filename_ss);


IRODS_LIB_API void UsePlatformFileSeparator (char *value_s);

/** Platform-specific functions */
IRODS_LIB_API bool IsPathValid (const char * const path);
IRODS_LIB_API bool IsPathAbsolute (const char * const path);
IRODS_LIB_API LinkedList *GetMatchingFiles (const char * const pattern, const bool full_path_flag);

IRODS_LIB_API char GetFileSeparatorChar (void);
IRODS_LIB_API const char *GetCurrentDirectoryString (void);
IRODS_LIB_API const char *GetParentDirectory (void);
IRODS_LIB_API const char *GetPluginPattern (void);

IRODS_LIB_API bool EnsureDirectoryExists (const char * const path_s);
IRODS_LIB_API bool CopyToNewFile (const char * const src_filename, const char * const dest_filename, void (*callback_fn) ());


IRODS_LIB_API bool SetCurrentWorkingDirectory (const char * const path);
IRODS_LIB_API char *GetCurrentWorkingDirectory (void);

IRODS_LIB_API bool IsDirectory (const char * const path);

IRODS_LIB_API bool EnsureDirectoryPath (const char * const path_s);

#ifdef __cplusplus
}
#endif

#endif	/* FILESYSTEM_H */
