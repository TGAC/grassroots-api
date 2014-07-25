#include "blast_tool.cpp"


typedef struct 
{
	int cla_num_args;
	char *cla_args_p [];
} CommandLineArgs;


CommandLineArgs *Cr




int CreateMakeBlastDbApplication (msParamArray_t *params_p)
{
	CMakeBlastDBApp *app_p = new CMakeBlastDBApp;
	
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

int ConvertArguments (char **argv_ss, msParamArray_t *params_p)
{
	int num_converted_args = -1;
	
	if (params_p)
		{
			if (argv_ss)
				{
					int i;
					const msParam_t **param_pp = params_p -> msParam;
					vector <char *> args;
			
			

					for (i = params_p -> len; i > 0; -- i, ++ param_pp)
						{
							const msParam_t *param_p = *param_pp;
							
							if (param_p -> label)
								{
									size_t key_size = 2 + strlen (param_p -> label);
									
									char *key_s = (char *) malloc (key_size * sizeof (char));
									
									if (key_s)
										{
											*key_s = '-';
											strcpy (key_s + 1, param_p -> label);
											
										}		/* if (key_s) */
									
								}
								
							
							
							
							if (strcmp (param_p -> type, STR_MS_T) == 0)
								{
									
								}
							else if (strcmp (param_p -> type, INT_MS_T) == 0)
								{
									
								}
							else if (strcmp (param_p -> type, BOOL_MS_T) == 0)
								{
									args.push_back ();
								}
						
						}		/* for (i = params_p -> len; i > 0; -- i, ++ param_pp) */
					
				}		/* if (argv_ss) */
				
		}		/* if (params_p) */


	return num_converted_args;
}
