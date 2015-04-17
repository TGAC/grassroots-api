#include <cstdlib>

#include <syslog.h>

#include "blast_tool.hpp"

#include "io_utils.h"

#include "byte_buffer.h"
#include "string_utils.h"


BlastTool *CreateBlastTool ()
{
	/**
	 * In the future, this would query the system to determine which type
	 * of blast tool to use, probably set by an admin. 
	 */
	return new QueuedBlastTool;
}


void FreeBlastTool (BlastTool *tool_p)
{
	delete tool_p;
}



BlastTool :: BlastTool ()
{
	bt_running_flag = false;
	bt_temp_input_filename_s = 0;
}

BlastTool :: ~BlastTool ()
{	
	std :: vector <char *> :: size_type i;
	
	for (i = bt_allocated_args.size (); i > 0; -- i)
		{
			char *data_s = bt_allocated_args.back ();
			
			bt_allocated_args.pop_back ();
			
			free (data_s);
		}
}




/**
 * Convert the iRods arguments into an argc, argv set 
 */
/* 
  makeblastdb [-h] [-help] [-in input_file] [-input_type type]
    -dbtype molecule_type [-title database_title] [-parse_seqids]
    [-hash_index] [-mask_data mask_data_files] [-mask_id mask_algo_ids]
    [-mask_desc mask_algo_descriptions] [-gi_mask]
    [-gi_mask_name gi_based_mask_names] [-out database_name]
    [-max_file_sz number_of_bytes] [-taxid TaxID] [-taxid_map TaxIDMapFile]
    [-logfile File_Name] [-version]

DESCRIPTION
   Application to create BLAST databases, version 2.2.29+

REQUIRED ARGUMENTS
 -dbtype <String, `nucl', `prot'>
   Molecule type of target db

OPTIONAL ARGUMENTS
 -h
   Print USAGE and DESCRIPTION;  ignore all other parameters
 -help
   Print USAGE, DESCRIPTION and ARGUMENTS; ignore all other parameters
 -version
   Print version number;  ignore other arguments

 *** Input options
 -in <File_In>
   Input file/database name
   Default = `-'
 -input_type <String, `asn1_bin', `asn1_txt', `blastdb', `fasta'>
   Type of the data specified in input_file
   Default = `fasta'

 *** Configuration options
 -title <String>
   Title for BLAST database
   Default = input file name provided to -in argument
 -parse_seqids
   Option to parse seqid for FASTA input if set, for all other input types
   seqids are parsed automatically
 -hash_index
   Create index of sequence hash values.

 *** Sequence masking options
 -mask_data <String>
   Comma-separated list of input files containing masking data as produced by
   NCBI masking applications (e.g. dustmasker, segmasker, windowmasker)
 -mask_id <String>
   Comma-separated list of strings to uniquely identify the masking algorithm
    * Requires:  mask_data
    * Incompatible with:  gi_mask
 -mask_desc <String>
   Comma-separated list of free form strings to describe the masking algorithm
   details
    * Requires:  mask_id
 -gi_mask
   Create GI indexed masking data.
    * Requires:  parse_seqids
    * Incompatible with:  mask_id
 -gi_mask_name <String>
   Comma-separated list of masking data output files.
    * Requires:  mask_data, gi_mask

 *** Output options
 -out <String>
   Name of BLAST database to be created
   Default = input file name provided to -in argumentRequired if multiple
   file(s)/database(s) are provided as input
 -max_file_sz <String>
   Maximum file size for BLAST database files
   Default = `1GB'

 *** Taxonomy options
 -taxid <Integer, >=0>
   Taxonomy ID to assign to all sequences
    * Incompatible with:  taxid_map
 -taxid_map <File_In>
   Text file mapping sequence IDs to taxonomy IDs.
   Format:<SequenceId> <TaxonomyId><newline>
    * Requires:  parse_seqids
    * Incompatible with:  taxid
 -logfile <File_Out>
   File to which the program log should be redirected
*/



bool RunBlast (BlastTool *tool_p)
{
	bool success_flag = tool_p -> Run ();
		
	return success_flag;	
}


void BlastTool :: AddArgument (char *arg_s, bool newly_allocated_flag)
{
	bt_command_line_args.push_back (arg_s);	
	
	if (newly_allocated_flag)
		{
			bt_allocated_args.push_back (arg_s);			
		}
}



bool ForkedBlastTool :: Run ()
{
	bool success_flag = true;
	
	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: ForkedBlastTool :: Run", __FILE__, __LINE__);		
	PostRun ();
	
	return success_flag;
}


bool InlineBlastTool :: Run ()
{
	bool success_flag = true;

	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: InlineBlastTool :: Run", __FILE__, __LINE__);		
	PostRun ();

	return success_flag;
}


bool ThreadedBlastTool :: Run ()
{
	bool success_flag = true;
	
	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: ThreadedBlastTool :: Run", __FILE__, __LINE__);	
	PostRun ();

	return success_flag;
}


bool QueuedBlastTool :: Run ()
{
	bool success_flag = true;
	
	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: QueuedBlastTool :: Run", __FILE__, __LINE__);	
	PostRun ();

	return success_flag;
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

bool QueuedBlastTool :: ParseParameters (ParameterSet *params_p)
{
	bool success_flag = false;
	ByteBuffer *args_buffer_p = AllocateByteBuffer (1024);

	if (args_buffer_p)
		{
			SharedType value;

			memset (&value, 0, sizeof (SharedType));

			/* Input query */
			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_QUERY, &value, true))
				{
					char *sequence_s = value.st_string_value_s;

					if (!IsStringEmpty (sequence_s))
						{
							char name_s [L_tmpnam];

							if ((bt_temp_input_filename_s = tmpnam (name_s)) != NULL)
								{

								}
						}
					else
						{
							/* try to get the input file */
							if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_FILE, &value, true))
								{

								}
						}
				}


			/* Query Location */
			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true))
				{
					SharedType to;

					memset (&to, 0, sizeof (SharedType));
					if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &value, true))
						{
							success_flag = AppendStringsToByteBuffer (args_buffer_p, value.st_string_value_s, "-", to.st_string_value_s, NULL);
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
									success_flag = AppendStringsToByteBuffer (args_buffer_p, " -reward ", value_s, NULL);

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
									success_flag = AppendStringsToByteBuffer (args_buffer_p, " -penalty ", value_s, NULL);

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
									success_flag = AppendStringsToByteBuffer (args_buffer_p, " -max_target_seqs ", value_s, NULL);

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
									success_flag = AppendStringsToByteBuffer (args_buffer_p, " -evalue ", value_s, NULL);

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
									success_flag = AppendStringsToByteBuffer (args_buffer_p, " -word_size ", value_s, NULL);

									FreeCopiedString (value_s);
								}		/* if (value_s) */
						}
				}

		}		/* if (args_buffer_p) */

	return success_flag;
}


void BlastTool :: PreRun ()
{
	bt_running_flag = true;
	PringArgsToLog ();	
}


void BlastTool :: PostRun ()
{
	bt_running_flag = false;
}


void BlastTool :: PringArgsToLog ()
{
	std :: vector <char *> :: size_type i;
	const std :: vector <char *> :: size_type num_args = bt_command_line_args.size ();
	
	for (i = 0; i < num_args; ++ i)
		{
			WriteToLog (NULL, LOG_INFO, "%s %d: arg [%d]=\"%s\"\n", __FILE__, __LINE__, (int) i, bt_command_line_args [i]);	
		}
}





