#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "memory_allocations.h"
#include "string_linked_list.h"
#include "string_utils.h"
#include "typedefs.h"


static LinkedList *ParseStringToLinkedList (const char * const format_p, const char * const delimiters_p, LinkedList *(*allocate_list_fn) (void), ListNode *(*allocate_node_fn) (const char *value_s), const bool treat_whitespace_as_delimiter_flag);

static ListNode *GetStringListNode (const char *value_s);


static const char *ScrollPastSpecificCharacters (const char **value_pp, const char * const delimiters_s, const bool ignore_whitespace_flag, const bool state_flag);


#ifdef _DEBUG
	#define STRING_UTILS_DEBUG (STM_LEVEL_FINER)
#else
	#define STRING_UTILS_DEBUG (STM_LEVEL_NONE)
#endif

void FreeCopiedString (char *str_p)
{
	if (str_p)
		{
			FreeMemory (str_p);
		}
}

/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error.
 */
char *CopyToNewString (const char * const src_p, const size_t l, bool trim)
{
	char *dest_p = NULL;

	if (src_p)
		{
			size_t len = strlen (src_p);

			if (l > 0)
				{
					if (len > l)
						{
							len = l;
						}
				}

			if (trim)
				{
					const char *start_p = src_p;
					const char *end_p = src_p + len - 1;

					while (isspace (*start_p))
						{
						 ++ start_p;
						}

					if (*start_p == '\0')
						{
							/* all white space */
							return NULL;
						}

					while ((start_p < end_p) && (isspace (*end_p)))
						{
						 -- end_p;
						}

					if (start_p <= end_p)
						{
							ptrdiff_t d = end_p - start_p + 1;
							dest_p = (char *) AllocMemory (d + 1);

							if (dest_p)
								{
									memcpy (dest_p, start_p, d);
									* (dest_p + d) = '\0';

									return dest_p;
								}

						}
				}
			else
				{
					dest_p = (char *) AllocMemory (len + 1);

					if (dest_p)
						{
							strncpy (dest_p, src_p, len);
							* (dest_p + len) = '\0';

							return dest_p;
						}
				}
		}

	return dest_p;
}


/**
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The FILE to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return true if the line was read successfully, false if there was an error.
 */
bool GetLineFromFile (FILE *in_f, char **buffer_pp)
{
	const size_t buffer_size = 1024;
	size_t last_index = 0;
	size_t old_size = 0;
	size_t buffer_length = 0;
	char *old_buffer_p = NULL;
	char *new_buffer_p = NULL;
	size_t latest_block_length = 0;
	size_t new_size = 0;
	size_t actual_length = 0;

	if (!feof (in_f))
		{
			bool looping = true;

			do
				{
					new_size = old_size + buffer_size;

					new_buffer_p = (char *) ReallocMemory (old_buffer_p, new_size, old_size);

					if (new_buffer_p)
						{
							char * const latest_block_p = new_buffer_p + buffer_length;

							* (new_buffer_p + new_size - 1) = '\0';

							if (new_buffer_p != old_buffer_p)
								{
									old_buffer_p = new_buffer_p;
								}

							if (fgets (latest_block_p, buffer_size, in_f))
								{
									char c;

									latest_block_length = strlen (latest_block_p);

									buffer_length += latest_block_length;
									last_index = buffer_length - 1;
									old_size = new_size;

									c = * (new_buffer_p + last_index);
									looping = (c != '\r') && (c != '\n') && (!feof (in_f));
								}
							else
								{
									*latest_block_p = '\0';
									looping = false;
								}
						}
					else
						{
							return false;
						}
				}
			while (looping);

			/* Remove the carriage return and/or line feed */
			actual_length = (new_buffer_p != NULL) ? strlen (new_buffer_p) : 0;
			if (actual_length > 0)
				{
					char *c_p = new_buffer_p + actual_length - 1;

					looping = true;
					while (looping)
						{
							if ((*c_p == '\r') || (*c_p == '\n'))
								{
									*c_p = '\0';

									if (c_p > new_buffer_p)
										{
											-- c_p;
										}
									else
										{
											looping = false;
										}
								}
							else
								{
									looping = false;
								}

						}		/* while (looping) */

				}		/* if (actual_length > 0) */

			*buffer_pp = new_buffer_p;
			return true;
		}

	*buffer_pp = NULL;
	return false;
}


void FreeLineBuffer (char *buffer_p)
{
	FreeMemory (buffer_p);
}



char *ConcatenateStrings (const char * const first_s, const char * const second_s)
{
	const size_t len1 = (first_s != NULL) ? strlen (first_s) : 0;
	const size_t len2 = (second_s != NULL) ? strlen (second_s) : 0;

	char *result_s = (char *) AllocMemory (sizeof (char) * (len1 + len2 + 1));

	if (result_s)
		{
			if (len1 > 0)
				{
					strncpy (result_s, first_s, len1);
				}

			if (len2 > 0)
				{
					strcpy (result_s + len1, second_s);
				}

			* (result_s + len1 + len2) = '\0';
		}

	return result_s;
}



/**
 * Parse the format string and sort out into a linked
 * list of tokens.
 *
 * @param format_p The format string to use.
 * @param treat_whitespace_as_delimiter_flag If true whitespace will be treated as a delimiter.
 * @return A pointer to a StringLinkedList where each node
 * refers to a token or NULL upon error.
 */
LinkedList *ParseStringToStringLinkedList (const char * const format_p, const char * const delimiters_p, const bool treat_whitespace_as_delimiter_flag)
{
	return (ParseStringToLinkedList (format_p, delimiters_p, AllocateStringLinkedList, GetStringListNode, treat_whitespace_as_delimiter_flag));
}


/**
 * Scroll through a string until we reach a charcter that satisfies some input constraints.
 *
 * @param state_flag If this is true then keep scrolling until we find some non-whitespace/delimiters, if false keep
 * scrolling until we find some whitespace/delimiters.
 */
static const char *ScrollPastSpecificCharacters (const char **value_pp, const char * const delimiters_s, const bool check_for_whitespace_flag, const bool state_flag)
{
	const char *value_s = *value_pp;
	const size_t delimiters_length = (delimiters_s != NULL) ? strlen (delimiters_s) : 0;
	bool is_delimter_flag;
	bool loop_flag = (*value_s != '\0');

	while (loop_flag)
		{
			const char c = *value_s;

			is_delimter_flag = false;

			/* Are we're checking for whitespace? */
			if (check_for_whitespace_flag)
				{
					is_delimter_flag = (isspace (c) != 0);
				}		/* if (check_for_whitespace_flag) */

			/* if the whitespace check failed, test against our delimiters */
			if (!is_delimter_flag)
				{
					/* If necessary, check for any delimiters */
					if (delimiters_length > 0)
						{
							size_t i = delimiters_length;
							const char *delim_p = delimiters_s;

							while (i > 0)
								{
									is_delimter_flag = (*delim_p == c);

									if (is_delimter_flag)
										{
											i = 0;
										}
									else
										{
											-- i;
											++ delim_p;
										}

								}		/* while (i > 0) */

						}		/* if (delimiters_length > 0) */

				}		/* if (!is_delimter_flag) */


			/* Are we continuing? */
			if (is_delimter_flag == state_flag)
				{
					++ value_s;

					loop_flag = (*value_s != '\0');
				}
			else
				{
					loop_flag = false;
				}

		}		/* while (loop_flag) */

	return value_s;
}


/**
 * Parse the format string and sort out into a linked
 * list of items.
 *
 * @param format_p The format string to use.
 * @param allocate_list_fn The function used to create the LinkedList.

 * @return A pointer to a LinkedList where each node
 * refers to a token or NULL upon error.
 */
static LinkedList *ParseStringToLinkedList (const char * const format_p, const char * const delimiters_p, LinkedList *(*allocate_list_fn) (void), ListNode *(*allocate_node_fn) (const char *value_s), const bool treat_whitespace_as_delimiter_flag)
{
	LinkedList *tokens_list_p = NULL;
	const char *index_p = format_p;

	if (index_p)
		{
			tokens_list_p = allocate_list_fn ();

			if (tokens_list_p)
				{
					const char *ptr = format_p;
					bool loop_flag = true;
					bool error_flag = false;
					char *copy_p = NULL;

					while (loop_flag)
						{
							/* scroll past any delimiters */
							char *value_s = GetNextToken (&ptr, delimiters_p, treat_whitespace_as_delimiter_flag, true);

							if (value_s)
								{
									ListNode *node_p = allocate_node_fn (value_s);

									if (node_p)
										{
											LinkedListAddTail (tokens_list_p, node_p);

											/*
											ptr = strtok (NULL, delimiters_p);
											loop_flag = (ptr != NULL);
											*/
										}
									else
										{
											loop_flag = false;
											error_flag = true;
										}

									FreeCopiedString (value_s);
								}		/* if (value_s) */
							else
								{
									loop_flag = false;
								}

						}		/* while (loop_flag) */

					if (error_flag)
						{
							FreeLinkedList (tokens_list_p);
							tokens_list_p = NULL;
						}

					FreeMemory (copy_p);


					/* If the list is empty, don't return it */
					if (tokens_list_p && (tokens_list_p-> ll_size == 0))
						{
							FreeLinkedList (tokens_list_p);
							tokens_list_p = NULL;
						}

				}		/* if (tokens_list_p) */

		}		/* if (index_p) */

	return tokens_list_p;
}


static ListNode *GetStringListNode (const char *value_s)
{
	ListNode *node_p = (ListNode *) AllocateStringListNode (value_s, MF_DEEP_COPY);

	return node_p;
}


/**
 * Replace all instances of a charcter by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 */
void ReplaceExpression (char *value_s, const char char_to_replace, const char replacement_char)
{
	char *ptr = strchr (value_s, char_to_replace);

	while (ptr)
		{
			*ptr = replacement_char;
			ptr = strchr (ptr + 1, char_to_replace);
		}
}


bool GetKeyValuePair (char *line_p, char **key_pp, char **value_pp, const char comment_identifier)
{
	char *marker_p = strchr (line_p, comment_identifier);
	size_t l;
	bool success_flag = false;

	/* treat the # as a comment delimiter for the rest of the line */
	if (marker_p)
		{
			*marker_p = '\0';
		}

	*key_pp = NULL;
	*value_pp = NULL;

	l = strlen (line_p);
	if (l > 2)
		{
			marker_p = strtok (line_p, "=");

			if (marker_p)
				{
					char *key_p = CopyToNewString (marker_p, 0, true);

					if (key_p)
						{
							marker_p = strtok (NULL, "=");

							if (marker_p)
								{
									char *value_p = CopyToNewString (marker_p, 0, true);

									if (value_p)
										{
											*key_pp = key_p;
											*value_pp = value_p;
											success_flag = true;
										}
								}

							if (!success_flag)
								{
									FreeCopiedString (key_p);
								}
						}
				}
		}

	return success_flag;
}



/** Is the line empty or a comment? */
bool IsLineValid (const char * const buffer_p)
{
	char c = '\0';

	if (sscanf (buffer_p, "%c", &c) == 1)
		{
			if (c != '#')
				{
					return true;
				}
		}

	return false;
}


/**
 * Get a copy of the next word from a buffer.
 *
 * @param buffer_p The buffer to read from.
 * @param end_pp If the word is successfully read, then this will
 * be where the orginal word  in the buffer ends.
 * @return A copied version of the string.
 */
char *GetNextToken (const char **start_pp, const char *delimiters_s, const bool ignore_whitespace_flag, const bool update_position_flag)
{
	char *word_s = NULL;
	const char *value_p = *start_pp;

	const char *start_p = ScrollPastSpecificCharacters (&value_p, delimiters_s, ignore_whitespace_flag, true);

	if (start_p && *start_p)
		{
			const char *end_p = NULL;
			bool quoted_token_flag = false;

			value_p = start_p;

			/* does it start with a " in which case we have a quoted string? */
			if (*start_p == '\"')
				{
					quoted_token_flag = true;

					++ value_p;

					end_p = ScrollPastSpecificCharacters (&value_p, "\"", false, false);
				}
			else
				{
					end_p = ScrollPastSpecificCharacters (&value_p, delimiters_s, ignore_whitespace_flag, false);
				}

			if (end_p)
				{

					/* scroll past the quotes  */
					if (quoted_token_flag)
						{
							++ start_p;
						}

					if (start_p != end_p)
						{
							const ptrdiff_t d = end_p - start_p;

							word_s = CopyToNewString (start_p, d, false);

							if (*end_p)
								{
									if (update_position_flag)
										{
											*start_pp = end_p + 1;
										}
								}
							else
								{
									*start_pp = end_p;
								}

						}		/* if (start_p != end_p) */

				}		/* if (end_p) */

		}		/* if (start_p) */

	return word_s;
}


void FreeToken (char *token_s)
{
	FreeMemory (token_s);
}



void NullifyTrailingZeroes (char *numeric_string_p)
{
	size_t l = strlen (numeric_string_p);
	char *c_p = numeric_string_p + (l - 1);

	bool loop_flag = (l >= 0);

	while (loop_flag)
		{
			switch (*c_p)
				{
					case '0':
						*c_p = '\0';
						-- l;
						-- c_p;
						break;

					case '.':
						*c_p = '\0';
						loop_flag = false;
						break;

					default:
						loop_flag = false;
						break;

				}		/* switch (*c_p) */

		}		/* while (loop_flag) */
}


bool IsStringEmpty (const char *value_s)
{
	if (value_s)
		{
			while (value_s)
				{
					if (isspace (*value_s))
						{
							++ value_s;
						}
					else
						{
							return false;
						}
				}
		}

	return true;
}



char *ConvertIntegerToString (const int value)
{
	char *value_s = NULL;
	size_t num_digits = 1;
	
	if (value < 0)
		{
			size_t temp = (size_t) log10 ((double) -value);
			++ num_digits;
			
			num_digits += temp;
		}
	else if (value > 0)
		{
			num_digits += (size_t) log10 ((double) value);
		}
		
	value_s = (char *) malloc (num_digits + 1);
	
	if (value_s)
		{
			sprintf (value_s, "%d", value);
			* (value_s + num_digits) = '\0';
		}

	return value_s;
}


char *ConvertDoubleToString (const double64 value)
{
	char buffer [1024];
	sprintf (buffer, "%lf", value);

	return CopyToNewString (buffer, 0, false);
}



int Stricmp (const char *c0_p, const char *c1_p)
{
	int res = 0;

	if (c0_p != c1_p)
		{
			bool loop_flag = true;
			int i0;
			int i1;
			char c0;
			char c1;

			while (loop_flag)
				{
					c0 = tolower (*c0_p);
					c1 = tolower (*c1_p);

					if ((c0 == c1) && (c0 != '\0'))
						{
							++ c0_p;
							++ c1_p;
						}
					else
						{
							loop_flag = false;
						}
				}

			/* The comparison must be performed as if the characters were unsigned characters. */
			i0 = (unsigned char) c0;
			i1 = (unsigned char) c1;

			res = i0 - i1;
		}

	return res;
}
