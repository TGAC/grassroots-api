/**************************************************
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * Nick Gorham      - nick@easysoft.com
 **************************************************/

#include <stdlib.h>
#include <odbcinstext.h>

/*
 * This avoids all sorts of problems with using putenv, we need to check 
 * that drivers can see this as well though....
 */

static int __config_mode = ODBC_BOTH_DSN;

__set_config_mode( int mode )
{
    __config_mode = mode;
}

__get_config_mode()
{
    char *p;

    /* 
     * if the environment var is set then it overrides the flag
     */

	p  = getenv( "ODBCSEARCH" );
	if ( p )
	{
		if ( strcmp( p, "ODBC_SYSTEM_DSN" ) == 0 )
		{
            __config_mode = ODBC_SYSTEM_DSN;
		}
		else if ( strcmp( p, "ODBC_USER_DSN" ) == 0 )
		{
            __config_mode = ODBC_USER_DSN;
		}
		else if ( strcmp( p, "ODBC_BOTH_DSN" ) == 0 )
		{
            __config_mode = ODBC_BOTH_DSN;
		}
	}

    return __config_mode;
}

BOOL SQLSetConfigMode(			UWORD	nConfigMode )
{
    __set_config_mode( nConfigMode );
    return TRUE;
}
