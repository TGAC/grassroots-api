/*
 * irods includes
 */
#include "dataObjGet.h"
#include "msParam.h"
#include "rodsClient.h" 
#include "rods.h"

int PutAndCheckForServices (rcComm_t **connection_pp, rodsEnv *env_p, rodsArguments_t *args_p, rodsPathInp_t *path_inp_p)
{
	int status = putUtil (connection_pp, env_p, args_p, path_inp_p);
	
	/* Has the file been put successfully? */
	if (status == 0)
		{
			/* 
			 * Now we run through our list of potential services
			 */
			int i = path_inp_p -> numSrc;
			rodsPath_t *src_p = path_inp_p -> srcPath;
			rodsPath_t *dest_p = path_inp_p -> destPath;
			rodsPath_t *target_p = path_inp_p -> targPath;
			
			for ( ; i > 0; ++ src_p, ++ dest_p, ++ target_p)
				{
					#ifdef WHEATIS_SERVER_PATCHES_DEBUG 
					printf ("src in \"%s\" out \"%s\"\n", src_p -> inPath, src_p -> outPath);
					printf ("src in \"%s\" out \"%s\"\n", dest_p -> inPath, dest_p -> outPath);
					printf ("src in \"%s\" out \"%s\"\n", target_p -> inPath, target_p -> outPath);
					#endif
				}
			 
		}		/* if (status == 0) */
		

	return status;
}
