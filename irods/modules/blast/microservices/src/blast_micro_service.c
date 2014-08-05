#include <syslog.h>

#include "reDataObjOpr.h"

#include "blast_micro_service.h"
#include "blast_tool.hpp"
#include "io_utils.h"

/*
micro service call
*/


int BlastSequenceData (msParam_t *in_p, msParam_t *out_p, ruleExecInfo_t *rei)
{
	int result = 0;
	BlastTool *blast_p = NULL;
	
	/* The	next line is needed	for	loop back	testing	using	the irule	-i option */
	RE_TEST_MACRO	("				Calling	BlastSequenceData");	
	
	/* Get the input parameters */
	if (strcmp (in_p -> type, STR_MS_T) == 0)
		{
			char *input_param_s = (char *) (in_p -> inOutStruct);

			if (input_param_s)
				{
					blast_p = CreateBlastTool ();

					if (blast_p)
						{
							bool success_flag = false;
							msParamArray_t params;
							memset (&params, 0, sizeof (msParamArray_t)); 

							/* is it a list of key-value pairs? */
							if (strstr (input_param_s, MS_INP_SEP_STR))
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: \"%s\" contains %s", __FILE__, __LINE__, input_param_s, MS_INP_SEP_STR);	
									success_flag = ConvertKeyValueArgument (blast_p, in_p);									
								}
							else
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: \"%s\" does not contain %s", __FILE__, __LINE__, input_param_s, MS_INP_SEP_STR);	

									/* it appears to be a single argument so assume it is the 
									 * input filename.
									 */									
									addMsParamToArray (&params, in_p -> label, STR_MS_T, input_param_s, NULL, 1); 			
									
									success_flag = ConvertArgumentsArray (blast_p, &params);
								}
							
							
							if (success_flag)
								{
									bool result = RunBlast (blast_p);
								}
							else
								{
									WriteToLog (NULL, LOG_INFO, "%s %d: BlastSequenceData :: Failed to convert input parameter \"%s\"", __FILE__, __LINE__, input_param_s);	
								}
							
							

							FreeBlastTool (blast_p);
						}		/* if (blast_p) */	
					else
						{
							
						}
					
					
				}		/* if (input_param_s) */
		
		}		/* if (strcmp (in_p -> type, STR_MS_T) == 0) */
	
			


	return result;
}


//static void do ()
//{
//		/* Get the input sequence stream */
//	if (msiDataObjOpen	(in_p, out_p, rei) > 0)	
//		{
//			/* Now we pass the data to Blast */			
//			
//			if (msiDataObjClose	(in_p, out_p, rei) <= 0)	
//				{
//					/* Error closing object */
//			
//				}		/* if (msiDataObjClose	(in_p, out_p, rule_p) > 0) */
//										
//		}		/* if (msiDataObjOpen	(in_p, out_p, rule_p) > 0) */
//	else	
//		{
//			/* Error opening object */
//		}
//}
