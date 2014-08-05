#include "reDataObjOpr.h"

#include "blast_micro_service.h"
#include "blast_tool.hpp"

/*
micro service call
*/


int BlastSequenceData (msParam_t *in_p, msParam_t *out_p, ruleExecInfo_t *rei)
{
	int result = 0;
	BlastTool *blast_p = NULL;
	
	/* The	next line is needed	for	loop back	testing	using	the irule	-i option */
	RE_TEST_MACRO	("				Calling	BlastSequenceData");	
	
	/* Get the input filename */
	if (strcmp (in_p -> type, STR_MS_T) == 0)
		{
			char *input_filename_s = (char *) (in_p -> inOutStruct);

			if (input_filename_s)
				{
					blast_p = CreateBlastTool ();

					if (blast_p)
						{
							msParamArray_t params;
							memset (&params, 0, sizeof (msParamArray_t)); 
							
							addMsParamToArray (&params, "in", STR_MS_T, input_filename_s, NULL, 1); 
							
							if (ConvertArgumentsArray (blast_p, &params))
								{
									bool result = RunBlast (blast_p);
								}
							else
								{
									
								}

							FreeBlastTool (blast_p);
						}	
					else
						{
							
						}
					
					
				}
		}
	
			
	/* Get the input sequence stream */
	if (msiDataObjOpen	(in_p, out_p, rei) > 0)	
		{
			/* Now we pass the data to Blast */			
			
			

			if (msiDataObjClose	(in_p, out_p, rei) <= 0)	
				{
					/* Error closing object */

				
				}		/* if (msiDataObjClose	(in_p, out_p, rule_p) > 0) */
										
		}		/* if (msiDataObjOpen	(in_p, out_p, rule_p) > 0) */
	else	
		{
			/* Error opening object */
		}


	return result;
}

