/**************************************************
 * isql
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlext.h>

#ifdef HAVE_STRTOL

char *szSyntax =
"\n" \
"**********************************************\n" \
"* unixODBC - isql                            *\n" \
"**********************************************\n" \
"* Syntax                                     *\n" \
"*                                            *\n" \
"*      isql DSN [UID [PWD]] [options]        *\n" \
"*                                            *\n" \
"* Options                                    *\n" \
"*                                            *\n" \
"* -b         batch.(no prompting etc)        *\n" \
"* -dx        delimit columns with x          *\n" \
"* -x0xXX     delimit columns with XX, where  *\n" \
"*            x is in hex, ie 0x09 is tab     *\n" \
"* -w         wrap results in an HTML table   *\n" \
"* -c         column names on first row.      *\n" \
"*            (only used when -d)             *\n" \
"* -mn        limit column display width to n *\n" \
"* -v         verbose.                        *\n" \
"* -lx        set locale to x                 *\n" \
"* --version  version                         *\n" \
"*                                            *\n" \
"* Commands                                   *\n" \
"*                                            *\n" \
"* help - list tables                         *\n" \
"* help table - list columns in table         *\n" \
"* help help - list all help options          *\n" \
"*                                            *\n" \
"* Examples                                   *\n" \
"*                                            *\n" \
"*      isql WebDB MyID MyPWD -w < My.sql     *\n" \
"*                                            *\n" \
"*      Each line in My.sql must contain      *\n" \
"*      exactly 1 SQL command except for the  *\n" \
"*      last line which must be blank.        *\n" \
"*                                            *\n" \
"* Please visit;                              *\n" \
"*                                            *\n" \
"*      http://www.unixodbc.org               *\n" \
"*      pharvey@codebydesign.com              *\n" \
"*      nick@easysoft.com                     *\n" \
"**********************************************\n\n";

#else

char *szSyntax =
"\n" \
"**********************************************\n" \
"* unixODBC - isql                            *\n" \
"**********************************************\n" \
"* Syntax                                     *\n" \
"*                                            *\n" \
"*      isql DSN [UID [PWD]] [options]        *\n" \
"*                                            *\n" \
"* Options                                    *\n" \
"*                                            *\n" \
"* -b         batch.(no prompting etc)        *\n" \
"* -dx        delimit columns with x          *\n" \
"* -x0xXX     delimit columns with XX, where  *\n" \
"*            x is in hex, ie 0x09 is tab     *\n" \
"* -w         wrap results in an HTML table   *\n" \
"* -c         column names on first row.      *\n" \
"*            (only used when -d)             *\n" \
"* -mn        limit column display width to n *\n" \
"* -v         verbose.                        *\n" \
"* --version  version                         *\n" \
"*                                            *\n" \
"* Commands                                   *\n" \
"*                                            *\n" \
"* help - list tables                         *\n" \
"* help table - list columns in table         *\n" \
"* help help - list all help options          *\n" \
"*                                            *\n" \
"* Examples                                   *\n" \
"*                                            *\n" \
"*      isql WebDB MyID MyPWD -w < My.sql     *\n" \
"*                                            *\n" \
"*      Each line in My.sql must contain      *\n" \
"*      exactly 1 SQL command except for the  *\n" \
"*      last line which must be blank.        *\n" \
"*                                            *\n" \
"* Please visit;                              *\n" \
"*                                            *\n" \
"*      http://www.unixodbc.org               *\n" \
"*      pharvey@codebydesign.com              *\n" \
"*      nick@easysoft.com                     *\n" \
"**********************************************\n\n";

#endif

#define MAX_DATA_WIDTH 300

#ifndef max
#define max( a, b ) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min( a, b ) (((a) < (b)) ? (a) : (b))
#endif

static int OpenDatabase( SQLHENV *phEnv, SQLHDBC *phDbc, char *szDSN, char *szUID, char *szPWD );
static int ExecuteSQL( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable );
static int ExecuteHelp( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable );
static int	CloseDatabase( SQLHENV hEnv, SQLHDBC hDbc );

static void WriteHeaderHTMLTable( SQLHSTMT hStmt );
static void WriteHeaderNormal( SQLHSTMT hStmt, SQLCHAR	*szSepLine );
static void WriteHeaderDelimited( SQLHSTMT hStmt, char cDelimiter );
static void WriteBodyHTMLTable( SQLHSTMT hStmt );
static SQLLEN WriteBodyNormal( SQLHSTMT hStmt );
static void WriteBodyDelimited( SQLHSTMT hStmt, char cDelimiter );
static void WriteFooterHTMLTable( SQLHSTMT hStmt );
static void WriteFooterNormal( SQLHSTMT hStmt, SQLCHAR	*szSepLine, SQLLEN nRows );

static int DumpODBCLog( SQLHENV hEnv, SQLHDBC hDbc, SQLHSTMT hStmt );
static int get_args(char *string, char **args, int maxarg);
static void free_args(char **args, int maxarg);
static void output_help(void);



