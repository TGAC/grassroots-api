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
	ebt_working_directory_s = working_directory_s;

	if (AddArg ("-db"))
		{
			if (AddArg (name_s))
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
					char *buffer_p = GetTempFilenameBuffer ("blast-input");

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
									PrintErrors (STM_LEVEL_WARNING, "Blast service failed to open temp file for query \"%s\"", sequence_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, "Blast service failed to allocate temp file buffer for query \"%s\"", sequence_s);
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
			if (AddArg ("-query"))
				{
					if (AddArg (filename_s))
						{
							success_flag = true;
						}
				}
		}

	/* Output File */
	if (success_flag)
		{
			char *buffer_p = GetTempFilenameBuffer ("blast-output");

			success_flag = false;

			if (buffer_p)
				{
					ebt_output_p = TempFile :: GetTempFile (buffer_p, "w");

					if (ebt_output_p)
						{
							if (AddArg ("-out"))
								{
									if (AddArg (ebt_output_p -> GetFilename ()))
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
	if (success_flag)
		{
			success_flag = false;

			if (bt_job_p -> sj_name_s)
				{
					if (AddArg ("-db"))
						{
							if (AddArg (bt_job_p -> sj_name_s))
								{
									success_flag = true;
								}
						}
				}
		}

	//return success_flag;

	/* Query Location */
	if (success_flag)
		{
			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true))
				{
					if (value.st_string_value_s)
						{
							SharedType to;

							memset (&to, 0, sizeof (SharedType));

							if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &to, true))
								{
									if (to.st_string_value_s)
										{
											ByteBuffer *buffer_p = AllocateByteBuffer (1024);
											success_flag = false;

											if (buffer_p)
												{
													if (AppendStringsToByteBuffer (buffer_p, value.st_string_value_s, "-", to.st_string_value_s, NULL))
														{
															if (AddArg (GetByteBufferData (buffer_p)))
																{
																	success_flag = true;
																}
														}

													FreeByteBuffer (buffer_p);
												}
										}
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
							if (AddArg ("-reward"))
								{
									if (AddArg (value_s))
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
							if (AddArg ("-penalty"))
								{
									if (AddArg (value_s))
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
							if (AddArg ("-max_target_seqs"))
								{
									if (AddArg (value_s))
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
					if (value.st_ulong_value > 0)
						{
							char *value_s = ConvertIntegerToString (value.st_ulong_value);

							if (value_s)
								{
									if (AddArg ("-evalue"))
										{
											if (AddArg (value_s))
												{
													success_flag = true;
												}
										}

									FreeCopiedString (value_s);
								}		/* if (value_s) */
						}
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
							if (AddArg ("-word_size"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Output Format */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if (AddArg ("-outfmt"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}

	return success_flag;
}


/* need a buffer where the final 6 chars are XXXXXX, see mkstemp */
char *ExternalBlastTool :: GetTempFilenameBuffer (const char * const prefix_s)
{
	char *buffer_s = 0;
	const char SUFFIX_S [] = "-XXXXXX";
	const size_t working_dir_length = strlen (ebt_working_directory_s);
	const size_t suffix_length = strlen (SUFFIX_S);
	const size_t prefix_length = strlen (prefix_s);

	size_t size = 1 + working_dir_length + prefix_length + suffix_length;
	bool needs_slash_flag = (* (ebt_working_directory_s + (size - 1)) != GetFileSeparatorChar ());

	if (needs_slash_flag)
		{
			++ size;
		}

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

			memcpy (buffer_p, prefix_s, prefix_length * sizeof (char));
			buffer_p +=  prefix_length * sizeof (char);

			memcpy (buffer_p, SUFFIX_S, suffix_length * sizeof (char));
			buffer_p += suffix_length * sizeof (char);
			*buffer_p = '\0';
		}

	return buffer_s;
}
