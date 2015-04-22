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


ExternalBlastTool :: ExternalBlastTool (Service *service_p)
: BlastTool (service_p)
{
	ebt_buffer_p = AllocateByteBuffer (1024);
	ebt_input_p = 0;
	ebt_output_p = 0;
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
					ebt_input_p = TempFile :: GetTempFile ("w");

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
					/* try to get the input file */
					if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_FILE, &value, true))
						{
							filename_s = value.st_string_value_s;
						}
				}
		}

	if (filename_s)
		{
			success_flag = AppendStringsToByteBuffer (ebt_buffer_p, "-query ", filename_s, NULL);
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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, value.st_string_value_s, "-", to.st_string_value_s, NULL);
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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -reward ", value_s, NULL);

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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -penalty ", value_s, NULL);

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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -max_target_seqs ", value_s, NULL);

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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -evalue ", value_s, NULL);

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
							success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -word_size ", value_s, NULL);

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Output File */
	if (success_flag)
		{
			ebt_output_p = TempFile :: GetTempFile ("r");

			if (ebt_output_p)
				{
					success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " -out ", ebt_output_p -> GetFilename (), NULL);

					ebt_output_p -> Close ();
				}
			else
				{
					success_flag = false;
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

