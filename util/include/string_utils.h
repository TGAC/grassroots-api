/**@file string_utils.h
*/

/** \file string_utils.h
 *  \brief A set of string functions.
 *
 *  A variety of functions for manipulating strings.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>

#include "typedefs.h"
#include "wheatis_util_library.h"
#include "linked_list.h"

#include "uuid/uuid.h"

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
WHEATIS_UTIL_API char *CopyToNewString (const char * const src_p, const size_t length_to_copy, bool trim);


/**
 * Delete a string previously allocated using CopyToNewString.
 *
 * @param str_p The string to deallocate.
 */
WHEATIS_UTIL_API void FreeCopiedString (char *str_p);


/**
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The File to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return TRUE if the line was read successfully, false if there was an error.
 */
WHEATIS_UTIL_API bool GetLineFromFile (FILE *in_f, char **buffer_pp);


/**
 * Free a buffer that has been allocated with GetLineFromFile.
 *
 * @param buffer_p Pointer to a buffer to be freed.
 */
WHEATIS_UTIL_API void FreeLineBuffer (char *buffer_p);


/**
 * Parse the format string and sort out into a linked
 * list of tokens.
 *
 * @param format_p The format string to use.
 * @param treat_whitespace_as_delimiter_flag If TRUE whitespace will be treated as a delimiter.
 * @return A pointer to a StringLinkedList where each node
 * refers to a token or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *ParseStringToStringLinkedList (const char * const format_p, const char * const delimiters_p, const bool treat_whitespace_as_delimiter_flag);



/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *ParseStringToIntLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 */
WHEATIS_UTIL_API LinkedList *ParseStringToDataLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Get a newly created string that is the concatenation of two others.
 *
 * @param first_s  The first string to concatenate.
 * @param second_s  The second string to concatenate.
 * @return The newly created string or NULL if there was an error.
 */
WHEATIS_UTIL_API char *ConcatenateStrings (const char * const first_s, const char * const second_s);


/**
 * Replace all instances of a charcter by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 */
WHEATIS_UTIL_API void ReplaceExpression (char *value_s, const char char_to_replace, const char replacement_char);


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
WHEATIS_UTIL_API bool GetKeyValuePair (char *line_p, char **key_pp, char **value_pp, const char comment_identifier);


/**
 * Get a copy of the next word from a buffer.
 *
 * @param buffer_p The buffer to read from.
 * @param end_pp If the word is successfully read, then this will
 * be where the orginal word  in the buffer ends.
 * @return A copied version of the string.
 */
WHEATIS_UTIL_API char *GetNextToken (const char **start_pp, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


WHEATIS_UTIL_API bool GetNextTokenAsRealNumber (const char **start_pp, double *data_p, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


WHEATIS_UTIL_API bool GetNextTokenAsInteger (const char **start_pp, int *value_p, const char *delimiters_p, const bool ignore_whitespace_flag, const bool update_position_flag);


WHEATIS_UTIL_API void FreeToken (char *token_s);


/**
 * Format a string containing a number by removing all of its
 * trailing zeroes, e.g.
 *
 *		7.0000 would become 7
 *		7.5000 would become 7.5
 *
 * @param numeric_string_p The string to formatted.
 */
WHEATIS_UTIL_API void NullifyTrailingZeroes (char *numeric_string_p);


/**
 * Test whether a string is NULL or contains just whitespace.
 *
 * @param value_s The string to test.
 * @return TRUE If the string is NULL or just whitespace, FALSE otherwise.
 */
WHEATIS_UTIL_API bool IsStringEmpty (const char *value_s);



WHEATIS_UTIL_API int Stricmp (const char *c0_s, const char *c1_s);


WHEATIS_UTIL_API char *ConvertIntegerToString (const int value);

WHEATIS_UTIL_API char *ConvertDoubleToString (const double64 value);


WHEATIS_UTIL_API char *GetUUIDAsString (const uuid_t id);


WHEATIS_UTIL_API void FreeUUIDString (char *uuid_s);

#ifdef __cplusplus
}
#endif





#endif	/* #ifndef STRING_UTILS_H */

