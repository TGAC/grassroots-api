/***************************************************************************  
 * Copyright (C) 1995-2002 MySQL AB, www.mysql.com                         *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program; if not, write to the Free Software Foundation  *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
****************************************************************************/

/*************************************************************************** 
 * TRANSACT.C                                                              *
 *                                                                         *
 * @description: For processing transactions                               *
 *                                                                         *
 * @author     : MySQL AB(monty@mysql.com, venu@mysql.com)                 *
 * @date       : 2001-Aug-15                                               *
 * @product    : myodbc3                                                   *
 *                                                                         *
****************************************************************************/

/*************************************************************************** 
 * The following ODBC APIs are implemented in this file:                   *
 *                                                                         *
 *   SQLEndTran          (ISO 92)                                          *
 *   SQLTransact         (ODBC, Deprecated)                                *
 *                                                                         *
****************************************************************************/

#include "myodbc3.h"

/*
  @type    : internal
  @purpose : to do the transaction at the connection level
*/

SQLRETURN my_transact(SQLHDBC hdbc,SQLSMALLINT CompletionType)
{
  SQLRETURN result = SQL_SUCCESS;
  DBC FAR *dbc = (DBC FAR *)hdbc;
  const char *query;

  if (dbc && !(dbc->flag & FLAG_NO_TRANSACTIONS))
  {
    switch(CompletionType) {
    case SQL_COMMIT:
      query = "COMMIT";
      break;

    case SQL_ROLLBACK:
      if (!(dbc->mysql.server_capabilities & CLIENT_TRANSACTIONS))
      {
	return set_handle_error(SQL_HANDLE_DBC,hdbc,MYERR_S1C00,
				"Underlying server does not support transactions, upgrade to version >= 3.23.38",0);
      }
      query = "ROLLBACK";
      break;

    default:
      return set_handle_error(SQL_HANDLE_DBC,hdbc,MYERR_S1012,NULL,0);
    }

    pthread_mutex_lock(&dbc->lock);
    if (check_if_server_is_alive(dbc) ||
	mysql_real_query(&dbc->mysql,query,strlen(query)))
    {
      result=set_handle_error(SQL_HANDLE_DBC,hdbc,MYERR_S1000,
			      mysql_error(&dbc->mysql),
                              mysql_errno(&dbc->mysql));
    }
    pthread_mutex_unlock(&dbc->lock);
  }
  return(result);
}

/*
  @type    : ODBC 3.0
  @purpose : requests a commit or rollback operation for all active
	     operations on all statements associated with a connection
*/

SQLRETURN SQL_API
SQLEndTran(SQLSMALLINT HandleType,
	   SQLHANDLE   Handle,
	   SQLSMALLINT CompletionType)
{
  SQLRETURN result=SQL_SUCCESS;
  DBUG_ENTER("SQLEndTran");
  DBUG_PRINT("enter",("type:%d,handle:%x,option:%d",HandleType,Handle,
		      CompletionType));

  switch (HandleType) {
  case SQL_HANDLE_ENV:
    /*
      TODO : The simple logic could be to call my_transact
      with all connection handles in the current environment,
      but as majority of the applications doesn't use this
      option, lets wait for the request or any bug-report
    */
    break;

  case SQL_HANDLE_DBC:
    result=my_transact(Handle,CompletionType);
    break;

  default:
    result=SQL_ERROR;
    set_handle_error(HandleType,Handle,MYERR_S1092,NULL,0);
    break;
  }
  DBUG_RETURN(result);
}

/*
  @type    : ODBC 1.0
  @purpose : Requests a commit or rollback operation for all active
	     operations on all statement handles (hstmts) associated
	     with a connection or for all connections associated
	     with the environment handle, henv
*/

SQLRETURN SQL_API SQLTransact(SQLHENV henv,SQLHDBC hdbc,SQLUSMALLINT fType)
{
  SQLRETURN result=SQL_SUCCESS;
  DBUG_ENTER("SQLTransact");
  DBUG_PRINT("enter",("henv:%x,hdbc:%x,option:%d",henv,hdbc,fType));

  if (hdbc)
    result = my_transact(hdbc,fType);

  DBUG_RETURN(result);
}
