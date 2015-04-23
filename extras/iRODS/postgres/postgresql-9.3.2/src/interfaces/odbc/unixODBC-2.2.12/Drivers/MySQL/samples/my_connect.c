/***************************************************************************
                          my_connect.c  -  description
                             -------------------
    begin                : Wed Sep 8 2001
    copyright            : (C) MySQL AB 1995-2002, www.mysql.com
    author               : venu ( venu@mysql.com )
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This is a basic sample to demonstrate how to connect to MySQL server   *
 *  using MySQL ODBC 3.51 driver                                           *
 *                                                                         *
 ***************************************************************************/

#include "my_utility.h" /* MyODBC 3.51 sample utility header */

/********************************************************
* main routine                                          *
*********************************************************/
int main(int argc, char *argv[])
{
  SQLHENV    henv;
  SQLHDBC    hdbc; 
  SQLCHAR    server_name[30];  
  SQLRETURN  rc;
  SQLINTEGER narg;
  
    /*
     *  show the usage string when the user asks for this
    */    
      printf("***********************************************\n");
      printf("usage: my_connect [DSN] [UID] [PWD] \n");  
      printf("***********************************************\n");     

    /*
     * if connection string supplied through arguments, overrite
     * the default one..
    */
    for(narg = 1; narg < argc; narg++)
    {     
      if ( narg == 1 )
        mydsn = argv[1];
      else if ( narg == 2 )
        myuid = argv[2];
      else if ( narg == 3 )
        mypwd = argv[3];      
          
    }   
    
    printf("\n allocating environment handle ...");

    rc = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&henv);
    myenv(henv, rc);
    
    printf("success..\n setting the environment version ...");

    rc = SQLSetEnvAttr(henv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
    myenv(henv, rc);

    printf("success..\n allocating the connection handle ...");

    rc = SQLAllocHandle(SQL_HANDLE_DBC,henv, &hdbc);   
    myenv(henv, rc);
    
    printf("success..\n connecting to server using DSN '%s'...",mydsn);

    rc = SQLConnect(hdbc, mydsn, SQL_NTS, myuid, SQL_NTS,  mypwd, SQL_NTS);
    mycon(hdbc, rc);
          
    printf("success..\n");

    rc = SQLGetInfo(hdbc,SQL_DBMS_NAME,&server_name,40,NULL);
    mycon(hdbc, rc);

    printf(" connection established successfully to server '%s'\n",server_name);
    
    rc = SQLDisconnect(hdbc); 
    mycon(hdbc, rc);
        
    rc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    mycon(hdbc, rc);
      
    rc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
    myenv(henv, rc);

  printf("\nSUCCESS ...\n\n");

  return(0);  
} 



