
#include "blast_tool.hpp"




BlastInterface *CreateBlastTool ()
{
	return new InlineBlastTool;
}


void FreeBlastTool (BlastInterface *interface_p)
{
	delete interface_p;
}



BlastTool :: BlastTool ()
{
}

BlastTool :: ~BlastTool ()
{	
	for (size_type i = bt_allocated_args.size (); i >= 0; -- i)
		{
			char *data_s = bt_allocated_args.back ();
			
			bt_allocated_args.pop_back ();
			
			free (data_s);
		}
}



int CreateMakeBlastDbApplication (msParamArray_t *params_p)
{
	CMakeBlastDBApp *app_p = new (std :: nothrow) CMakeBlastDBApp;
	
	if (app_p)
		{
			
		}
		
	const CArgDescriptions *arg_descriptions_p = app_p -> GetArgDescriptions ();
	
	if (arg_descriptions_p)
		{
			
		}
		
		
	delete app_p;	
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

bool ConvertArguments (BlastInterface *interface_p, msParamArray_t *params_p);
{
	bool success_flag = false;

	/** Get the C++ class from the C struct */
	BlastTool *tool_p = dynamic_cast <BlastTool *> (interface_p);
	
	if (tool_p)
		{
			if (params_p)
				{
					if (interface_p)
						{
							int i;
							const msParam_t **param_pp = params_p -> msParam;
							
							success_flag = true;
							
							for (i = params_p -> len; i > 0; -- i, ++ param_pp)
								{
									const msParam_t *param_p = *param_pp;
									
									if (param_p -> label)
										{
											size_t size = 2 + strlen (param_p -> label);
											
											char *key_s = (char *) malloc (size * sizeof (char));
											
											if (key_s)
												{
													*key_s = '-';
													strcpy (key_s + 1, param_p -> label);


													if (strcmp (param_p -> type, STR_MS_T) == 0)
														{
															tool_p -> AddArgument (key_s, true);
															tool_p -> AddArgument ((char *) (param_p -> inOutStruct), false);
														}
													else if (strcmp (param_p -> type, INT_MS_T) == 0)
														{
															char *value_s = GetIntAsString ((int *) (param_p -> inOutStruct));
															
															if (value_s)
																{
																	tool_p -> AddArgument (key_s, true);
																	tool_p -> AddArgument (value_s, true);
																}
															else
																{
																	/* not enough memory */
																	free (key_s);
																	success_flag = false;

																	/* force exit from loop */
																	i = -1;		
																}
															
														}
													else if (strcmp (param_p -> type, BOOL_MS_T) == 0)
														{
															tool_p -> AddArgument (key_s, true);
														}

												}		/* if (key_s) */
										}
										
								}		/* for (i = params_p -> len; i > 0; -- i, ++ param_pp) */							
							
						}		/* if (interface_p) */
						
				}		/* if (params_p) */
			
		}		/* if (tool_p) */
	
	return success_flag;
}

bool RunBlast (BlastInterface *interface_p)
{
	bool success_flag = false;

	/** Get the C++ class from the C struct */
	BlastTool *tool_p = dynamic_cast <BlastTool *> (interface_p);
	
	if (tool_p)
		{
			success_flag = tool_p -> Run ();
		}		/* if (tool_p) */
		
	return tool_p;	
}


void BlastTool :: AddArgument (char *arg_s, bool newly_allocated_flag)
{
	bt_command_line_args.push_back (value_s);	
	
	if (newly_allocated_flag)
		{
			bt_allocated_args.push_back (value_s);			
		}
}



bool ForkedBlastTool :: Run ()
{
	bool success_flag = true;
	
	WriteToLog (NULL, LOG_INFO, "ForkedBlastTool :: Run");	
}


bool InlineBlastTool :: Run ()
{
	bool success_flag = true;
	
	WriteToLog (NULL, LOG_INFO, "InlineBlastTool :: Run");	
}


bool ThreadedBlastTool :: Run ()
{
	bool success_flag = true;
	
	WriteToLog (NULL, LOG_INFO, "ThreadedBlastTool :: Run");	
}


bool QueuedBlastTool :: Run ()
{
	bool success_flag = true;
	
	WriteToLog (NULL, LOG_INFO, "QueuedBlastTool :: Run");	
}




