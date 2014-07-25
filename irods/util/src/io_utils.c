/*
 * 
 * These are based upon the examples at 
 * 
 * http://www.alepho.com/index.php?do=coding&date=20121203
 *
 */ 


#include <sys/statfs.h>

#include "dataObjGet.h"
#include "rodsClient.h" 
#include "rods.h"

#include "typedefs.h"



/**
 * 
 */
char *GetLocalFilename (char * const original_filename_s)
{
	char *local_filename_s = NULL;
	
	if (GetFileLocation (original_filename_s) != FILE_LOCATION_LOCAL)
		{
			/* Copy the file to the local system */
			local_copy_filename_s = CopyFileToLocalFilesystem (original_filename_s);
		}
	else
		{
			local_filename_s = original_filename_s;
		}
		
	return local_filename_s;
}


/**
 * Determine whether a file is on the local filesystem or not.
 * 
 * @param filename_s The file to check.
 * @return The type of location of the file or FILE_LOCATION_UNKNOWN if it could
 * not be determined.
 */
FileLocation GetFileLocation (const char * const filename_s)
{
	FileLocation fl = FILE_LOCATION_UNKNOWN;
	struct statfs stat;
	
	/* Clear the data */
	memset (&stat, 0, sizeof (struct statfs));
	
	/* Get the stat for the file */ 
	if (statfs (filename_s, &stat) == 0)
		{
			/* 
			 * As we determine which file systems are remote, more
			 * can be added to the switch statement.
			 */
			switch (stat.f_type)
				{
					case NFS_SUPER_MAGIC:
						fl = FILE_LOCATION_REMOTE;					
						break;
					
					default:
						fl = FILE_LOCATION_LOCAL;
						break;
				}		/* switch (stat.f_type) */
				
		}		/* if (statfs (filename_s, &stat) == 0) */
	else
		{
			/* Couldn't stat the file */
		}
	
	return fl;
}


/**
 * Copy a file .
 * 
 * @param src_filename_s The name of the the source file.
 * @param dest_filename_s The name of the the source file.
 * @param callback_fn The callback_fn to denote progress (currently unused).
 * @return true on success, false on error with errno set to the appropriate value.
 */ 
char *CopyFileToLocalFilesystem (const char * const filename_s)
{
	char *temp_name_s = (char *) calloc (L_tmpnam, sizeof (char));
	
	if (temp_name_s)
		{
			tmpnam (temp_name_s);
			
			/* Copy the remote file to this new temporary one */
			if (CopyToNewFile (filename_s, temp_name_s, NULL))
				{
					return temp_name_s;
				}
			
			free (temp_name_s);
		}		/* if (temp_name_s) */
		
	return NULL;
}



/**
 * Copy the contents of one named file to another.
 * 
 * @param src_filename_s The name of the the source file.
 * @param dest_filename_s The name of the the source file.
 * @param callback_fn The callback_fn to denote progress (currently unused).
 * @return true on success, false on error with errno set to the appropriate value.
 */ 
bool CopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) ())
{
	FILE *in_f = fopen (src_filename_s, "rb");
	bool success_flag = false;
	int saved_errno = 0;

	if (in_f)
		{
			FILE *out_f = fopen (dest_filename_s, "wb");

			if (out_f)
				{
					char buffer [8192];		/* 32 * 256 */
					success_flag = true;

					while (success_flag)
						{
							size_t num_objs = fread (buffer, 32, 256, in_f);

							if (num_objs > 0)
								{
									if (fwrite (buffer, 32, 256, out_f) != num_objs)
										{
											success_flag = false;
											saved_errno = errno;
										}
								}
						}		/* while (success_flag) */

					if (success_flag)
						{
							success_flag = (feof (in_f) != 0) ? true : false;
						}

					fclose (out_f);

				}		/* if (out_f) */

			fclose (in_f);

		}		/* if (in_f) */

	/* In case closing one of the files overwrote it, restore errno */
	if (saved_errno != 0)
		{
			errno = saved_errno;
		}

	return success_flag;
}



int DisposeConnection (rcComm_t *conn_p)
{
	return rcDisconnect (conn_p);
}


rcComm_t *GetConnection (rcComm_t **conn_pp, const char *username_s, const char *rods_zone_s)
{
	rcComm_t *conn_p = NULL;
	rodsEnv myRodsEnv;
	
	int status = getRodsEnv (&myRodsEnv);
	
	if (status >= 0) 
		{
			rErrMsg_t errMsg;
			memset (&errMsg, 0, sizeof (rErrMsg_t));

			conn_p = rcConnect (myRodsEnv.rodsHost, myRodsEnv.rodsPort, username_s, rods_zone_s, 0, &errMsg);
			if (conn_p != NULL) 
				{					
					status = clientLogin (conn_p);

					if (status == 0) 
						{
		
						}
					else
						{
							rcDisconnect(conn);
							exit(1);
						}
			
				}
			else
				{
					fprintf(stderr, "rcConnect error\n");
					exit(1);
				}
		}
	else
		{
			fprintf (stderr, "getRodsEnv error, status = %d\n", status);
			exit(1);
		}

	return conn_p;
}

int ReadFile ()
{
	rcComm_t* conn;
	rodsEnv myRodsEnv;
	dataObjInp_t dataObjOpenInp;
	ruleExecDelInp_t ruleExecDelInp;

	rErrMsg_t errMsg;
	memset(&errMsg, 0, sizeof(rErrMsg_t));

	int status = getRodsEnv(&myRodsEnv);
	if (status < 0) 
		{
			fprintf(stderr, "getRodsEnv() error, status = %d\n", status);
			exit(1);
		}

	conn = rcConnect(myRodsEnv.rodsHost, myRodsEnv.rodsPort, "rods", "tempZone", 0, &errMsg);

	if (conn == NULL) 
		{
			fprintf(stderr, "rcConnect() error\n");
			exit(1);
		}

	status = clientLogin(conn);
	if (status != 0) 
		{
			rcDisconnect(conn);
			exit(1);
		}

	dataObjInp_t dataObjInp;
	bzero(&dataObjInp, sizeof (dataObjInp));
	strncpy(dataObjInp.objPath, "/tempZone/home/rods/hello.txt", MAX_NAME_LEN);
	char locFilePath[MAX_NAME_LEN];
	strncpy(locFilePath, "./hello.txt", MAX_NAME_LEN);
	dataObjInp.dataSize = 0;
	status = rcDataObjGet(conn, &dataObjInp, locFilePath);
	if (status < 0) 
		{
			fprintf(stderr, "error status = %d\n", status);
		}

	return 0;	
	
}


int WriteFile ()
{
	rcComm_t *conn_p;
	rodsEnv myRodsEnv;
	dataObjInp_t dataObjOpenInp;
	ruleExecDelInp_t ruleExecDelInp;

	rErrMsg_t errMsg;
	memset(&errMsg, 0, sizeof(rErrMsg_t));



	int status = getRodsEnv(&myRodsEnv);
	if (status < 0) 
		{
			fprintf (stderr, "getRodsEnv error, status = %d\n", status);
			exit(1);
		}

	conn = rcConnect(myRodsEnv.rodsHost, myRodsEnv.rodsPort, "rods", "tempZone", 0, &errMsg);
	if (conn == NULL) 
		{
			fprintf(stderr, "rcConnect error\n");
			exit(1);
		}

	status = clientLogin(conn);
	if (status != 0) 
		{
			rcDisconnect(conn);
			exit(1);
		}

	dataObjInp_t dataObjInp;
	bzero(&dataObjInp, sizeof (dataObjInp));
	strncpy(dataObjInp.objPath, "/tempZone/home/rods/hello.txt", MAX_NAME_LEN);
	char locFilePath[MAX_NAME_LEN];
	strncpy(locFilePath, "./hello.txt", MAX_NAME_LEN);
	dataObjInp.dataSize = 13;
	status = rcDataObjPut(conn, &dataObjInp, locFilePath);
	
	if (status < 0) 
		{
			fprintf(stderr, "put error, status = %d\n", status);
			}

	return 0;
}

int ReadFile ()
{
	rcComm_t* conn;
	rodsEnv myRodsEnv;
	dataObjInp_t dataObjOpenInp;
	ruleExecDelInp_t ruleExecDelInp;

	rErrMsg_t errMsg;
	memset(&errMsg, 0, sizeof(rErrMsg_t));

	int status = getRodsEnv(&myRodsEnv);
	if (status < 0) 
		{
			fprintf(stderr, "getRodsEnv() error, status = %d\n", status);
			exit(1);
		}

	conn = rcConnect(myRodsEnv.rodsHost, myRodsEnv.rodsPort, "rods", "tempZone", 0, &errMsg);

	if (conn == NULL) 
		{
			fprintf(stderr, "rcConnect() error\n");
			exit(1);
		}

	status = clientLogin(conn);
	if (status != 0) 
		{
			rcDisconnect(conn);
			exit(1);
		}

	dataObjInp_t dataObjInp;
	bzero(&dataObjInp, sizeof (dataObjInp));
	strncpy(dataObjInp.objPath, "/tempZone/home/rods/hello.txt", MAX_NAME_LEN);
	char locFilePath[MAX_NAME_LEN];
	strncpy(locFilePath, "./hello.txt", MAX_NAME_LEN);
	dataObjInp.dataSize = 0;
	status = rcDataObjGet(conn, &dataObjInp, locFilePath);
	if (status < 0) 
		{
			fprintf(stderr, "error status = %d\n", status);
		}

	return 0;	
}
