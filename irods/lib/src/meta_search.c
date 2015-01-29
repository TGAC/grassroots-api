#include "rodsGenQueryNames.h"
#include "rcMisc.h"

#include "typedefs.h"


/*
  1898  imeta add -d barley crop barley
 1899  imeta add -d wheat_and_barley crop barley
 1900  imeta add -d wheat_and_barley crop wheat
 1901  imeta add -d wheat crop wheat
 1902  imeta au -d crop wheat
 1903  imeta au -d crop = wheat
 1904  imeta qu -d crop = wheat
 1905  imeta qu -d crop =  barley

 *
 */
int DoMetaSearch (rcComm_t *connection_p, const int *column_ids_p, const uint32 num_columns, const char **search_tags_pp, bool upper_case_flag )
{
	#define CLAUSE_SIZE (20)

  genQueryInp_t genQueryInp;
  genQueryOut_t *genQueryOut;
  int i1a[CLAUSE_SIZE];
  int i1b[CLAUSE_SIZE];
  int i2a[CLAUSE_SIZE];
  char *condVal[CLAUSE_SIZE];
  char v1[BIG_STR];
  char v2[BIG_STR];
  char v3[BIG_STR];
  int status;
  //char *columnNames[]={"collection", "dataObj"};
  int cmdIx;
  int condIx;
  char vstr[CLAUSE_SIZE] [BIG_STR];
  uint32 i;

  memset (&genQueryInp, 0, sizeof (genQueryInp_t));

  if (upper_case_flag)
  	{
  		genQueryInp.options = UPPER_CASE_WHERE;
  	}

  memcpy (i1a, column_ids_p, num_columns * sizeof (int));
  memset (i1b, 0, num_columns * sizeof (int));

  genQueryInp.selectInp.inx = i1a;
  genQueryInp.selectInp.value = i1b;
  genQueryInp.selectInp.len = num_columns;

  i2a[0]=COL_META_DATA_ATTR_NAME;
  snprintf(v1,sizeof(v1),"='%s'", cmdToken[2]);
  condVal[0]=v1;

  i2a[1]=COL_META_DATA_ATTR_VALUE;
  snprintf(v2, sizeof(v2),"%s '%s'", cmdToken[3], cmdToken[4]);
  condVal[1]=v2;

  genQueryInp.sqlCondInp.inx = i2a;
  genQueryInp.sqlCondInp.value = condVal;
  genQueryInp.sqlCondInp.len=2;

  if (strcmp(cmdToken[5], "or")==0) {
     snprintf(v3, sizeof(v3), "|| %s '%s'", cmdToken[6], cmdToken[7]);
     rstrcat(v2, v3, BIG_STR);
  }

  cmdIx = 5;
  condIx = 2;
  while (strcmp(cmdToken[cmdIx], "and")==0) {
     i2a[condIx]=COL_META_DATA_ATTR_NAME;
     cmdIx++;
     snprintf(vstr[condIx],BIG_STR,"='%s'", cmdToken[cmdIx]);
     condVal[condIx]=vstr[condIx];
     condIx++;

     i2a[condIx]=COL_META_DATA_ATTR_VALUE;
     snprintf(vstr[condIx], BIG_STR,
	       "%s '%s'", cmdToken[cmdIx+1], cmdToken[cmdIx+2]);
     cmdIx+=3;
     condVal[condIx]=vstr[condIx];
     condIx++;
     genQueryInp.sqlCondInp.len+=2;
  }

  if (*cmdToken[cmdIx] != '\0') {
     printf("Unrecognized input\n");
     return(-2);
  }

  genQueryInp.maxRows=10;
  genQueryInp.continueInx=0;
  genQueryInp.condInput.len=0;

  if (zoneArgument[0]!='\0') {
     addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
  }

  status = rcGenQuery (connection_p, &genQueryInp, &genQueryOut);

  //printGenQueryResults(Conn, status, genQueryOut, columnNames);

  while (status==0 && genQueryOut->continueInx > 0) {
     genQueryInp.continueInx=genQueryOut->continueInx;
     status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
     if (genQueryOut->rowCnt>0) printf("----\n");
     printGenQueryResults(Conn, status, genQueryOut,
					columnNames);
  }

  return (0);

}

/*
Do a query on AVUs for dataobjs and show the results
attribute op value [AND attribute op value] [REPEAT]
 */
int queryDataObj(char *cmdToken[]) {
   genQueryInp_t genQueryInp;
   genQueryOut_t *genQueryOut;
   int i1a[20];
   int i1b[20];
   int i2a[20];
   char *condVal[20];
   char v1[BIG_STR];
   char v2[BIG_STR];
   char v3[BIG_STR];
   int status;
   char *columnNames[]={"collection", "dataObj"};
   int cmdIx;
   int condIx;
   char vstr[20] [BIG_STR];

   memset (&genQueryInp, 0, sizeof (genQueryInp_t));

   if (upperCaseFlag) {
     genQueryInp.options = UPPER_CASE_WHERE;
   }

   printCount=0;
   i1a[0]=COL_COLL_NAME;
   i1b[0]=0;  /* (unused) */
   i1a[1]=COL_DATA_NAME;
   i1b[1]=0;
   genQueryInp.selectInp.inx = i1a;
   genQueryInp.selectInp.value = i1b;
   genQueryInp.selectInp.len = 2;

   i2a[0]=COL_META_DATA_ATTR_NAME;
   snprintf(v1,sizeof(v1),"='%s'", cmdToken[2]);
   condVal[0]=v1;

   i2a[1]=COL_META_DATA_ATTR_VALUE;
   snprintf(v2, sizeof(v2),"%s '%s'", cmdToken[3], cmdToken[4]);
   condVal[1]=v2;

   genQueryInp.sqlCondInp.inx = i2a;
   genQueryInp.sqlCondInp.value = condVal;
   genQueryInp.sqlCondInp.len=2;

   if (strcmp(cmdToken[5], "or")==0) {
      snprintf(v3, sizeof(v3), "|| %s '%s'", cmdToken[6], cmdToken[7]);
      rstrcat(v2, v3, BIG_STR);
   }

   cmdIx = 5;
   condIx = 2;
   while (strcmp(cmdToken[cmdIx], "and")==0) {
      i2a[condIx]=COL_META_DATA_ATTR_NAME;
      cmdIx++;
      snprintf(vstr[condIx],BIG_STR,"='%s'", cmdToken[cmdIx]);
      condVal[condIx]=vstr[condIx];
      condIx++;

      i2a[condIx]=COL_META_DATA_ATTR_VALUE;
      snprintf(vstr[condIx], BIG_STR,
	       "%s '%s'", cmdToken[cmdIx+1], cmdToken[cmdIx+2]);
      cmdIx+=3;
      condVal[condIx]=vstr[condIx];
      condIx++;
      genQueryInp.sqlCondInp.len+=2;
   }

   if (*cmdToken[cmdIx] != '\0') {
      printf("Unrecognized input\n");
      return(-2);
   }

   genQueryInp.maxRows=10;
   genQueryInp.continueInx=0;
   genQueryInp.condInput.len=0;

   if (zoneArgument[0]!='\0') {
      addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
   }

   status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);

   printGenQueryResults(Conn, status, genQueryOut, columnNames);

   while (status==0 && genQueryOut->continueInx > 0) {
      genQueryInp.continueInx=genQueryOut->continueInx;
      status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
      if (genQueryOut->rowCnt>0) printf("----\n");
      printGenQueryResults(Conn, status, genQueryOut, 
					columnNames);
   }

   return (0);
}

/*
Do a query on AVUs for collections and show the results
 */
int queryCollection(char *cmdToken[]) {
   genQueryInp_t genQueryInp;
   genQueryOut_t *genQueryOut;
   int i1a[20];
   int i1b[20];
   int i2a[20];
   char *condVal[20];
   char v1[BIG_STR];
   char v2[BIG_STR];
   char v3[BIG_STR];
   int status;
   char *columnNames[]={"collection"};
   int cmdIx;
   int condIx;
   char vstr[20] [BIG_STR];

   memset (&genQueryInp, 0, sizeof (genQueryInp_t));

   if (upperCaseFlag) {
     genQueryInp.options = UPPER_CASE_WHERE;
   }

   printCount=0;
   i1a[0]=COL_COLL_NAME;
   i1b[0]=0;  /* (unused) */
   genQueryInp.selectInp.inx = i1a;
   genQueryInp.selectInp.value = i1b;
   genQueryInp.selectInp.len = 1;

   i2a[0]=COL_META_COLL_ATTR_NAME;
   snprintf(v1,sizeof(v1),"='%s'",cmdToken[2]);
   condVal[0]=v1;

   i2a[1]=COL_META_COLL_ATTR_VALUE;
   snprintf(v2, sizeof(v2),"%s '%s'", cmdToken[3], cmdToken[4]);
   condVal[1]=v2;

   genQueryInp.sqlCondInp.inx = i2a;
   genQueryInp.sqlCondInp.value = condVal;
   genQueryInp.sqlCondInp.len=2;

   if (strcmp(cmdToken[5], "or")==0) {
      snprintf(v3, sizeof(v3), "|| %s '%s'", cmdToken[6], cmdToken[7]);
      rstrcat(v2, v3, BIG_STR);
   }

   cmdIx = 5;
   condIx = 2;
   while (strcmp(cmdToken[cmdIx], "and")==0) {
      i2a[condIx]=COL_META_COLL_ATTR_NAME;
      cmdIx++;
      snprintf(vstr[condIx],BIG_STR,"='%s'", cmdToken[cmdIx]);
      condVal[condIx]=vstr[condIx];
      condIx++;

      i2a[condIx]=COL_META_COLL_ATTR_VALUE;
      snprintf(vstr[condIx], BIG_STR, 
	       "%s '%s'", cmdToken[cmdIx+1], cmdToken[cmdIx+2]);
      cmdIx+=3;
      condVal[condIx]=vstr[condIx];
      condIx++;
      genQueryInp.sqlCondInp.len+=2;
   }

   if (*cmdToken[cmdIx] != '\0') {
      printf("Unrecognized input\n");
      return(-2);
   }

   genQueryInp.maxRows=10;
   genQueryInp.continueInx=0;
   genQueryInp.condInput.len=0;

   if (zoneArgument[0]!='\0') {
      addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
   }

   status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);

   printGenQueryResults(Conn, status, genQueryOut, columnNames);

   while (status==0 && genQueryOut->continueInx > 0) {
      genQueryInp.continueInx=genQueryOut->continueInx;
      status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
      if (genQueryOut->rowCnt>0) printf("----\n");
      printGenQueryResults(Conn, status, genQueryOut, 
					columnNames);
   }

   return (0);
}


/*
Do a query on AVUs for resources and show the results
 */
int queryResc(char *attribute, char *op, char *value) {
   genQueryInp_t genQueryInp;
   genQueryOut_t *genQueryOut;
   int i1a[10];
   int i1b[10];
   int i2a[10];
   char *condVal[10];
   char v1[BIG_STR];
   char v2[BIG_STR];
   int status;
   char *columnNames[]={"resource"};

   memset (&genQueryInp, 0, sizeof (genQueryInp_t));

   if (upperCaseFlag) {
     genQueryInp.options = UPPER_CASE_WHERE;
   }

   printCount=0;
   i1a[0]=COL_R_RESC_NAME;
   i1b[0]=0;  /* (unused) */
   genQueryInp.selectInp.inx = i1a;
   genQueryInp.selectInp.value = i1b;
   genQueryInp.selectInp.len = 1;

   i2a[0]=COL_META_RESC_ATTR_NAME;
   snprintf(v1,sizeof(v1),"='%s'",attribute);
   condVal[0]=v1;

   i2a[1]=COL_META_RESC_ATTR_VALUE;
   snprintf(v2, sizeof(v2),"%s '%s'", op, value);
   condVal[1]=v2;

   genQueryInp.sqlCondInp.inx = i2a;
   genQueryInp.sqlCondInp.value = condVal;
   genQueryInp.sqlCondInp.len=2;

   genQueryInp.maxRows=10;
   genQueryInp.continueInx=0;
   genQueryInp.condInput.len=0;

   if (zoneArgument[0]!='\0') {
      addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
   }

   status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);

   printGenQueryResults(Conn, status, genQueryOut, columnNames);

   while (status==0 && genQueryOut->continueInx > 0) {
      genQueryInp.continueInx=genQueryOut->continueInx;
      status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
      if (genQueryOut->rowCnt>0) printf("----\n");
      printGenQueryResults(Conn, status, genQueryOut, 
					columnNames);
   }

   return (0);
}

/*
Do a query on AVUs for resource groups and show the results
 */
int queryRescGroup(char *attribute, char *op, char *value) {
   genQueryInp_t genQueryInp;
   genQueryOut_t *genQueryOut;
   int i1a[10];
   int i1b[10];
   int i2a[10];
   char *condVal[10];
   char v1[BIG_STR];
   char v2[BIG_STR];
   int status;
   char *columnNames[]={"resource group"};

   memset (&genQueryInp, 0, sizeof (genQueryInp_t));

   if (upperCaseFlag) {
     genQueryInp.options = UPPER_CASE_WHERE;
   }

   printCount=0;
   i1a[0]=COL_RESC_GROUP_NAME;
   i1b[0]=0;  /* (unused) */
   genQueryInp.selectInp.inx = i1a;
   genQueryInp.selectInp.value = i1b;
   genQueryInp.selectInp.len = 1;

   i2a[0]=COL_META_RESC_GROUP_ATTR_NAME;
   snprintf(v1,sizeof(v1),"='%s'",attribute);
   condVal[0]=v1;

   i2a[1]=COL_META_RESC_GROUP_ATTR_VALUE;
   snprintf(v2, sizeof(v2),"%s '%s'", op, value);
   condVal[1]=v2;

   genQueryInp.sqlCondInp.inx = i2a;
   genQueryInp.sqlCondInp.value = condVal;
   genQueryInp.sqlCondInp.len=2;

   genQueryInp.maxRows=10;
   genQueryInp.continueInx=0;
   genQueryInp.condInput.len=0;

   /*
   if (zoneArgument[0]!='\0') {
      addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
   }
   */
   
   status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);

   printGenQueryResults(Conn, status, genQueryOut, columnNames);

   while (status==0 && genQueryOut->continueInx > 0) {
      genQueryInp.continueInx=genQueryOut->continueInx;
      status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
      if (genQueryOut->rowCnt>0) printf("----\n");
      printGenQueryResults(Conn, status, genQueryOut, 
					columnNames);
   }

   return (0);
}

/*
Do a query on AVUs for users and show the results
 */
int queryUser(char *attribute, char *op, char *value) {
   genQueryInp_t genQueryInp;
   genQueryOut_t *genQueryOut;
   int i1a[10];
   int i1b[10];
   int i2a[10];
   char *condVal[10];
   char v1[BIG_STR];
   char v2[BIG_STR];
   int status;
   char *columnNames[]={"user", "zone"};

   printCount=0;
   memset (&genQueryInp, 0, sizeof (genQueryInp_t));

   if (upperCaseFlag) {
     genQueryInp.options = UPPER_CASE_WHERE;
   }

   i1a[0]=COL_USER_NAME;
   i1b[0]=0;  /* (unused) */
   i1a[1]=COL_USER_ZONE;
   i1b[1]=0;  /* (unused) */
   genQueryInp.selectInp.inx = i1a;
   genQueryInp.selectInp.value = i1b;
   genQueryInp.selectInp.len = 2;

   i2a[0]=COL_META_USER_ATTR_NAME;
   snprintf(v1,sizeof(v1),"='%s'",attribute);
   condVal[0]=v1;

   i2a[1]=COL_META_USER_ATTR_VALUE;
   snprintf(v2, sizeof(v2),"%s '%s'", op, value);
   condVal[1]=v2;

   genQueryInp.sqlCondInp.inx = i2a;
   genQueryInp.sqlCondInp.value = condVal;
   genQueryInp.sqlCondInp.len=2;

   genQueryInp.maxRows=10;
   genQueryInp.continueInx=0;
   genQueryInp.condInput.len=0;

   if (zoneArgument[0]!='\0') {
      addKeyVal (&genQueryInp.condInput, ZONE_KW, zoneArgument);
   }

   status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);

   printGenQueryResults(Conn, status, genQueryOut, columnNames);

   while (status==0 && genQueryOut->continueInx > 0) {
      genQueryInp.continueInx=genQueryOut->continueInx;
      status = rcGenQuery(Conn, &genQueryInp, &genQueryOut);
      if (genQueryOut->rowCnt>0) printf("----\n");
      printGenQueryResults(Conn, status, genQueryOut, 
					columnNames);
   }

   return (0);
}


