/**@file filesystem.h
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "typedefs.h"
#include "linked_list.h"
#include "epr_library.h"


#ifdef __cplusplus
	extern "C" {
#endif


/** Generic functions */
LIB_API char *GetFilenameOnly (const char * const full_path_to_file);
LIB_API char *GetPathOnly (const char * const full_path_to_file);
LIB_API char *MakeFilename (const char * const path, const char * const filename);

LIB_API BOOLEAN DeterminePathAndFile (const char * const full_path_s, char ** const path_ss, char ** const filename_ss);


LIB_API void UsePlatformFileSeparator (char *value_s);

/** Platform-specific functions */
LIB_API BOOLEAN IsPathValid (const char * const path);
LIB_API BOOLEAN IsPathAbsolute (const char * const path);
LIB_API LinkedList *GetMatchingFiles (const char * const pattern);

LIB_API char GetFileSeparatorChar (void);
LIB_API const char *GetCurrentDirectoryString (void);
LIB_API const char *GetParentDirectory (void);
LIB_API const char *GetPluginPattern (void);

LIB_API BOOLEAN EnsureDirectoryExists (const char * const path_s);
LIB_API BOOLEAN CopyToNewFile (const char * const src_filename, const char * const dest_filename, void (*callback_fn) ());


LIB_API BOOLEAN SetCurrentWorkingDirectory (const char * const path);
LIB_API char *GetCurrentWorkingDirectory (void);

LIB_API BOOLEAN IsDirectory (const char * const path);

LIB_API BOOLEAN EnsureDirectoryPath (const char * const path_s);

#ifdef __cplusplus
}
#endif

#endif	/* FILESYSTEM_H */
