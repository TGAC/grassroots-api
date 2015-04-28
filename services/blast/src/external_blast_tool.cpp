/*
 * external_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <cstring>

#include "external_blast_tool.hpp"
#include "string_utils.h"
#include "temp_file.hpp"


ExternalBlastTool :: ExternalBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
: BlastTool (job_p, name_s)
{
	bool success_flag = false;

	ebt_buffer_p = AllocateByteBuffer (1024);
	ebt_input_p = 0;
	ebt_output_p = 0;
	ebt_arg_callback = &ExternalBlastTool :: AddArgToInternalBuffer;
	ebt_working_directory_s = working_directory_s;

	if ((this ->*ebt_arg_callback) ("-db"))
		{
			if ((this ->*ebt_arg_callback) (name_s))
				{
					success_flag = true;
				}
		}
}


ExternalBlastTool :: ~ExternalBlastTool ()
{
	if (ebt_input_p)
		{
			delete ebt_input_p;
		}

	if (ebt_output_p)
		{
			delete ebt_output_p;
		}

	FreeByteBuffer (ebt_buffer_p);
}


bool ExternalBlastTool :: AddArg (const char *arg_s)
{
	bool success_flag = true;

	if (ebt_buffer_p -> bb_current_index > 0)
		{
			success_flag = AppendStringToByteBuffer (ebt_buffer_p, " ");
		}

	if (success_flag)
		{
			success_flag =  AppendStringToByteBuffer (ebt_buffer_p, arg_s);
		}

	return success_flag;
}


const char *ExternalBlastTool :: GetOutputData ()
{
	const char *results_s = NULL;

	if (ebt_output_p -> Open ("r"))
		{
			results_s = ebt_output_p -> GetData ();
		}

	return results_s;
}


/*
#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')
#define TAG_BLAST_SUBRANGE_FROM MAKE_TAG ('B', 'Q', 'F', 'R')
#define TAG_BLAST_SUBRANGE_TO MAKE_TAG ('B', 'Q', 'T', 'O')
#define TAG_BLAST_MATCH_SCORE MAKE_TAG ('B', 'M', 'T', 'C')
#define TAG_BLAST_MISMATCH_SCORE MAKE_TAG ('B', 'M', 'S', 'M')
*/

bool ExternalBlastTool :: ParseParameters (ParameterSet *params_p)
{
	bool success_flag = false;
	SharedType value;
	const char *filename_s = NULL;

	memset (&value, 0, sizeof (SharedType));

	/* Input query */
	if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_QUERY, &value, true))
		{
			char *sequence_s = value.st_string_value_s;

			if (!IsStringEmpty (sequence_s))
				{
					char *buffer_p = GetTempFilenameBuffer ();

					if (buffer_p)
						{
							ebt_input_p = TempFile :: GetTempFile (buffer_p, "w");

							if (ebt_input_p)
								{
									if (ebt_input_p -> Print (sequence_s))
										{
											filename_s = ebt_input_p -> GetFilename ();
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, "Blast service failed to write to temp file \"%s\" for query \"%s\"", ebt_input_p -> GetFilename (), sequence_s);
										}

									ebt_input_p -> Close ();
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, "Blast service failed to open temp file \"%s\" for query \"%s\"", ebt_input_p -> GetFilename (), sequence_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, "Blast service failed to allocate temp file buffer\"%s\" for query \"%s\"", ebt_input_p -> GetFilename (), sequence_s);
						}
				}
			else
				{
					/* try to get the input file */
					if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_FILE, &value, true))
						{
							filename_s = value.st_string_value_s;
						}
				}
		}

	if (filename_s)
		{
			if ((this ->*ebt_arg_callback) ("-query"))
				{
					if ((this ->*ebt_arg_callback) (filename_s))
						{
							success_flag = true;
						}
				}
		}


	/* Query Location */
	if (success_flag)
		{
			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true))
				{
					SharedType to;

					memset (&to, 0, sizeof (SharedType));
					if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &value, true))
						{
							ByteBuffer *buffer_p = AllocateByteBuffer (1024);
							success_flag = false;

							if (buffer_p)
								{
									if (AppendStringsToByteBuffer (buffer_p, value.st_string_value_s, "-", to.st_string_value_s, NULL))
										{
											if ((this ->*ebt_arg_callback) (GetByteBufferData (buffer_p)))
												{
													success_flag = true;
												}
										}

									FreeByteBuffer (buffer_p);
								}
						}
				}
		}


	/* Reward */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MATCH_SCORE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_long_value);

					if (value_s)
						{
							if ((this ->*ebt_arg_callback) ("-reward"))
								{
									if ((this ->*ebt_arg_callback) (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Penalty */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MISMATCH_SCORE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_long_value);

					if (value_s)
						{
							if ((this ->*ebt_arg_callback) ("-penalty"))
								{
									if ((this ->*ebt_arg_callback) (value_s))
										{
											success_flag = true;
										}
								}


							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}

	/* Max target sequences */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MAX_SEQUENCES, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if ((this ->*ebt_arg_callback) ("-max_target_seqs"))
								{
									if ((this ->*ebt_arg_callback) (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}

	/* Expect threshold */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_EXPECT_THRESHOLD, &value, true))
				{
					char *value_s = ConvertDoubleToString (value.st_data_value);

					if (value_s)
						{
							if ((this ->*ebt_arg_callback) ("-evalue"))
								{
									if ((this ->*ebt_arg_callback) (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Word Size */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_WORD_SIZE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if ((this ->*ebt_arg_callback) ("-word_size"))
								{
									if ((this ->*ebt_arg_callback) (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Output File */
	if (success_flag)
		{
			char *buffer_p = GetTempFilenameBuffer ();

			success_flag = false;

			if (buffer_p)
				{
					ebt_input_p = TempFile :: GetTempFile (buffer_p, "w");

					if (ebt_output_p)
						{
							if ((this ->*ebt_arg_callback) ("-out"))
								{
									if ((this ->*ebt_arg_callback) (ebt_output_p -> GetFilename ()))
										{
											success_flag = true;
										}
								}


							ebt_output_p -> Close ();
						}
					else
						{
							success_flag = false;
						}
				}
		}

	/* Db */
/*
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_DATABASE_NAME, &value, true))
				{
					success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -db ", value.st_string_value_s, NULL);
				}
		}
*/

	return success_flag;
}


/* need a buffer where the final 6 chars are XXXXXX, see mkstemp */
char *ExternalBlastTool :: GetTempFilenameBuffer ()
{
	char *buffer_s = 0;
	const char FILE_TEMPLATE_S [] = "blast-XXXXXX";
	const size_t working_dir_length = strlen (ebt_working_directory_s);
	const size_t file_template_length = strlen (FILE_TEMPLATE_S);

	size_t size = 1 + working_dir_length;
	bool needs_slash_flag = (* (ebt_working_directory_s + (size - 1)) != GetFileSeparatorChar ());

	if (needs_slash_flag)
		{
			++ size;
		}

	size += file_template_length;

	buffer_s = (char *) AllocMemory (size * sizeof (char));

	if (buffer_s)
		{
			char *buffer_p = buffer_s;

			memcpy (buffer_p, ebt_working_directory_s, working_dir_length * sizeof (char));
			buffer_p +=  working_dir_length * sizeof (char);

			if (needs_slash_flag)
				{
					*buffer_p = GetFileSeparatorChar ();
					++ buffer_p;
				}

			memcpy (buffer_p, FILE_TEMPLATE_S, file_template_length * sizeof (char));
			buffer_p += file_template_length * sizeof (char);
			*buffer_p = '\0';
		}

	return buffer_s;
}
