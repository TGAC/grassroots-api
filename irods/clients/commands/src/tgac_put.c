/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* 
 * iput - The irods put utility
*/

#include "rodsClient.h"
#include "parseCommandLine.h"
#include "rodsPath.h"
#include "putUtil.h"
#include "rcMisc.h"
#include "lsUtil.h"
#include "specificQuery.h"
#define TGAC_PUT_DEBUG	(1)


void usage ();

static int CallWheatISServices (rodsEnv *env_p,  rcComm_t *conn_p);


static void GetDataIdForFile (rcComm_t *conn_p, const char * const path_s);
static char *ConcatenateStrings (const char * const first_s, const char * const second_s);


void
printFormatted(char *format, char *args[], int nargs) {
   if (nargs==1) {
      printf(format, args[0]);
   }
   if (nargs==2) {
      printf(format, args[0], args[1]);
   }
   if (nargs==3) {
      printf(format, args[0], args[1], args[2]);
   }
   if (nargs==4) {
      printf(format, args[0], args[1], args[2], args[3]);
   }
   if (nargs==5) {
      printf(format, args[0], args[1], args[2], args[3], args[4]);
   }
   if (nargs==6) {
      printf(format, args[0], args[1], args[2], args[3], args[4],
                     args[5]);
   }
   if (nargs==7) {
      printf(format, args[0], args[1], args[2], args[3], args[4],
                     args[5], args[6]);
   }
   if (nargs==8) {
      printf(format, args[0], args[1], args[2], args[3], args[4],
                     args[5], args[6], args[7]);
   }
   if (nargs==9) {
      printf(format, args[0], args[1], args[2], args[3], args[4],
                     args[5], args[6], args[7], args[8]);
   }
   if (nargs==10) {
      printf(format, args[0], args[1], args[2], args[3], args[4],
                     args[5], args[6], args[7], args[8], args[9]);
   }
}

void
printBasicGenQueryOut(genQueryOut_t *genQueryOut, char *format) {
   int i, j;
   if (format==NULL || strlen(format)==0) {
      for (i=0;i<genQueryOut->rowCnt;i++) {
	 if (i>0) printf("----\n");
	 for (j=0;j<genQueryOut->attriCnt;j++) {
	    char *tResult;
	    tResult = genQueryOut->sqlResult[j].value;
	    tResult += i*genQueryOut->sqlResult[j].len;
	    printf("%s\n", tResult);
	 }
      }
   }
   else {
      for (i=0;i<genQueryOut->rowCnt;i++) {
	 char *results[20];
	 for (j=0;j<genQueryOut->attriCnt;j++) {
	    char *tResult;
	    tResult = genQueryOut->sqlResult[j].value;
	    tResult += i*genQueryOut->sqlResult[j].len;
	    results[j]=tResult;
	 }
	 printFormatted(format, results, j);
      }
   }
}



int
main(int argc, char **argv) {
    int status;
    rodsEnv myEnv;
    rErrMsg_t errMsg;
    rcComm_t *conn;
    rodsArguments_t myRodsArgs;
    char *optStr;
    rodsPathInp_t rodsPathInp;
    int reconnFlag;
    

    optStr = "abD:fhIkKn:N:p:Prt:R:QTvVX:Z";
   
    status = parseCmdLineOpt (argc, argv, optStr, 1, &myRodsArgs);

    if (status < 0) {
	printf("use -h for help.\n");
        exit (1);
    }

    if (myRodsArgs.help==True) {
       usage();
       exit(0);
    }

    status = getRodsEnv (&myEnv);
    if (status < 0) {
        rodsLogError (LOG_ERROR, status, "main: getRodsEnv error. ");
        exit (1);
    }

    status = parseCmdLinePath (argc, argv, optind, &myEnv,
      UNKNOWN_FILE_T, UNKNOWN_OBJ_T, 0, &rodsPathInp);

    if (status < 0) {
        rodsLogError (LOG_ERROR, status, "main: parseCmdLinePath error. "); 
	printf("use -h for help.\n");
        exit (1);
    }

    if (myRodsArgs.reconnect == True) {
        reconnFlag = RECONN_TIMEOUT;
    } else {
        reconnFlag = NO_RECONN;
    }

    conn = rcConnect (myEnv.rodsHost, myEnv.rodsPort, myEnv.rodsUserName,
      myEnv.rodsZone, reconnFlag, &errMsg);

    if (conn == NULL) {
        exit (2);
    }
   
    status = clientLogin(conn);
    if (status != 0) {
       rcDisconnect(conn);
        exit (7);
    }

    if (myRodsArgs.progressFlag == True) {
        gGuiProgressCB = (irodsGuiProgressCallbak) iCommandProgStat;
    }

    status = putUtil (&conn, &myEnv, &myRodsArgs, &rodsPathInp);

		/* Has the file been put successfully? */
		printf ("status %d\n", status);
		if (status == 0)
			{
				rodsPathInp_t *path_inp_p = &rodsPathInp;
				
				/* 
				 * Now we run through our list of potential services
				 */
				int i = path_inp_p -> numSrc;
				rodsPath_t *src_p = path_inp_p -> srcPath;
				rodsPath_t *dest_p = path_inp_p -> destPath;
				rodsPath_t *target_p = path_inp_p -> targPath;
			
				for ( ; i > 0; ++ src_p, ++ dest_p, ++ target_p, -- i)
					{
						#if TGAC_PUT_DEBUG > 0
						printf ("i %d\n", i);
						printf ("src in \"%s\" out \"%s\"\n", src_p -> inPath, src_p -> outPath);
						printf ("dest in \"%s\" out \"%s\"\n", dest_p -> inPath, dest_p -> outPath);
						printf ("target in \"%s\" out \"%s\"\n", target_p -> inPath, target_p -> outPath);

						GetDataIdForFile (conn, target_p -> outPath);
						#endif
					}
				 
				 
			}		/* if (status == 0) */

    printErrorStack(conn->rError);

    rcDisconnect(conn);

    if (status < 0) {
	exit (3);
    } else {
        exit(0);
    }

}



static void GetDataIdForFile (rcComm_t *conn_p, const char * const path_s)
{
	const char * const QUERY_S = "SELECT DATA_FILEMETA_OBJ_ID, DATA_NAME WHERE DATA_NAME = '";

	char *sql_start_s = ConcatenateStrings (QUERY_S, path_s);
	
	if (sql_start_s)
		{
			char *sql_s = ConcatenateStrings (sql_start_s, "\'");
		
			if (sql_s)
				{
			specificQueryInp_t in_query;
			int status, i;
			genQueryOut_t *out_query_p = NULL;
		
			memset (&in_query, 0, sizeof (specificQueryInp_t));
			in_query.maxRows = MAX_SQL_ROWS;
			in_query.continueInx =0;
			in_query.sql = sql_s;
		
		
			printf ("sql: \"%s\"\n", sql_s);
		
			status = rcSpecificQuery (conn_p, &in_query, &out_query_p);
		
			if (status == CAT_NO_ROWS_FOUND) 
				{
					printf ("No rows found\n"); 
				}
			else if (status < 0 ) 
				{
				 	printf ("error\n");
				}
			else
				{
					printBasicGenQueryOut (out_query_p, "result: \"%s\" \"%s\"\n");
				}
		
			free (sql_s);
				
				}

			free (sql_start_s);		
		}
}



static char *ConcatenateStrings (const char * const first_s, const char * const second_s)
{
	const size_t len1 = (first_s != NULL) ? strlen (first_s) : 0;
	const size_t len2 = (second_s != NULL) ? strlen (second_s) : 0;

	char *result_s = (char *) malloc (sizeof (char) * (len1 + len2 + 1));

	if (result_s)
		{
			if (len1 > 0)
				{
					strncpy (result_s, first_s, len1);
				}

			if (len2 > 0)
				{
					strcpy (result_s + len1, second_s);
				}

			* (result_s + len1 + len2) = '\0';
		}

	return result_s;
}



void 
usage ()
{
   const char *msgs[]={
"Usage : iput [-abfIkKPQrtTUvV] [-D dataType] [-N numThreads] [-n replNum]",
"             [-p physicalPath] [-R resource] [-X restartFile] [--link]", 
"             [--lfrestart lfRestartFile] [--retries count] [--wlock]",
"             [--purgec]",
"               localSrcFile|localSrcDir ...  destDataObj|destColl",
"Usage : iput [-abfIkKPQtTUvV] [-D dataType] [-N numThreads] [-n replNum] ",
"             [-p physicalPath] [-R resource] [-X restartFile] [--link]",
"             [--lfrestart lfRestartFile] [--retries count] [--wlock]",
"             [--purgec]",
"               localSrcFile",
" ",
"Store a file into iRODS.  If the destination data-object or collection are",
"not provided, the current iRODS directory and the input file name are used.",
" ",
"The -X option specifies that the restart option is on and the operation",
"is restartable. The restartFile input specifies a local file that contains",
"the restart information. If the restartFile does not exist, it will be created", 
"and used for recording subsequent restart information. If it exists and is not", 
"empty, the restart information contained in this file will be used to restart", 
"the operation. Note that the operation is not restarted automatically", 
"when it failed. Another iput -X run must be made to continue from where", 
"it left off using the restart information. But the -X option can be used in", 
"conjunction with the --retries option to automatically restart the operation",
"in case of failure. Also note that the restart operation only works for",
"uploading directories and the path input must be identical to the one",
"that generated the restart file.",
" ",
"The --lfrestart option specifies that the large file restart option is on",
"and the lfRestartFile input specifies a local file that contains the restart",
"information. Currently, only files larger than 32 Mbytes will be restarted.",
"The --lfrestart option can be used together with the -X option to do large",
"file transfer restart as part of the overall directory upload restart.",
" ",
"If the -f option is used to overwrite an existing data-object, the copy",
"in the resource specified by the -R option will be picked if it exists.",
"Otherwise, one of the copies in the other resources will be picked for the",
"overwrite. Note that a copy will not be made in the specified resource",
"if a copy in the specified resource does not already exist. The irepl",
"command should be used to make a replica of an existing copy.", 
" ",
"The -I option specifies the redirection of the connection so that it can",
"be connected directly to the resource server. This option can improve",
"the performance of uploading a large number of small (<32 Mbytes) files.", 
"This option is only effective if the source is a directory and the -f ",
"option is not used.", 
" ",
"The -Q option specifies the use of the RBUDP transfer mechanism which uses",
"the UDP protocol for data transfer. The UDP protocol is very efficient",
"if the network is very robust with few packet losses. Two environment",
"variables - rbudpSendRate and rbudpPackSize are used to tune the RBUDP",
"data transfer. rbudpSendRate is used to throttle the send rate in ",
"kbits/sec. The default rbudpSendRate is 600,000. rbudpPackSize is used",
"to set the packet size. The default rbudpPackSize is 8192. The -V option", 
"can be used to show the loss rate of the transfer. If the loss rate is", 
"more than a few %, the rbudpSendRate should be reduced.",
" ",
"The -T option will renew the socket connection between the client and ",
"server after 10 minutes of connection. This gets around the problem of",
"sockets getting timed out by the server firewall as reported by some users.",
" ",
"The -b option specifies the bulk upload operation which can do up to 50 uploads",
"at a time to reduce overhead. If the -b option is specified with the -f option",
"to overwrite existing files, the operation will work only if there is no",
"existing copy at all or if there is an existing copy in the target resource.",
"The operation will fail if there are existing copies but not in the",
"target resource because this type of operation requires a replication",
"operation and bulk replication has not been implemented yet.",
"The bulk option does work for mounted collections which may represent the",
"quickest way to upload a large number of small files.",
" ",
"Options are:",
" -a  all - update all existing copies",
" -b  bulk upload to reduce overhead",
" -D  dataType - the data type string",
" -f  force - write data-object even it exists already; overwrite it",
" -I  redirect connection - redirect the connection to connect directly",
"       to the resource server.",
" -k  checksum - calculate a checksum on the data",
" -K  verify checksum - calculate and verify the checksum on the data",
" --link - ignore symlink.",
" -n  replNum  - the replica to be replaced, typically not needed",
" -N  numThreads - the number of threads to use for the transfer. A value of",
"       0 means no threading. By default (-N option not used) the server ",
"       decides the number of threads to use.",
" --purgec  Purge the staged cache copy after uploading an object to a",
"     COMPOUND resource",
" -p  physicalPath - the physical path of the uploaded file on the server ",
" -P  output the progress of the upload.",
" -Q  use RBUDP (datagram) protocol for the data transfer",
" -R  resource - specifies the resource to store to. This can also be specified",
"     in your environment or via a rule set up by the administrator.",
" -r  recursive - store the whole subdirectory",
" -T  renew socket connection after 10 minutes",
" -t  ticket - ticket (string) to use for ticket-based access",
" -v  verbose",
" -V  Very verbose",
" -X  restartFile - specifies that the restart option is on and the restartFile",
"       input specifies a local file that contains the restart information.",
"--retries count - Retry the iput in case of error. The 'count' input",
"       specifies the number of times to retry. It must be used with the", 
"       -X option",
" --lfrestart lfRestartFile - specifies that the large file restart option is",
"       on and the lfRestartFile input specifies a local file that contains",
"       the restart information.",
" --wlock - use advisory write (exclusive) lock for the upload",
" --hash md5|sha256 - use the specified file hash type (checksum) instead of",
""};
   const char *msgs2[]={
"       See https://www.irods.org/index.php/File_Hashes for more.",
" -h  this help",
""};
   int i;
#ifdef SHA256_FILE_HASH
   int use_sha256 = PREFER_SHA256_FILE_HASH;
#endif
   for (i=0;;i++) {
      if (strlen(msgs[i])==0) break;
      printf("%s\n",msgs[i]);
   }
#ifdef SHA256_FILE_HASH
   if (use_sha256==1) {
      printf("       the default (which is sha256 for this 'iput' build).\n");
   }
   else {
      printf("       the default (which is md5 for this 'iput' build).\n");
   }
#else
      printf("       the default.  This 'iput' is built without sha256 support\n");
      printf("       so the '--hash' option is not available.\n");
#endif
   for (i=0;;i++) {
      if (strlen(msgs2[i])==0) break;
      printf("%s\n",msgs2[i]);
   }
   printReleaseInfo("iput");
}



static int CallWheatISServices (rodsEnv *env_p,  rcComm_t *conn_p)
{
	int result = 0;
	
	return result;
}


