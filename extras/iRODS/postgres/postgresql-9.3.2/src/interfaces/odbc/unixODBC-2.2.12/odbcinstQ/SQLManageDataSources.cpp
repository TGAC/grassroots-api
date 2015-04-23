/**************************************************
 * SQLManageDataSources
 *
 **************************************************
 *
 * This library should only be used by odbcinst!
 *
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 26.OCT.01
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * Nick Gorham      - nick@easysoft.com
 **************************************************/

#include <odbcinstext.h>

#include <qdir.h>
#include <qwidget.h>
#if QT_VERSION<300
#include <qnamespace.h>
#endif

#include "CODBCConfig.h"
#include "CODBCCreate.h"

static BOOL SQLManage( HWND hWnd )
{
    //
    // This will allow us to call this from a non QT app
    // 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ", NULL };

        static QApplication a( argc, argv );
    }

    QWidget *pwidget = (QWidget*)hWnd;

#if QT_VERSION<300
	CODBCConfig odbcconfig( pwidget, "ODBCConfig", Qt::WType_Modal );
#else
	CODBCConfig odbcconfig( pwidget, "ODBCConfig", Qt::WType_Dialog | Qt::WShowModal );
#endif
	odbcconfig.exec();

    return true;
}

static BOOL SQLCreate( HWND hWnd, LPCSTR dsn )
{
    //
    // This will allow us to call this from a non QT app
    // 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ", NULL };

        static QApplication a( argc, argv );
    }

    QWidget *pwidget = (QWidget*)hWnd;

#if QT_VERSION<300
	CODBCCreate odbccreate( pwidget, "ODBCCreate" );
#else
	CODBCCreate odbccreate( pwidget, "ODBCCreate" );
#endif
	odbccreate.setDsn( dsn );
	
	odbccreate.exec();

    return odbccreate.getRetCode();
}


#ifdef __cplusplus
extern "C" {
#endif

BOOL QTSQLManageDataSources( HWND hWnd )
{
    return SQLManage( hWnd );
}

BOOL QTSQLCreateDataSources( HWND hWnd, LPCSTR lpszDS )
{
    return SQLCreate( hWnd, lpszDS );
}

#ifdef __cplusplus
};
#endif

