#ifndef IO_UTILS_H
#ifndef IO_UTILS_H



Text *GetLocalFilename (char * const original_filename_s);


/**
 * Determine whether a file is on the local filesystem or not.
 * 
 * @param filename_s The file to check.
 * @return The type of location of the file or FILE_LOCATION_UNKNOWN if it could
 * not be determined.
 */
FileLocation GetFileLocation (const char * const filename_s);


/**
 * Copy a file .
 * 
 * @param src_filename_s The name of the the source file.
 * @param dest_filename_s The name of the the source file.
 * @param callback_fn The callback_fn to denote progress (currently unused).
 * @return true on success, false on error with errno set to the appropriate value.
 */ 
char *CopyFileToLocalFilesystem (const char * const filename_s);



/**
 * Copy the contents of one named file to another.
 * 
 * @param src_filename_s The name of the the source file.
 * @param dest_filename_s The name of the the source file.
 * @param callback_fn The callback_fn to denote progress (currently unused).
 * @return true on success, false on error with errno set to the appropriate value.
 */ 
bool CopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) ());



/**
 * Get a newly allocated char * with the given value.
 * 
 * @param value The value to print into the the newly allocated string.
 * @return The string with the given value or NULL upon error.
 */
char *GetIntAsString (const int value);



#endif		/* #ifndef IO_UTILS_H */
