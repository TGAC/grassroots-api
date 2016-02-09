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


const char * const ExternalBlastTool :: EBT_INPUT_SUFFIX_S = ".input";
const char * const ExternalBlastTool :: EBT_LOG_SUFFIX_S = ".log";


char *ExternalBlastTool :: GetJobFilename (const char * const prefix_s, const char * const suffix_s)
{
	char *job_filename_s = NULL;
	char *job_id_s = GetUUIDAsString (bt_job_p -> sj_id);

	if (job_id_s)
		{
			char *file_stem_s = NULL;

			if (ebt_working_directory_s)
				{
					file_stem_s = MakeFilename (ebt_working_directory_s, job_id_s);
				}
			else
				{
					file_stem_s = job_id_s;
				}

			if (file_stem_s)
				{
					ByteBuffer *buffer_p = AllocateByteBuffer (1024);

					if (buffer_p)
						{
							bool success_flag = false;

							if (prefix_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, prefix_s, file_stem_s, NULL);
								}
							else
								{
									success_flag = AppendStringToByteBuffer (buffer_p, file_stem_s);
								}

							if (success_flag && suffix_s)
								{
									success_flag = AppendStringToByteBuffer (buffer_p, suffix_s);
								}

							if (success_flag)
								{
									job_filename_s = DetachByteBufferData (buffer_p);
								}
							else
								{
									FreeByteBuffer (buffer_p);
								}

						}		/* if (buffer_p) */

					FreeCopiedString (file_stem_s);
				}		/* if (file_stem_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get file stem for \"%s\"", job_id_s);
				}

			FreeUUIDString (job_id_s);
		}		/* if (job_id_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid string for %s", bt_job_p -> sj_name_s);
		}

	return job_filename_s;
}

ExternalBlastTool :: ExternalBlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p, const char *working_directory_s, const char * const blast_program_name_s)
: BlastTool (job_p, name_s, data_p)
{
	ebt_buffer_p = AllocateByteBuffer (1024);

	if (!ebt_buffer_p)
		{
			throw std :: bad_alloc ();
		}
	ebt_output_p = 0;
	ebt_working_directory_s = working_directory_s;
	ebt_blast_s = blast_program_name_s;
	ebt_output_format = BS_DEFAULT_OUTPUT_FORMAT;
}


ExternalBlastTool :: ~ExternalBlastTool ()
{
	if (ebt_output_p)
		{
			delete ebt_output_p;
		}

	FreeByteBuffer (ebt_buffer_p);
}


bool ExternalBlastTool :: AddArgsPair (const char *key_s, const char *value_s)
{
	bool success_flag = false;

	if (AddArg (key_s))
		{
			if (AddArg (value_s))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add value arg for \"%s\"=\"%s\"", key_s, value_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add key arg for \"%s\"=\"%s\"", key_s, value_s);
		}

	return success_flag;
}



bool ExternalBlastTool :: AddArg (const char *arg_s)
{
	bool success_flag = true;

	if (ebt_buffer_p -> bb_current_index > 0)
		{
			success_flag = AppendStringsToByteBuffer (ebt_buffer_p, " ", arg_s, NULL);
		}
	else
		{
			success_flag =  AppendStringToByteBuffer (ebt_buffer_p, arg_s);
		}

	return success_flag;
}


char *ExternalBlastTool :: GetResults (BlastFormatter *formatter_p)
{
	char *results_s = NULL;

	if (ebt_output_p)
		{
			if (formatter_p && (ebt_output_format != BS_DEFAULT_OUTPUT_FORMAT))
				{
					const char *filename_s = ebt_output_p -> GetFilename ();

					if (filename_s)
						{
							results_s = formatter_p -> GetConvertedOutput (filename_s, ebt_output_format);
						}
					else
						{
							char uuid_s [UUID_STRING_BUFFER_SIZE];

							ConvertUUIDToString (bt_job_p -> sj_id, uuid_s);
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to get results filename for \"%s\"", uuid_s);
						}
				}
			else
				{
					if (ebt_output_p -> Open ("r"))
						{
							const char *out_s = ebt_output_p -> GetData ();

							if (out_s)
								{
									results_s = CopyToNewString (out_s, 0, false);

									if (!results_s)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to copy \"%s\"  to results string", out_s);
										}
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to open results file \"%s\"", ebt_output_p -> GetFilename ());
						}
				}
		}		/* if (ebt_output_p) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (bt_job_p -> sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Results output object is NULL for \"%s\"", uuid_s);
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

	memset (&value, 0, sizeof (SharedType));

	if (AddArgsPair ("-task", "blastn"))
		{
			if (AddArgsPair ("-num_alignments", "5"))
				{
					if (bt_job_p -> sj_name_s)
						{
							if (AddArgsPair ("-db", bt_job_p -> sj_name_s))
								{
									/* Reward */
									if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MATCH_SCORE, "-reward", false, false))
										{
											/* Penalty */
											if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MISMATCH_SCORE, "-penalty", false, false))
												{
													/* Expect threshold */
													if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_EXPECT_THRESHOLD, "-evalue", true, false))
														{
															/* Word Size */
															if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_WORD_SIZE, "-word_size", true, false))
																{
																	/* Output Format
																	 * If we have a BlastFormatter then the output is always set to 11 which is ASN and
																	 * from that we can convert into any other format using a BlastFormatter tool
																	 */
																	memset (&value, 0, sizeof (SharedType));

																	if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &value, true))
																		{
																			ebt_output_format = value.st_ulong_value;

																			if (bt_service_data_p -> bsd_formatter_p)
																				{
																					success_flag = AddArgsPair ("-outfmt", BS_DEFAULT_OUTPUT_FORMAT_S);
																				}
																			else
																				{
																					char *value_s = ConvertIntegerToString (ebt_output_format);

																					if (value_s)
																						{
																							success_flag = AddArgsPair ("-outfmt", value_s);
																							FreeCopiedString (value_s);
																						}		/* if (value_s) */
																					else
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert output format \"" UINT32_FMT "\" to string", ebt_output_format);
																						}
																				}

																		}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &value, true)) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output format");
																		}

																	if (success_flag)
																		{
																			/* Query Location */
																			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true))
																				{
																					uint32 from = value.st_ulong_value;

																					if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &value, true))
																						{
																							uint32 to = value.st_ulong_value;

																							if ((from != 0) && (to != 0))
																								{
																									ByteBuffer *buffer_p = AllocateByteBuffer (1024);

																									if (buffer_p)
																										{
																											char *from_s = ConvertIntegerToString (from);

																											if (from_s)
																												{
																													char *to_s = ConvertIntegerToString (to);

																													if (to_s)
																														{
																															if (AppendStringsToByteBuffer (buffer_p, from_s, "-", to_s, NULL))
																																{
																																	const char *query_loc_s = GetByteBufferData (buffer_p);

																																	if (!AddArgsPair ("-query_loc", query_loc_s))
																																		{
																																			success_flag = false;
																																		}
																																}

																															FreeCopiedString (to_s);
																														}		/* if (to_s) */

																													FreeCopiedString (from_s);
																												}		/* if (from_s) */

																											FreeByteBuffer (buffer_p);
																										}		/* if (buffer_p) */

																								}		/* if ((from != 0) && (to != 0)) */

																						}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &to, true)) */

																				}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true)) */

																		}		/*  if (success_flag) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set output format");
																		}

																}		/* if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_WORD_SIZE, "-word_size", true)) */

														}		/* if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_EXPECT_THRESHOLD, "-evalue", true)) */

												}		/* if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MISMATCH_SCORE, "-penalty", false)) */

										}		/* if (AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MATCH_SCORE, "-reward", false)) */

								}		/* if (AddArgsPair ("-db", bt_job_p -> sj_name_s))*/
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set database name");
								}
						}		/* if (bt_job_p -> sj_name_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job name");
						}

				}		/* if (AddArgsPair ("-num_alignments", "5")) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add num_alignments parameter");
				}

		}		/* if (AddArgsPair ("-task", "blastn")) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add task parameter");
		}

	return success_flag;
}


bool ExternalBlastTool :: AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const Tag tag, const char *key_s, const bool unsigned_flag, const bool required_flag)
{
	bool success_flag = !required_flag;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (params_p, tag, &value, true))
		{
			int32 param_value = unsigned_flag ? ((int32) value.st_ulong_value) : (value.st_long_value);
			char *value_s = ConvertIntegerToString (param_value);

			if (value_s)
				{
					success_flag = AddArgsPair (key_s, value_s);
					FreeCopiedString (value_s);
				}		/* if (value_s) */
		}

	return success_flag;
}


bool ExternalBlastTool :: SetUpOutputFile ()
{
	bool success_flag = false;

	ebt_output_p = TempFile :: GetTempFile (ebt_working_directory_s, bt_job_p -> sj_id, BS_OUTPUT_SUFFIX_S);

	if (ebt_output_p)
		{
			const char *output_filename_s = ebt_output_p -> GetFilename ();

			if (output_filename_s)
				{
					success_flag = AddArgsPair ("-out", output_filename_s);
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output filename for \"%s\"", bt_job_p -> sj_name_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get temp output file for \"%s\"", bt_job_p -> sj_name_s);
		}


	return success_flag;
}


bool ExternalBlastTool :: SetInputFilename (const char * const filename_s)
{
	bool success_flag = false;

	if (filename_s)
		{
			success_flag = AddArgsPair ("-query", filename_s);
		}

	return success_flag;
}
