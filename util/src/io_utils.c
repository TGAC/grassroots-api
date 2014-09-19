/*
 *
 * These are based upon the examples at
 *
 * http://www.alepho.com/index.php?do=coding&date=20121203
 *
 */

/*
 * Standard c includes
 */
#include <math.h>
#include <stdarg.h>

/*
 * os includes
 */
#include <syslog.h>

#include <linux/magic.h>

#include <sys/statfs.h>

/*
 * irods includes
 */
#include "dataObjGet.h"
#include "msParam.h"
#include "rodsClient.h"
#include "rods.h"

/*
 * our includes
 */
#include "io_utils.h"



/**
 *
 */
char *GetLocalFilename (char * const original_filename_s)
{
	char *local_filename_s = NULL;

	if (GetFileLocation (original_filename_s) != FILE_LOCATION_LOCAL)
		{
			/* Copy the file to the local system */
			local_filename_s = CopyFileToLocalFilesystem (original_filename_s);
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
	char *temp_name_s = (char *) AllocMemoryArray (L_tmpnam, sizeof (char));

	if (temp_name_s)
		{
			tmpnam (temp_name_s);

			/* Copy the remote file to this new temporary one */
			if (CopyToNewFile (filename_s, temp_name_s, NULL))
				{
					return temp_name_s;
				}

			FreeMemory (temp_name_s);
		}		/* if (temp_name_s) */

	return NULL;
}


/*
int DisposeConnection (rcComm_t *conn_p)
{
	return rcDisconnect (conn_p);
}


rcComm_t *GetConnection (rcComm_t **conn_pp, char *username_s, char *rods_zone_s)
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
*/

char *GetIntAsString (int value)
{
	char *buffer_s = NULL;
	bool negative_flag = false;
	size_t i = 1;

	if (value < 0)
		{
			value = -value;
			negative_flag = true;
			++ i;
		}

	if (value == 0)
		{
			++ i;
		}
	else
		{
			double d = ceil (log10 ((double) value));
			i += (size_t) d;
		}

	buffer_s = (char *) AllocMemory (i * sizeof (char));

	if (buffer_s)
		{
			if (negative_flag)
				{
					sprintf (buffer_s, "-%d", value);
				}
			else
				{
					sprintf (buffer_s, "%d", value);
				}

			* (buffer_s + i - 1) = '\0';
		}

	return buffer_s;
}



bool NotifyUser (userInfo_t *user_p, const char * const title_s, msParamArray_t *params_p)
{
	bool success_flag = true;
	/*
	 * depending on the user's preferences, notify them by e.g. email, logfile, etc.
	 */

	 return success_flag;
}


char *GetParamsAsString (const msParamArray_t * const params_p)
{
	char *params_s = NULL;


	return params_s;
}




void WriteToLog (const char *log_ident_s, const int log_level, const char *message_s, ...)
{
	va_list args;

	va_start (args, message_s);


	if (!log_ident_s)
		{
			log_ident_s = "slog";
		}


	/* Open the log and write to console if we couldn't open it */
	openlog (log_ident_s, LOG_PID | LOG_CONS, LOG_USER);

	/* Write the message */
	vsyslog (log_level, message_s, args);

	closelog ();

	va_end (args);
}





int PrintRodsPath (FILE *out_f, const rodsPath_t * const rods_path_p, const char * const description_s)
{
	return fprintf (out_f, "%s: in \"%s\" out \"%s\"\n", description_s, rods_path_p -> inPath, rods_path_p -> outPath);
}
