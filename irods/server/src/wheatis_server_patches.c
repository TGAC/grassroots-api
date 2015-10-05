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
