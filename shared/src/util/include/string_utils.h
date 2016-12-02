/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/**@file string_utils.h
 * @brief A set of string functions.
 *
 *  A variety of functions for manipulating strings.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>

#include "typedefs.h"
#include "grassroots_util_library.h"
#include "linked_list.h"

#include "uuid/uuid.h"

/**
 * The number of bytes required to store a c-style string
 * representation of a UUID.
 */
#define UUID_STRING_BUFFER_SIZE (37)

/**
 * The number of bytes required to store the raw data
 * of a UUID.
 */
#define UUID_RAW_SIZE (16)

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param length_to_copy If this is greater than zero, copy this many characters from src_p
 * or its entirety, whichever is smaller. If this is zero or less then all of src_p is copied.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error. If
 * the string was successfully created, its deallocation must be done with FreeCopiedString
 * when you are finished with it.
 */
GRASSROOTS_UTIL_API char *CopyToNewString (const char * const src_p, const size_t length_to_copy, bool trim);


/**
 * Delete a string previously allocated using CopyToNewString.
 *
 * @param str_p The string to deallocate.
 */
GRASSROOTS_UTIL_API void FreeCopiedString (char *str_p);


/**
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The File to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return TRUE if the line was read successfully, false if there was an error.
 */
GRASSROOTS_UTIL_API bool GetLineFromFile (FILE *in_f, char **buffer_pp);


/**
 * Free a buffer that has been allocated with GetLineFromFile.
 *
 * @param buffer_p Pointer to a buffer to be freed.
 */
GRASSROOTS_UTIL_API void FreeLineBuffer (char *buffer_p);


/**
 * Parse the format string and sort out into a linked
 * list of tokens.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @param treat_whitespace_as_delimiter_flag If TRUE whitespace will be treated as a delimiter.
 * @return A pointer to a StringLinkedList where each node
 * refers to a token or NULL upon error.
 */
GRASSROOTS_UTIL_API LinkedList *ParseStringToStringLinkedList (const char * const format_p, const char * const delimiters_p, const bool treat_whitespace_as_delimiter_flag);



/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 */
GRASSROOTS_UTIL_API LinkedList *ParseStringToIntLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 */
GRASSROOTS_UTIL_API LinkedList *ParseStringToDataLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Get a newly created string that is the concatenation of two others.
 *
 * @param first_s  The first string to concatenate.
 * @param second_s  The second string to concatenate.
 * @return The newly created string or NULL if there was an error.
 */
GRASSROOTS_UTIL_API char *ConcatenateStrings (const char * const first_s, const char * const second_s);


/**
 * Replace all instances of a charcter by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 */
GRASSROOTS_UTIL_API void ReplaceExpression (char *value_s, const char char_to_replace, const char replacement_char);


/**
 * Get and make copies of two strings from a line of the format
 *
 * key string = value string
 *
 * @param line_p The string to get the values from.
 * @param key_pp Pointer to a new string where the key will be stored upon success.
 * @param value_pp Pointer to a new string where the value will be stored upon success.
 * @param comment_identifier If the line begins with this charcter then treat the whole
 * line as a comment and do not try to get the key and value.
 * @return TRUE if the key and values were successfully copied, FALSE otherwise.
 */
GRASSROOTS_UTIL_API bool GetKeyValuePair (char *line_p, char **key_pp, char **value_pp, const char comment_identifier);


/**
 * Get a copy of the next word from a buffer.
 *
 * @param start_pp Address of the buffer to read from.
 * @param delimiters_p An array of characters that can be used as delimiters. If this is NULL
 * then the behvaiour is determined by check_for_whitespace_flag.
 * @param check_for_whitespace_flag
 * @param update_position_flag If this is set to <code>true</code>, then the address pointed to by start_pp will be updated to after
 * the token.
 * @return A copied version of the string or <code>NULL</code> if there was an error or no more tokens were available.
 * @see FreeToken
 */
GRASSROOTS_UTIL_API char *GetNextToken (const char **start_pp, const char *delimiters_p, const bool check_for_whitespace_flag, const bool update_position_flag);

/**
 * Free a c-style string that has been returned by GetNextToken.
 *
 * @param token_s The token to free.
 * @see GetNextToken
 */
GRASSROOTS_UTIL_API void FreeToken (char *token_s);


/**
 * Format a string containing a number by removing all of its
 * trailing zeroes, e.g.
 *
 *		7.0000 would become 7
 *		7.5000 would become 7.5
 *
 * @param numeric_string_p The string to formatted.
 */
GRASSROOTS_UTIL_API void NullifyTrailingZeroes (char *numeric_string_p);


/**
 * Test whether a string is NULL or contains just whitespace.
 *
 * @param value_s The string to test.
 * @return <code>true</code> If the string is <code>NULL</code> or just whitespace, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool IsStringEmpty (const char *value_s);


/**
 * Do a case-insensitive comparison between two strings.
 * This is a case-insensitive version of the standard ANSI function strcmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 */
GRASSROOTS_UTIL_API int Stricmp (const char *c0_s, const char *c1_s);


/**
 * Do a case-insensitive comparison between the initial portions of two strings.
 * This is a case-insensitive version of the standard ANSI function strncmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @param length The maximum number of characters in each string to compare.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 */
GRASSROOTS_UTIL_API int Strnicmp (const char *c0_s, const char *c1_s, size_t length);


/**
 * Find a substring within a string. The search is case-insensitive.
 * This is a case-insensitive version of the standard ANSI function strstr.
 *
 * @param value_s The string to search within.
 * @param substring_s The string to search for
 * @return The substring within the larger string or <code>NULL</code> if it was
 * not found.
 */
GRASSROOTS_UTIL_API const char *Stristr (const char *value_s, const char *substring_s);


/**
 * Get an integer as a string, e.g.
 *
 *  1 would be converted to "1"
 *
 * @param value The value to convert to a string.
 * @return The c-style string or <code>NULL</code> upon error.
 * This needs to be freed using FreeCopiedString.
 * @see FreeCopiedString.
 */
GRASSROOTS_UTIL_API char *ConvertIntegerToString (const int value);


/**
 * Get an double as a string, e.g.
 *
 *  1.03 would be converted to "1.03"
 *
 * @param value The value to convert to a string.
 * @return The c-style string or <code>NULL</code> upon error.
 * This needs to be freed using FreeCopiedString.
 * @see FreeCopiedString.
 */
GRASSROOTS_UTIL_API char *ConvertDoubleToString (const double64 value);


/**
 * Get the string representation of a uuid_t.
 *
 * @param id The uuid_t to get the string representation of.
 * @return A newly-allocated string of the id or <code>NULL</code> upon error.
 * @see FreeUUIDString.
 */
GRASSROOTS_UTIL_API char *GetUUIDAsString (const uuid_t id);


/**
 * Convert a uuid_t into a given string buffer.
 *
 * @param id The uuid_t to get the string representation of.
 * @param uuid_s The buffer to write the representation to. This must be at least
 * UUID_STRING_BUFFER_SIZE bytes long.
 * @see UUID_STRING_BUFFER_SIZE
 */
GRASSROOTS_UTIL_API void ConvertUUIDToString (const uuid_t id, char *uuid_s);



/**
 * Convert the string representation of a uuid_t to a uuid_t.
 *
 * @param id_s The buffer containing the representation.
 * @param id The uuid_t to set.
 * @return <code>true</code> if the uuid_t was set successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool ConvertStringToUUID (const char *id_s, uuid_t id);


/**
 * Free a string representation of a uuid_t.
 *
 * @param uuid_s The string representation to free.
 * @see GetUUIDAsString.
 */
GRASSROOTS_UTIL_API void FreeUUIDString (char *uuid_s);


/**
 * Make a copy of a string.
 *
 * @param dest_ss Pointer to the string where the source string
 * will be copied to.  * @param src_s The string to copy.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * If successful, any string that dest_ss currently points to will be freed
 * prior to pointing at the copy. If the function is unsuccessful then the value
 * pointed to by dest_ss will remain intact.
 */
GRASSROOTS_UTIL_API bool ReplaceStringValue (char **dest_ss, const char * const src_s);


/**
 * Replace each instance of a character within a string with another
 *
 * @param value_s The string to replace the character in.
 * @param old_data The character to be replaced.
 * @param new_data The replacement character.
 */
GRASSROOTS_UTIL_API void ReplaceChars (char *value_s, char old_data, char new_data);


/**
 * Create a numeric value for suitable for hashing from a given string.
 *
 * @param key_p The string to create the hashed value for.
 * @return The hashed value.
 */
GRASSROOTS_UTIL_API uint32 HashString (const void * const key_p);


/**
 * Get the contents of FILE as c-style string.
 *
 * @param input_f The FILE to get the contents of
 * @return The newly-allocated c-style string containing the contents
 * of the FILE or <code>NULL</code> upon error. If this value is not
 * <code>NULL</code>, it should be freed by calling FreeCopiedString
 * when it is no longer needed.
 * @see FreeCopiedString
 */
GRASSROOTS_UTIL_API char *GetFileContentsAsString (FILE *input_f);


/**
 * Get the contents of FILE as c-style string.
 *
 * @param filename_s The filename to get the contents of
 * @return The newly-allocated c-style string containing the contents
 * of the FILE or <code>NULL</code> upon error. If this value is not
 * <code>NULL</code>, it should be freed by calling FreeCopiedString
 * when it is no longer needed.
 * @see GetFileContentsAsString
 */
GRASSROOTS_UTIL_API char *GetFileContentsAsStringByFilename (const char *filename_s);

/**
 * Concatenate a va_list of strings.
 *
 * @param value_s The varargs-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varargs-array must be a <code>NULL</code>.
 * @return The newly-allocated string that should be freed with FreeCopiedString when
 * no longer needed or <code>NULL</code> upon failure.
 */
GRASSROOTS_UTIL_API char *ConcatenateVarargsStrings (const char *value_s, ...);



/**
 * Test whether a uuid_t contains a valid value.
 *
 * @param id The uuid_t to test.
 * @return <code>true</code> if the uuid_t is valid, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool IsUUIDSet (uuid_t id);



/**
 * Test whether a given string contains any whitespace characters.
 *
 * @param value_s The string to check.
 * @return <code>true</code> if the string contains any whitespace characters, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool DoesStringContainWhitespace (const char *value_s);


#ifdef __cplusplus
}
#endif





#endif	/* #ifndef STRING_UTILS_H */

