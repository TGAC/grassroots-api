#include "reDataObjOpr.h"

#include "blast_micro_service.h"

/*
micro service call
*/


int BlastSequenceData (msParam_t *in_p, msParam_t *out_p, ruleExecInfo_t *rei)
{
	int result = 0;
	
	/* The	next line is needed	for	loop back	testing	using	the irule	-i option */
	RE_TEST_MACRO	("				Calling	BlastSequenceData");	
	
	/* Get the input filename */
	if (strcmp (in_p -> type, STR_MS_T) == 0)
		{
			char *input_filename_s = (char *) (in_p -> inOutStruct);
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



int 

