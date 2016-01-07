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

#define UUID_STRING_BUFFER_SIZE (37)
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
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 */
GRASSROOTS_UTIL_API LinkedList *ParseStringToIntLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
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
 * @param buffer_p The buffer to read from.
 * @param end_pp If the word is successfully read, then this will
 * be where the orginal word  in the buffer ends.
 * @return A copied version of the string.
 */
GRASSROOTS_UTIL_API char *GetNextToken (const char **start_pp, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


GRASSROOTS_UTIL_API bool GetNextTokenAsRealNumber (const char **start_pp, double *data_p, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


GRASSROOTS_UTIL_API bool GetNextTokenAsInteger (const char **start_pp, int *value_p, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


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
 * @return TRUE If the string is NULL or just whitespace, FALSE otherwise.
 */
GRASSROOTS_UTIL_API bool IsStringEmpty (const char *value_s);


GRASSROOTS_UTIL_API int Stricmp (const char *c0_s, const char *c1_s);


GRASSROOTS_UTIL_API int Strnicmp (const char *c0_p, const char *c1_p, size_t length);


GRASSROOTS_UTIL_API char *ConvertIntegerToString (const int value);


GRASSROOTS_UTIL_API char *ConvertDoubleToString (const double64 value);


/**
 * Get the string representation of a uuid_t.
 *
 * @param id The uuid_t to get the string representation of.
 * @return A newly-allocated string of the id or <code>NULL<code> upon error.
 * @see FreeUUIDString.
 */
GRASSROOTS_UTIL_API char *GetUUIDAsString (const uuid_t id);


/**
 * Convert a uuid_t into a given string buffer.
 *
 * @param id The uuid_t to get the string representation of.
 * @param The buffer to write the representation to. This must be at least
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


GRASSROOTS_UTIL_API void ClearUUID (uuid_t *id_p);


GRASSROOTS_UTIL_API bool IsUUIDSet (uuid_t id);


GRASSROOTS_UTIL_API void GenerateUUID (uuid_t *id_p);


GRASSROOTS_UTIL_API bool ReplaceStringValue (char **dest_ss, const char * const src_s);


GRASSROOTS_UTIL_API void ReplaceChars (char *value_s, char old_data, char new_data);


GRASSROOTS_UTIL_API uint32 HashString (const void * const key_p);


GRASSROOTS_UTIL_API char *GetFileContentsAsString (FILE *input_f);


/**
 * Create a new c-style string from a va_list of strings.
 *
 * @param value_s The varargs-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varargs-array must be a <code>NULL</code>.
 * @param args The varargs list of arguments used by value_s.
 * @return <code>true</code> if the append was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof see AppendVarArgsToByteBuffer
 */
GRASSROOTS_UTIL_API char *ConcatenateVarargsStrings (const char *value_s, ...);



#ifdef __cplusplus
}
#endif





#endif	/* #ifndef STRING_UTILS_H */

