#include "blast_micro_service.h"

#include "reDataObjOpr.hpp"

/*
micro service call
*/


int BlastSequenceData (msParam_t *in_p, msParam_t *out_p, ruleExecInfo_t *rule_p)
{
	int result = 0;
	
	/* The next 2 lines are needed	for	loop back	testing	using	the irule	-i option */
	ruleExecInfo_t *rei = rule_p;
	RE_TEST_MACRO	("				Calling	BlastSequenceData");	
		
	/* Get the input sequence stream */
	if (msiDataObjOpen	(in_p, out_p, rule_p) > 0)	
		{
			/* Now we pass the data to Blast */			
			
			

			if (msiDataObjClose	(in_p, out_p, rule_p) <= 0)	
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



