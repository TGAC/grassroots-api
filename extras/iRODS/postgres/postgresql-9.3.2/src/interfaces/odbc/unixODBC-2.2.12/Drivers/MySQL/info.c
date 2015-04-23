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
 * INFO.C                                                                  *
 *                                                                         *
 * @description: Code for information functions                            *
 *                                                                         *
 * @author     : MySQL AB(monty@mysql.com, venu@mysql.com)                 *
 * @date       : 2001-Aug-15                                               *
 * @product    : myodbc3                                                   *
 *                                                                         *
****************************************************************************/

/*************************************************************************** 
 * The following ODBC APIs are implemented in this file:                   *
 *                                                                         *
 *   SQLGetInfo          (ISO 92)                                          *
 *   SQLGetFunctions     (ISO 92)                                          *
 *   SQLGetTypeInfo      (ISO 92)                                          *
 *                                                                         *
****************************************************************************/

#include "myodbc3.h"
#include <m_ctype.h>

uchar allowed_chars[]= {
  '\307','\374','\351','\342','\344','\340','\345','\347','\352','\353',
  '\350','\357','\356','\354','\304','\305','\311','\346','\306','\364',
  '\366','\362','\373','\371','\377','\326','\334','\341','\355','\363',
  '\372','\361','\321',0};

#define mysql_keywords "UNIQUE,ZEROFILL,UNSIGNED,BIGINT,BLOB,TINYBLOB,MEDIMUMBLOB,LONGBLOB,MEDIUMINT,PROCEDURE,SHOW,LIMIT,DEFAULT,TABLES,REGEXP,RLIKE,KEYS,TINYTEXT,MEDIUMTEXT"

/*
  @type    : ODBC 1.0 API
  @purpose : returns general information about the driver and data
       source associated with a connection
*/

SQLRETURN SQL_API SQLGetInfo(SQLHDBC hdbc, SQLUSMALLINT fInfoType,
           SQLPOINTER rgbInfoValue,
           SQLSMALLINT cbInfoValueMax,
           SQLSMALLINT FAR *pcbInfoValue)
{
  DBC FAR *dbc=(DBC FAR*) hdbc;
  char dummy2[255];
  SQLSMALLINT dummy;
  bool using_322;
  DBUG_ENTER("SQLGetInfo");
  DBUG_PRINT("enter",("fInfoType: %d",fInfoType));

  if (cbInfoValueMax)
    cbInfoValueMax--;

  if (!pcbInfoValue)
    pcbInfoValue=&dummy;

  if (!rgbInfoValue)
  {
    rgbInfoValue=dummy2;
    cbInfoValueMax=sizeof(dummy2)-1;
  }
  switch (fInfoType) {

  case SQL_ACTIVE_ENVIRONMENTS:
    *((SQLUSMALLINT*) rgbInfoValue)=0;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_AGGREGATE_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_AF_ALL | SQL_AF_AVG | SQL_AF_COUNT |
             SQL_AF_DISTINCT | SQL_AF_MAX |
             SQL_AF_MIN | SQL_AF_SUM);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_ALTER_DOMAIN:
    *((SQLUINTEGER*) rgbInfoValue)=0;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_ALTER_TABLE:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_AT_ADD_COLUMN | SQL_AT_DROP_COLUMN;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_ASYNC_MODE:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_AM_NONE;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_BATCH_ROW_COUNT:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_BRC_EXPLICIT;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_BATCH_SUPPORT:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_BS_ROW_COUNT_EXPLICIT;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_BOOKMARK_PERSISTENCE:
    *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_CATALOG_LOCATION:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_CL_START;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_ACCESSIBLE_TABLES:
  case SQL_CATALOG_NAME:
  case SQL_COLUMN_ALIAS:
  case SQL_EXPRESSIONS_IN_ORDERBY:
  case SQL_LIKE_ESCAPE_CLAUSE:
  case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
  case SQL_MULT_RESULT_SETS:
  case SQL_MULTIPLE_ACTIVE_TXN:
  case SQL_OUTER_JOINS:
  case SQL_ORDER_BY_COLUMNS_IN_SELECT:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"Y",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_CATALOG_NAME_SEPARATOR:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,".",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_CATALOG_TERM:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"database",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_CATALOG_USAGE:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_CU_DML_STATEMENTS |
             SQL_CU_TABLE_DEFINITION |
             SQL_CU_INDEX_DEFINITION |
             SQL_CU_PRIVILEGE_DEFINITION);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_COLLATION_SEQ:
    /*
      We have to change this to return to the active collation sequence
      in the server MySQL server, as soon as it supports different
      collation sequences / user.
    */
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_CONCAT_NULL_BEHAVIOR:
    *((SQLUSMALLINT*) rgbInfoValue)= SQL_CB_NULL;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_CONVERT_BIGINT:
  case SQL_CONVERT_BIT:
  case SQL_CONVERT_CHAR:
  case SQL_CONVERT_DATE:
  case SQL_CONVERT_DECIMAL:
  case SQL_CONVERT_DOUBLE:
  case SQL_CONVERT_FLOAT:
  case SQL_CONVERT_INTEGER:
  case SQL_CONVERT_LONGVARCHAR:
  case SQL_CONVERT_NUMERIC:
  case SQL_CONVERT_REAL:
  case SQL_CONVERT_SMALLINT:
  case SQL_CONVERT_TIME:
  case SQL_CONVERT_TIMESTAMP:
  case SQL_CONVERT_TINYINT:
  case SQL_CONVERT_VARCHAR:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_CVT_CHAR | SQL_CVT_NUMERIC |
             SQL_CVT_DECIMAL | SQL_CVT_INTEGER |
             SQL_CVT_SMALLINT | SQL_CVT_FLOAT |
             SQL_CVT_REAL | SQL_CVT_DOUBLE |
             SQL_CVT_VARCHAR | SQL_CVT_LONGVARCHAR |
             SQL_CVT_BIT | SQL_CVT_TINYINT |
             SQL_CVT_BIGINT | SQL_CVT_DATE |
             SQL_CVT_TIME | SQL_CVT_TIMESTAMP);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_CONVERT_BINARY:
  case SQL_CONVERT_VARBINARY:
  case SQL_CONVERT_LONGVARBINARY:
    *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  /* Non supported options..*/
  case SQL_CONVERT_FUNCTIONS: 
  case SQL_CONVERT_WCHAR:
  case SQL_CONVERT_WVARCHAR:
  case SQL_CONVERT_WLONGVARCHAR:
  case SQL_CREATE_ASSERTION:
  case SQL_CREATE_CHARACTER_SET:
  case SQL_CREATE_COLLATION:
  case SQL_CREATE_DOMAIN:
  case SQL_CREATE_SCHEMA:
  case SQL_CREATE_TRANSLATION:
  case SQL_CREATE_VIEW:
  case SQL_DROP_ASSERTION:
  case SQL_DROP_CHARACTER_SET:
  case SQL_DROP_COLLATION:
  case SQL_DROP_DOMAIN:
  case SQL_DROP_SCHEMA:
  case SQL_DROP_TRANSLATION:
  case SQL_DROP_VIEW:
  case SQL_KEYSET_CURSOR_ATTRIBUTES1:
  case SQL_KEYSET_CURSOR_ATTRIBUTES2:
  case SQL_INFO_SCHEMA_VIEWS:
  case SQL_SCHEMA_USAGE:
  case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
  case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
  case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
  case SQL_SQL92_PREDICATES:
  case SQL_SQL92_VALUE_EXPRESSIONS:
  case SQL_SUBQUERIES:
  case SQL_TIMEDATE_ADD_INTERVALS:
  case SQL_TIMEDATE_DIFF_INTERVALS:
  case SQL_UNION:
  case SQL_LOCK_TYPES:
    *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;
  
  /* Un limit, set to default..*/
  case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
  case SQL_MAX_BINARY_LITERAL_LEN:
  case SQL_MAX_CHAR_LITERAL_LEN:
  case SQL_MAX_DRIVER_CONNECTIONS:
  case SQL_MAX_ROW_SIZE:
    *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_MAX_COLUMNS_IN_GROUP_BY:
  case SQL_MAX_COLUMNS_IN_ORDER_BY:
  case SQL_MAX_COLUMNS_IN_SELECT:
  case SQL_MAX_COLUMNS_IN_TABLE:
  case SQL_MAX_CONCURRENT_ACTIVITIES:
  case SQL_MAX_PROCEDURE_NAME_LEN:
  case SQL_MAX_SCHEMA_NAME_LEN:
    *((SQLUSMALLINT*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  

  case SQL_CORRELATION_NAME:
    *((SQLSMALLINT*) rgbInfoValue)= SQL_CN_DIFFERENT;
    *pcbInfoValue=sizeof(SQLSMALLINT);
    break;

  case SQL_CREATE_TABLE:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_CT_CREATE_TABLE |
             SQL_CT_COMMIT_DELETE |
             SQL_CT_LOCAL_TEMPORARY |
             SQL_CT_COLUMN_DEFAULT);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_CURSOR_COMMIT_BEHAVIOR:
  case SQL_CURSOR_ROLLBACK_BEHAVIOR:
    *((SQLUSMALLINT*) rgbInfoValue)= SQL_CB_PRESERVE;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

#ifdef SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY
  case SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_UNSPECIFIED;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;
#endif

#ifdef SQL_CURSOR_SENSITIVITY
  case SQL_CURSOR_SENSITIVITY:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_UNSPECIFIED;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;
#endif

  case SQL_DATA_SOURCE_NAME:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,dbc->dsn,
           cbInfoValueMax) -
         (char*) rgbInfoValue);
    break;

  case SQL_ACCESSIBLE_PROCEDURES:
  case SQL_DATA_SOURCE_READ_ONLY:
  case SQL_DESCRIBE_PARAMETER:
  case SQL_INTEGRITY:
  case SQL_NEED_LONG_DATA_LEN:
  case SQL_PROCEDURES:
  case SQL_ROW_UPDATES:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"N",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_DATABASE_NAME:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,dbc->database,
           cbInfoValueMax)
         - (char*) rgbInfoValue);    
    break;

  case SQL_DATETIME_LITERALS:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_DL_SQL92_DATE |
      SQL_DL_SQL92_TIME |
      SQL_DL_SQL92_TIMESTAMP;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_DBMS_NAME:
  {
    char dbname[256];
    strxmov(dbname,"mysqld-",dbc->mysql.server_version,NullS);
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,dbname,
           cbInfoValueMax) -
         (char*) rgbInfoValue);
  }
  break;

  case SQL_DBMS_VER:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,
           dbc->mysql.server_version,
           cbInfoValueMax) -
         (char*) rgbInfoValue);
    break;

  case SQL_DDL_INDEX:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_DI_CREATE_INDEX | SQL_DI_DROP_INDEX;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_DEFAULT_TXN_ISOLATION:
    *((SQLUINTEGER*) rgbInfoValue)= DEFAULT_TXN_ISOLATION;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_DRIVER_NAME:
#ifndef _UNIX_
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"myodbc3.dll",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
#else
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"libmyodbc3.so",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
#endif /* _UNIX */
    break;

  case SQL_DRIVER_ODBC_VER:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue, SQL_SPEC_STRING,
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_DRIVER_VER:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,DRIVER_VERSION,
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_DROP_TABLE:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_DT_DROP_TABLE |
             SQL_DT_CASCADE |
             SQL_DT_RESTRICT);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_STATIC_CURSOR_ATTRIBUTES1:
  case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
    *((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT |
              SQL_CA1_ABSOLUTE |
              SQL_CA1_RELATIVE |
              SQL_CA1_LOCK_NO_CHANGE |
              SQL_CA1_POS_POSITION |
              SQL_CA1_POS_UPDATE |
              SQL_CA1_POS_DELETE |
              SQL_CA1_POS_REFRESH |
              SQL_CA1_POSITIONED_UPDATE |
              SQL_CA1_POSITIONED_DELETE |
              SQL_CA1_BULK_ADD);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_STATIC_CURSOR_ATTRIBUTES2:
  case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
    *((SQLUINTEGER*) rgbInfoValue) =  (SQL_CA2_MAX_ROWS_SELECT |
               SQL_CA2_MAX_ROWS_INSERT |
               SQL_CA2_MAX_ROWS_DELETE |
               SQL_CA2_MAX_ROWS_UPDATE);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
    if (dbc->flag & FLAG_DYNAMIC_CURSOR)
      *((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT |
          SQL_CA1_ABSOLUTE |
          SQL_CA1_RELATIVE |
          SQL_CA1_LOCK_NO_CHANGE |
          SQL_CA1_POS_POSITION |
          SQL_CA1_POS_UPDATE |
          SQL_CA1_POS_DELETE |
          SQL_CA1_POS_REFRESH |
          SQL_CA1_POSITIONED_UPDATE |
          SQL_CA1_POSITIONED_DELETE |
          SQL_CA1_BULK_ADD);
    else
      *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
    if (dbc->flag & FLAG_DYNAMIC_CURSOR)
      *((SQLUINTEGER*) rgbInfoValue) =  (SQL_CA2_SENSITIVITY_ADDITIONS |
           SQL_CA2_SENSITIVITY_DELETIONS |
           SQL_CA2_SENSITIVITY_UPDATES |
           SQL_CA2_MAX_ROWS_SELECT |
           SQL_CA2_MAX_ROWS_INSERT |
           SQL_CA2_MAX_ROWS_DELETE |
           SQL_CA2_MAX_ROWS_UPDATE |
           SQL_CA2_SIMULATE_TRY_UNIQUE);
    else *((SQLUINTEGER*) rgbInfoValue)=0L;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_FILE_USAGE:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_FILE_NOT_SUPPORTED;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_GETDATA_EXTENSIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_GD_ANY_COLUMN |
             SQL_GD_ANY_ORDER |
             SQL_GD_BOUND);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_GROUP_BY:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_GB_NO_RELATION;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_IDENTIFIER_CASE:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_IC_MIXED;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_IDENTIFIER_QUOTE_CHAR:
    using_322=is_prefix(mysql_get_server_info(&dbc->mysql),"3.22");
    *pcbInfoValue=(SQLSMALLINT)
      (strmake((char*) rgbInfoValue, (!using_322 ? "`" : " "), cbInfoValueMax)
       - (char*) rgbInfoValue);
    break;

  case SQL_INDEX_KEYWORDS:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_IK_NONE;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_INSERT_STATEMENT:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_IS_INSERT_LITERALS |
             SQL_IS_INSERT_SEARCHED |
             SQL_IS_SELECT_INTO);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_KEYWORDS: /* Need to return all keywords..*/
    *pcbInfoValue=(SQLSMALLINT)
      (strmake((char*) rgbInfoValue, mysql_keywords, cbInfoValueMax)
       - (char*) rgbInfoValue);
    break;

  case SQL_MAX_CATALOG_NAME_LEN:
  case SQL_MAX_COLUMN_NAME_LEN:
  case SQL_MAX_IDENTIFIER_LEN:
  case SQL_MAX_TABLE_NAME_LEN:
    *((SQLUSMALLINT*) rgbInfoValue)= NAME_LEN;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_MAX_COLUMNS_IN_INDEX:
    *((SQLUSMALLINT*) rgbInfoValue)=32;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_MAX_CURSOR_NAME_LEN:
    *((SQLUSMALLINT*) rgbInfoValue)=MYSQL_MAX_CURSOR_LEN;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_MAX_INDEX_SIZE:
    *((SQLUINTEGER*) rgbInfoValue)=500;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_MAX_STATEMENT_LEN:
    *((SQLUINTEGER*) rgbInfoValue)=net_buffer_length;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_MAX_TABLES_IN_SELECT:
    /* This is 63 on 64 bit machines */
    *((SQLUSMALLINT*) rgbInfoValue)=31;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_MAX_USER_NAME_LEN:
    *((SQLUSMALLINT*) rgbInfoValue)=16;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_NON_NULLABLE_COLUMNS:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_NNC_NON_NULL;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_NULL_COLLATION:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_NC_START;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_NUMERIC_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)=
      (SQL_FN_NUM_ABS | SQL_FN_NUM_ACOS | SQL_FN_NUM_ASIN | SQL_FN_NUM_ATAN |
       SQL_FN_NUM_ATAN2 | SQL_FN_NUM_CEILING | SQL_FN_NUM_COS |
       SQL_FN_NUM_COT | SQL_FN_NUM_EXP | SQL_FN_NUM_FLOOR | SQL_FN_NUM_LOG
       | SQL_FN_NUM_MOD | SQL_FN_NUM_SIGN | SQL_FN_NUM_SIN | SQL_FN_NUM_SQRT
       | SQL_FN_NUM_TAN | SQL_FN_NUM_PI | SQL_FN_NUM_RAND |
       SQL_FN_NUM_DEGREES | SQL_FN_NUM_LOG10 | SQL_FN_NUM_POWER |
       SQL_FN_NUM_RADIANS | SQL_FN_NUM_ROUND | SQL_FN_NUM_TRUNCATE);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_ODBC_API_CONFORMANCE:
    *((SQLSMALLINT*) rgbInfoValue)=SQL_OAC_LEVEL1;
    *pcbInfoValue=sizeof(SQLSMALLINT);
    break;

  case SQL_ODBC_SQL_CONFORMANCE:
    *((SQLSMALLINT*) rgbInfoValue)=SQL_OSC_CORE;
    *pcbInfoValue=sizeof(SQLSMALLINT);
    break;

  case SQL_ODBC_INTERFACE_CONFORMANCE:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_OIC_LEVEL1;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_OJ_CAPABILITIES:
    using_322=is_prefix(mysql_get_server_info(&dbc->mysql),"3.22");
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_OJ_LEFT | SQL_OJ_NESTED |
             SQL_OJ_NOT_ORDERED |
             SQL_OJ_INNER | SQL_OJ_ALL_COMPARISON_OPS |
             (!using_322 ? SQL_OJ_RIGHT : 0));
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_PARAM_ARRAY_ROW_COUNTS:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_PARC_NO_BATCH;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_PARAM_ARRAY_SELECTS:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_PAS_NO_SELECT;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_PROCEDURE_TERM:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"",
           cbInfoValueMax) -
         (char*) rgbInfoValue);
    break;

  case SQL_POS_OPERATIONS:
    *((SQLINTEGER*) rgbInfoValue)= (SQL_POS_POSITION |
            SQL_POS_UPDATE  |
            SQL_POS_DELETE |
            SQL_POS_ADD |
            SQL_POS_REFRESH);
    *pcbInfoValue=sizeof(SQLINTEGER);
    break;

  case SQL_POSITIONED_STATEMENTS:
    *((SQLINTEGER*) rgbInfoValue)= (SQL_PS_POSITIONED_DELETE |
            SQL_PS_POSITIONED_UPDATE);
    *pcbInfoValue=sizeof(SQLINTEGER);
    break;

  case SQL_QUOTED_IDENTIFIER_CASE:
    *((SQLUSMALLINT*) rgbInfoValue)=SQL_IC_SENSITIVE;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_SCHEMA_TERM:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_SCROLL_OPTIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SO_FORWARD_ONLY |
             SQL_SO_STATIC | SQL_SO_DYNAMIC);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SCROLL_CONCURRENCY:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SS_ADDITIONS |
             SQL_SS_DELETIONS |
             SQL_SS_UPDATES);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SEARCH_PATTERN_ESCAPE:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"\\",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_SERVER_NAME:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,
           dbc->mysql.host_info,
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_SPECIAL_CHARACTERS:
    *pcbInfoValue=(SQLSMALLINT)(strmake((char*) rgbInfoValue,
          (char*) allowed_chars,
          cbInfoValueMax)
        - (char*) rgbInfoValue);
    break;

  case SQL_SQL_CONFORMANCE:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_SC_SQL92_INTERMEDIATE;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_STRING_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)=
      (SQL_FN_STR_CONCAT | SQL_FN_STR_INSERT |
       SQL_FN_STR_LEFT | SQL_FN_STR_LTRIM | SQL_FN_STR_LENGTH |
       SQL_FN_STR_LOCATE | SQL_FN_STR_LCASE | SQL_FN_STR_REPEAT |
       SQL_FN_STR_REPLACE | SQL_FN_STR_RIGHT | SQL_FN_STR_RTRIM |
       SQL_FN_STR_SUBSTRING | SQL_FN_STR_UCASE | SQL_FN_STR_ASCII |
       SQL_FN_STR_CHAR | SQL_FN_STR_LOCATE_2 | SQL_FN_STR_SOUNDEX |
       SQL_FN_STR_SPACE);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_TIMEDATE_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)=
      (SQL_FN_TD_NOW | SQL_FN_TD_CURDATE | SQL_FN_TD_DAYOFMONTH |
       SQL_FN_TD_DAYOFWEEK | SQL_FN_TD_DAYOFYEAR | SQL_FN_TD_MONTH |
       SQL_FN_TD_QUARTER | SQL_FN_TD_WEEK | SQL_FN_TD_YEAR |
      SQL_FN_TD_CURTIME | SQL_FN_TD_HOUR | SQL_FN_TD_MINUTE |
      SQL_FN_TD_SECOND | SQL_FN_TD_DAYNAME | SQL_FN_TD_MONTHNAME);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_DATETIME_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SDF_CURRENT_DATE |
             SQL_SDF_CURRENT_TIME |
             SQL_SDF_CURRENT_TIMESTAMP);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_GRANT:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SG_DELETE_TABLE |
             SQL_SG_INSERT_COLUMN |
             SQL_SG_INSERT_TABLE |
             SQL_SG_REFERENCES_TABLE |
             SQL_SG_REFERENCES_COLUMN |
             SQL_SG_SELECT_TABLE |
             SQL_SG_UPDATE_COLUMN |
             SQL_SG_UPDATE_TABLE);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SRJO_CROSS_JOIN |
             SQL_SRJO_INNER_JOIN  |
             SQL_SRJO_LEFT_OUTER_JOIN |
             SQL_SRJO_NATURAL_JOIN |
             SQL_SRJO_RIGHT_OUTER_JOIN);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_REVOKE:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SR_DELETE_TABLE |
             SQL_SR_INSERT_COLUMN |
             SQL_SR_INSERT_TABLE |
             SQL_SR_REFERENCES_TABLE |
             SQL_SR_REFERENCES_COLUMN |
             SQL_SR_SELECT_TABLE |
             SQL_SR_UPDATE_COLUMN |
             SQL_SR_UPDATE_TABLE);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SRVC_VALUE_EXPRESSION |
             SQL_SRVC_NULL |
             SQL_SRVC_DEFAULT);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SQL92_STRING_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_SSF_CONVERT |
             SQL_SSF_LOWER |
             SQL_SSF_UPPER |
             SQL_SSF_SUBSTRING |
             SQL_SSF_TRANSLATE |
             SQL_SSF_TRIM_BOTH |
             SQL_SSF_TRIM_LEADING |
             SQL_SSF_TRIM_TRAILING);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_STANDARD_CLI_CONFORMANCE:
    *((SQLUINTEGER*) rgbInfoValue)= SQL_SCC_ISO92_CLI;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_SYSTEM_FUNCTIONS:
    *((SQLUINTEGER*) rgbInfoValue)= (SQL_FN_SYS_DBNAME |
             SQL_FN_SYS_IFNULL |
             SQL_FN_SYS_USERNAME);
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_STATIC_SENSITIVITY:
    *((SQLINTEGER*) rgbInfoValue)= (SQL_SS_ADDITIONS |
            SQL_SS_DELETIONS |
            SQL_SS_UPDATES);
    *pcbInfoValue=sizeof(SQLINTEGER);
    break;

  case SQL_TABLE_TERM:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"table",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_TXN_CAPABLE:
    if(dbc && !(dbc->flag & FLAG_NO_TRANSACTIONS))
      *((SQLUSMALLINT*) rgbInfoValue)=SQL_TC_DDL_COMMIT;
    else
      *((SQLUSMALLINT*) rgbInfoValue)=SQL_TC_NONE;
    *pcbInfoValue=sizeof(SQLUSMALLINT);
    break;

  case SQL_TXN_ISOLATION_OPTION:
    if (!(dbc->mysql.server_capabilities & CLIENT_TRANSACTIONS) ||
         (dbc->flag & FLAG_NO_TRANSACTIONS))
      *((SQLUINTEGER*) rgbInfoValue)= SQL_TXN_READ_COMMITTED;
    else
      *((SQLUINTEGER*) rgbInfoValue)= SQL_TXN_READ_COMMITTED |
                                      SQL_TXN_READ_UNCOMMITTED |
                                      SQL_TXN_REPEATABLE_READ |
                                      SQL_TXN_SERIALIZABLE;
    *pcbInfoValue=sizeof(SQLUINTEGER);
    break;

  case SQL_USER_NAME:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"user",
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_XOPEN_CLI_YEAR:
    *pcbInfoValue=(SQLSMALLINT) (strmake((char*) rgbInfoValue,"1992", 
           cbInfoValueMax)
         - (char*) rgbInfoValue);
    break;

  case SQL_FETCH_DIRECTION:
    *((long*) rgbInfoValue)=
      (SQL_FD_FETCH_NEXT | SQL_FD_FETCH_FIRST |
       SQL_FD_FETCH_LAST | SQL_FD_FETCH_PRIOR |
       SQL_FD_FETCH_ABSOLUTE | SQL_FD_FETCH_RELATIVE);
    if (dbc->flag & FLAG_NO_DEFAULT_CURSOR)
      *((long*) rgbInfoValue)&= ~ (long) SQL_FD_FETCH_PRIOR;
    *pcbInfoValue=sizeof(long);
    break;

  case SQL_ODBC_SAG_CLI_CONFORMANCE:
    *((SQLSMALLINT*) rgbInfoValue)=SQL_OSCC_COMPLIANT;
    *pcbInfoValue=sizeof(SQLSMALLINT);
    break;

  default:
  {
    char buff[80];
    sprintf(buff,"Unsupported option: %d to SQLGetInfo",fInfoType);
    DBUG_RETURN(set_handle_error(SQL_HANDLE_DBC,hdbc,MYERR_S1C00,buff,4000));
  }
  }/* end of switch */
  DBUG_RETURN(SQL_SUCCESS);
}

/*
  Function sets up a result set containing details of the types
  supported by mysql.
*/

MYSQL_FIELD SQL_GET_TYPE_INFO_fields[] = {
  {"TYPE_NAME","GetTypeInfo",NULL,FIELD_TYPE_VAR_STRING,32,32,NOT_NULL_FLAG},
  {"DATA_TYPE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"COLUMN_SIZE","GetTypeInfo",NULL,FIELD_TYPE_LONG,10,10},
  {"LITERAL_PREFIX","GetTypeInfo",NULL,FIELD_TYPE_VAR_STRING,2,2},
  {"LITERAL_SUFFIX","GetTypeInfo",NULL,FIELD_TYPE_VAR_STRING,2,2},
  {"CREATE_PARAMS","GetTypeInfo",NULL,FIELD_TYPE_VAR_STRING,15,15},
  {"NULLABLE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"CASE_SENSITIVE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"SEARCHABLE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"UNSIGNED_ATTRIBUTE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
  {"FIXED_PREC_SCALE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"AUTO_UNIQUE_VALUE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
  {"LOCAL_TYPE_NAME","GetTypeInfo",NULL,FIELD_TYPE_VAR_STRING,60,60},
  {"MINIMUM_SCALE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
  {"MAXIMUM_SCALE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
  {"SQL_DATATYPE","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2,NOT_NULL_FLAG},
  {"SQL_DATETIME_SUB","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
  {"NUM_PREC_RADIX","GetTypeInfo",NULL,FIELD_TYPE_LONG,10,10},
  {"INTERVAL_PRECISION","GetTypeInfo",NULL,FIELD_TYPE_SHORT,2,2},
};


const uint SQL_GET_TYPE_INFO_FIELDS=array_elements(SQL_GET_TYPE_INFO_fields);
#define MYSQL_DATA_TYPES 36

char sql_searchable[6],sql_unsearchable[6],sql_nullable[6],sql_no_nulls[6],
     sql_bit[6],
     sql_tinyint[6],sql_smallint[6],sql_integer[6],sql_bigint[6],sql_float[6],
     sql_real[6],sql_double[6], sql_char[6],sql_varchar[6],sql_longvarchar[6],
     sql_timestamp[6], sql_decimal[6],sql_numeric[6],sql_varbinary[6],
     sql_time[6],sql_date[6],sql_binary[6],sql_longvarbinary[6];

char *SQL_GET_TYPE_INFO_values[MYSQL_DATA_TYPES][19]=
{
  /* SQL_BIT= -7 */
  {"bit",sql_bit,"1",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","char(1)",NULL,NULL,sql_bit,NULL,
    NULL,NULL},

  /* SQL_TINY= -6 */
  {"tinyint",sql_tinyint,"3",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","tinyint",NULL,NULL,sql_tinyint,
    NULL,"10",NULL},
  {"tinyint unsigned",sql_tinyint,"3",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","tinyint unsigned",NULL,NULL,
    sql_tinyint,NULL,"10",NULL},

  /* SQL_BIGINT= -5 */
  {"bigint",sql_bigint,"19",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","bigint",NULL,NULL,sql_bigint,
    NULL,"10",NULL},
  {"bigint unsigned",sql_bigint,"20",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","bigint unsigned",NULL,NULL,sql_bigint,
    NULL,"10",NULL},

  /* SQL_LONGVARBINARY = -3 */
  {"long varbinary",sql_longvarbinary,"16777215","0x",NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","mediumblob",NULL,NULL,sql_longvarbinary,
    NULL,NULL,NULL},
  {"blob",sql_longvarbinary,"65535","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","binary large object (0-65535)",NULL,NULL,
    sql_longvarbinary,NULL,NULL,NULL},
  {"longblob",sql_longvarbinary,"2147483647","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","binary large object, use mediumblob instead",NULL,NULL,
    sql_longvarbinary,NULL,NULL,NULL},
  {"tinyblob",sql_longvarbinary,"255","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","binary large object (0-255) ",NULL,NULL,
    sql_longvarbinary,NULL,NULL,NULL},
  {"mediumblob",sql_longvarbinary,"16777215","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","binary large object",NULL,NULL,
    sql_longvarbinary,NULL,NULL,NULL},

  /* SQL_LONGVARCHAR = -1 */
  {"long varchar",sql_longvarchar,"16777215","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","mediumtext",NULL,NULL,sql_longvarchar,
   NULL,NULL,NULL},
  {"text",sql_longvarchar,"65535","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","text (0-65535)",NULL,NULL,
   sql_longvarchar,NULL,NULL,NULL},
  {"mediumtext",sql_longvarchar,"16777215","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","mediumtext",NULL,NULL,
   sql_longvarchar,NULL,NULL,NULL},

  /* SQL_CHAR = 1 */
  {"char",sql_char,"255","'","'","length",sql_nullable,
   "0",sql_searchable,"0","0","0","char",NULL,NULL,sql_char,
   NULL,NULL,NULL},

  /* SQL_DECIMAL = 3 */
  {"numeric",sql_numeric,"19",NULL,NULL,"precision,scale",sql_nullable,
   "0",sql_searchable,"0","0","0","numeric","0","19",sql_numeric,
   NULL,"10",NULL},
  {"decimal",sql_decimal,"19",NULL,NULL,"precision,scale",sql_nullable,
   "0",sql_searchable,"0","0","0","decimal","0","19",sql_numeric,
   NULL,"10",NULL},

  /* SQL_INTEGER = 4 */
  {"integer",sql_integer,"10",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","integer",NULL,NULL,sql_integer,
   NULL,"10",NULL},
  {"integer unsigned",sql_integer,"10",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","integer unsigned",NULL,NULL,
    sql_integer,NULL,"10",NULL},
  {"int",sql_integer,"10",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","integer",NULL,NULL,sql_integer,
    NULL,"10",NULL},
  {"int unsigned",sql_integer,"10",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","integer unsigned",NULL,NULL,
    sql_integer,NULL,"10",NULL},
  {"mediumint",sql_integer,"7",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","Medium integer",NULL,NULL,
    sql_integer,NULL,"10",NULL},
  {"mediumint unsigned",sql_integer,"8",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","Medium integer unsigned",NULL,NULL,
    sql_integer,NULL,"10",NULL},

  /* SQL_SMALLINT = 5 */
  {"smallint",sql_smallint,"5",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","smallint",NULL,NULL,
    sql_smallint,NULL,"10",NULL},
  {"smallint unsigned",sql_smallint,"5",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"1","0","0","smallint unsigned",NULL,NULL,
    sql_smallint,NULL,"10",NULL},
  {"year",sql_smallint,"4",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","year",NULL,NULL,sql_smallint,
    NULL,"10",NULL},

  /* SQL_FLOAT = 6 */
  {"double",sql_float,"15",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","double","0","4",sql_float,
   NULL,"2",NULL},

  /* SQL_REAL = 7 */
  {"float",sql_real,"7",NULL,NULL,NULL,sql_nullable,
   "0",sql_unsearchable,"0","0","0","float","0","2",sql_float,
   NULL,"10",NULL},

  /* SQL_DOUBLE = 8 */
  {"double",sql_double,"15",NULL,NULL,NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","double","0","4",sql_double,
   NULL,"10",NULL},

  /* SQL_TYPE_DATE = 91 */
  {"date",sql_date,"10","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","date",NULL,NULL,sql_date,
    NULL,NULL,NULL},

  /* SQL_TYPE_TIME = 92 */
  {"time",sql_time,"6","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","time",NULL,NULL,sql_time,
    NULL,NULL,NULL},

  /* SQL_TYPE_TIMESTAMP = 93 */
  {"datetime",sql_timestamp,"21","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","datetime","0","0",sql_timestamp,
    NULL,NULL,NULL},
  {"timestamp",sql_timestamp,"14","'","'",NULL,sql_no_nulls,
   "0",sql_searchable,"0","0","0","timestamp","0","0",sql_timestamp,
    NULL,NULL,NULL},

  /* SQL_VARCHAR = 12 */
  {"text",sql_varchar,"255","'","'","",sql_nullable,
   "0",sql_searchable,"0","0","0","text",NULL,NULL,sql_varchar,
   NULL,NULL,NULL},
  {"varchar",sql_varchar,"255","'","'","maxlength",sql_nullable,
   "0",sql_searchable,"0","0","0","varchar",NULL,NULL,sql_varchar,
   NULL,NULL,NULL},
  {"enum",sql_char,"255","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","enum(value1,value2,value3...)",NULL,NULL,
    sql_varchar,NULL,NULL,NULL},
  {"set",sql_char,"255","'","'",NULL,sql_nullable,
   "0",sql_searchable,"0","0","0","set(value1,value2,value3...)",NULL,NULL,
    sql_varchar,NULL,NULL,NULL}
};

/*
  @type    : ODBC 1.0 API
  @purpose : returns info about datata types supported by the data source
*/

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT hstmt, SQLSMALLINT fSqlType)
{
  STMT FAR *stmt=(STMT FAR*) hstmt;
  uint i;
  DBUG_ENTER("SQLGetTypeInfo");
  DBUG_PRINT("enter",("fSqlType: %d",fSqlType));

  my_SQLFreeStmt(hstmt,MYSQL_RESET);

  /* Set up result Data dictionary. */

  stmt->result=(MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
  stmt->result_array= (char**) my_malloc(sizeof(SQL_GET_TYPE_INFO_values),
           MYF(MY_FAE | MY_ZEROFILL));

  if(stmt->dbc->env->odbc_ver == SQL_OV_ODBC2)
  {	  
    int2str(SQL_TIMESTAMP,sql_timestamp,-10);
    int2str(SQL_DATE,sql_date,-10);
    int2str(SQL_TIME,sql_time,-10);
  }

  if (fSqlType == SQL_ALL_TYPES)
  {
    memcpy((byte*) stmt->result_array,(byte*) SQL_GET_TYPE_INFO_values,
           sizeof(SQL_GET_TYPE_INFO_values));
    stmt->result->row_count=MYSQL_DATA_TYPES;	
  }
  else
  {		
    if((fSqlType == SQL_TYPE_DATE || 
        fSqlType == SQL_TYPE_TIME ||
        fSqlType == SQL_TYPE_TIMESTAMP) &&
        stmt->dbc->env->odbc_ver == SQL_OV_ODBC2)
    {
      /* Hack in case DM passes _type in 2.x behaviour.. */
      fSqlType-=82;
    }

    for (i=0 ; i < MYSQL_DATA_TYPES ; i++)
    {
      if (atoi(SQL_GET_TYPE_INFO_values[i][1]) == fSqlType)
      {
         memcpy((gptr) &stmt->result_array[stmt->result->row_count++
            * SQL_GET_TYPE_INFO_FIELDS],
         (gptr) &SQL_GET_TYPE_INFO_values[i][0],
         sizeof(char*)*SQL_GET_TYPE_INFO_FIELDS);
      }
    }
  }
  mysql_link_fields(stmt,SQL_GET_TYPE_INFO_fields,
        SQL_GET_TYPE_INFO_FIELDS);
  DBUG_RETURN(SQL_SUCCESS);
}

/*
  @type    : myodbc3 internal
  @purpose : function initializaions
*/

void init_getfunctions(void)
{
  DBUG_ENTER("init_getfunctions");

  /* Make some integers to strings for easy init of string arrays */
  int2str(SQL_SEARCHABLE,sql_searchable,-10);
  int2str(SQL_UNSEARCHABLE,sql_unsearchable,-10);
  int2str(SQL_NULLABLE,sql_nullable,-10);
  int2str(SQL_NO_NULLS,sql_no_nulls,-10);
  int2str(SQL_BIT,sql_bit,-10);
  int2str(SQL_TINYINT,sql_tinyint,-10);
  int2str(SQL_SMALLINT,sql_smallint,-10);
  int2str(SQL_INTEGER,sql_integer,-10);
  int2str(SQL_BIGINT,sql_bigint,-10);
  int2str(SQL_DECIMAL,sql_decimal,-10);
  int2str(SQL_NUMERIC,sql_numeric,-10);
  int2str(SQL_REAL,sql_real,-10);
  int2str(SQL_FLOAT,sql_float,-10);
  int2str(SQL_DOUBLE,sql_double,-10);
  int2str(SQL_CHAR,sql_char,-10);
  int2str(SQL_VARCHAR,sql_varchar,-10);
  int2str(SQL_LONGVARCHAR,sql_longvarchar,-10);
  int2str(SQL_LONGVARBINARY,sql_longvarbinary,-10);
  int2str(SQL_VARBINARY,sql_varbinary,-10);
  int2str(SQL_BINARY,sql_binary,-10);
  int2str(SQL_TYPE_TIMESTAMP,sql_timestamp,-10);
  int2str(SQL_TYPE_DATE,sql_date,-10);
  int2str(SQL_TYPE_TIME,sql_time,-10);
  DBUG_VOID_RETURN;
}

/*
  @type    : myodbc3 internal
  @purpose : list of supported and unsupported functions in the driver
*/

SQLINTEGER myodbc3_functions[] = {

  SQL_API_SQLALLOCCONNECT,
  SQL_API_SQLALLOCENV,
  SQL_API_SQLALLOCHANDLE,
  SQL_API_SQLALLOCSTMT,
  SQL_API_SQLBINDCOL,
  /* SQL_API_SQLBINDPARAM */
  SQL_API_SQLCANCEL,
  SQL_API_SQLCLOSECURSOR,
  SQL_API_SQLCOLATTRIBUTE,
  SQL_API_SQLCOLUMNS,
  SQL_API_SQLCONNECT,
  /* SQL_API_SQLCOPYDESC  */
  SQL_API_SQLDATASOURCES,
  SQL_API_SQLDESCRIBECOL,
  SQL_API_SQLDISCONNECT,
  SQL_API_SQLENDTRAN,
  SQL_API_SQLERROR,
  SQL_API_SQLEXECDIRECT,
  SQL_API_SQLEXECUTE,
  SQL_API_SQLFETCH,
  SQL_API_SQLFETCHSCROLL,
  SQL_API_SQLFREECONNECT,
  SQL_API_SQLFREEENV,
  SQL_API_SQLFREEHANDLE,
  SQL_API_SQLFREESTMT,
  SQL_API_SQLGETCONNECTATTR,
  SQL_API_SQLGETCONNECTOPTION,
  SQL_API_SQLGETCURSORNAME,
  SQL_API_SQLGETDATA,
  /* SQL_API_SQLGETDESCFIELD, */
  /* SQL_API_SQLGETDESCREC, */
  SQL_API_SQLGETDIAGFIELD,
  SQL_API_SQLGETDIAGREC,
  SQL_API_SQLGETENVATTR,
  SQL_API_SQLGETFUNCTIONS,
  SQL_API_SQLGETINFO,
  SQL_API_SQLGETSTMTATTR,
  SQL_API_SQLGETSTMTOPTION,
  SQL_API_SQLGETTYPEINFO,
  SQL_API_SQLNUMRESULTCOLS,
  SQL_API_SQLPARAMDATA,
  SQL_API_SQLPREPARE,
  SQL_API_SQLPUTDATA,
  SQL_API_SQLROWCOUNT,
  SQL_API_SQLSETCONNECTATTR,
  SQL_API_SQLSETCONNECTOPTION,
  SQL_API_SQLSETCURSORNAME,
  /* SQL_API_SQLSETDESCFIELD */
  /* SQL_API_SQLSETDESCREC */
  SQL_API_SQLSETENVATTR,
  /* SQL_API_SQLSETPARAM */
  SQL_API_SQLSETSTMTATTR,
  SQL_API_SQLSETSTMTOPTION,
  SQL_API_SQLSPECIALCOLUMNS,
  SQL_API_SQLSTATISTICS,
  SQL_API_SQLTABLES,
  SQL_API_SQLTRANSACT,
  /* SQL_API_SQLALLOCHANDLESTD */
  SQL_API_SQLBULKOPERATIONS,
  SQL_API_SQLBINDPARAMETER,
  SQL_API_SQLBROWSECONNECT,
  SQL_API_SQLCOLATTRIBUTES,
  SQL_API_SQLCOLUMNPRIVILEGES ,
  SQL_API_SQLDESCRIBEPARAM,
  SQL_API_SQLDRIVERCONNECT,
  SQL_API_SQLDRIVERS,
  SQL_API_SQLEXTENDEDFETCH,
  SQL_API_SQLFOREIGNKEYS,
  SQL_API_SQLMORERESULTS,
  SQL_API_SQLNATIVESQL,
  SQL_API_SQLNUMPARAMS,
  SQL_API_SQLPARAMOPTIONS,
  SQL_API_SQLPRIMARYKEYS,
  SQL_API_SQLPROCEDURECOLUMNS,
  SQL_API_SQLPROCEDURES,
  SQL_API_SQLSETPOS,
  SQL_API_SQLSETSCROLLOPTIONS,
  SQL_API_SQLTABLEPRIVILEGES
};

/*
  @type    : ODBC 1.0 API
  @purpose : returns information about whether a driver supports a specific
       ODBC function
*/

SQLRETURN SQL_API
SQLGetFunctions(SQLHDBC hdbc,SQLUSMALLINT fFunction,
    SQLUSMALLINT FAR *pfExists)
{
  DBC FAR *dbc=(DBC FAR*) hdbc;
  SQLINTEGER index;
  SQLINTEGER myodbc_func_size;
  DBUG_ENTER("SQLGetFunctions");
  DBUG_PRINT("enter",("fFunction: %d",fFunction));

  myodbc_func_size = (sizeof(myodbc3_functions)/
          sizeof(myodbc3_functions[0]));

  if (fFunction == SQL_API_ODBC3_ALL_FUNCTIONS)
  {
    for (index = 0; index < myodbc_func_size; index ++)
    {
      int id = myodbc3_functions[index];
      pfExists[id >> 4] |= (1 << ( id & 0x000F));
    }
    DBUG_RETURN(SQL_SUCCESS);
  }
  if (fFunction == SQL_API_ALL_FUNCTIONS)
  {
    for (index = 0; index < myodbc_func_size; index ++)
    {
      if (myodbc3_functions[index] < 100)
  pfExists[myodbc3_functions[index]] = SQL_TRUE;
    }
    DBUG_RETURN(SQL_SUCCESS);
  }
  *pfExists = SQL_FALSE;
  for (index = 0; index < myodbc_func_size; index ++)
  {
    if (myodbc3_functions[index] == fFunction)
    {
      *pfExists = SQL_TRUE;
      break;
    }
  }
  DBUG_RETURN(SQL_SUCCESS);
}
