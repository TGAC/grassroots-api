//#include "rodsGenQueryNames.h"
#include "rcMisc.h"

#include "typedefs.h"
#include "byte_buffer.h"
#include "meta_search.h"
#include "streams.h"
#include "memory_allocations.h"


static bool AddSearchTermNodeFromJSON (LinkedList *terms_p, const json_t * const json_p);



SearchTermNode *AllocateSearchTermNode (const char *clause_s, const char *key_s, const char *op_s, const char *value_s)
{
	SearchTermNode *node_p = (SearchTermNode *) AllocMemory (sizeof (SearchTermNode));

	if (node_p)
		{
			node_p -> stn_node.ln_prev_p = node_p -> stn_node.ln_next_p = NULL;

			node_p -> stn_term.st_clause_s = clause_s;
			node_p -> stn_term.st_key_s = clause_s;
			node_p -> stn_term.st_op_s = op_s;
			node_p -> stn_term.st_value_s = value_s;
		}

	return node_p;
}


int32 DetermineSearchTerms (LinkedList *terms_p, const json_t *json_p)
{
	int32 res = 0;

	if (json_is_array (json_p))
		{
			/* array is a JSON array */
			size_t index;
			json_t *json_value_p;

			json_array_foreach (json_p, index, json_value_p)
				{
					if (AddSearchTermNodeFromJSON (terms_p, json_value_p))
						{
							++ res;
						}
					else
						{
							char *dump_s = json_dumps (json_value_p, JSON_INDENT (2));

							if (dump_s)
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to get search term node from %s\n", dump_s);
									free (dump_s);
								}
						}
				}
		}
	else
		{
			if (AddSearchTermNodeFromJSON (terms_p, json_p))
				{
					++ res;
				}
			else
				{
					char *dump_s = json_dumps (json_p, JSON_INDENT (2));

					if (dump_s)
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to get search term node from %s\n", dump_s);
							free (dump_s);
						}
				}
		}

	return res;
}


static bool AddSearchTermNodeFromJSON (LinkedList *terms_p, const json_t * const json_p)
{
	bool success_flag = false;
	SearchTermNode *node_p = NULL;
	json_t *child_json_p = json_object_get (json_p, "operation");

	if (child_json_p  && json_is_string (child_json_p))
		{
			const char *op_s = json_string_value (child_json_p);

			child_json_p = json_object_get (json_p, "key");

			if (child_json_p && json_is_string (child_json_p))
				{
					const char *key_s = json_string_value (child_json_p);

					child_json_p = json_object_get (json_p, "value");

					if (child_json_p && json_is_string (child_json_p))
						{
							const char *value_s = json_string_value (child_json_p);
							const char *clause_s = NULL;

							child_json_p = json_object_get (json_p, "clause");

							if (child_json_p && json_is_string (child_json_p))
								{
									clause_s = json_string_value (child_json_p);
								}

							node_p = AllocateSearchTermNode (clause_s, key_s, op_s, value_s);

							if (node_p)
								{
									LinkedListAddTail (terms_p, & (node_p -> stn_node));
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to allocate search term node\n");
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to get search term value from json\n");
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, "Failed to get search term key from json\n");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to get search term operation from json\n");
		}

	return success_flag;
}

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
int DoMetaSearch (rcComm_t *connection_p, const int *column_ids_p, const uint32 num_columns, const char **search_tags_pp, bool upper_case_flag, char * const zone_s)
{
	#define CLAUSE_SIZE (20)
#define BIG_STR (300)
  genQueryInp_t input_query;
  genQueryOut_t *query_output_p = NULL;
  int i1a[CLAUSE_SIZE];
  int i1b[CLAUSE_SIZE];
  int i2a[CLAUSE_SIZE];
  char *condVal[CLAUSE_SIZE];
  char v1[BIG_STR];
  char v2[BIG_STR];
  char v3[BIG_STR];

  ByteBuffer *condition_values_p;

  ByteBuffer *v1_p;
  ByteBuffer *v2_p;
  ByteBuffer *v3_p;

  int status;
  //char *columnNames[]={"collection", "dataObj"};
  int cmdIx;
  int condIx;
  char vstr[CLAUSE_SIZE] [BIG_STR];
  uint32 i;

  memset (&input_query, 0, sizeof (genQueryInp_t));

  if (upper_case_flag)
  	{
  		input_query.options = UPPER_CASE_WHERE;
  	}

  memcpy (i1a, column_ids_p, num_columns * sizeof (int));
  memset (i1b, 0, num_columns * sizeof (int));

  input_query.selectInp.inx = i1a;
  input_query.selectInp.value = i1b;
  input_query.selectInp.len = num_columns;

  i2a[0]=COL_META_DATA_ATTR_NAME;

  if (AppendStringsToByteBuffer (v1_p, "='", search_tags_pp [2], "'", NULL))
  	{
  	  if (AppendStringsToByteBuffer (v2_p, search_tags_pp [3], " '", search_tags_pp [4], "'", NULL))
  	  	{

  	  	}

  	}

  condVal[0]=v1;

  i2a[1]=COL_META_DATA_ATTR_VALUE;
  condVal[1]=v2;

  input_query.sqlCondInp.inx = i2a;
  input_query.sqlCondInp.value = condVal;
  input_query.sqlCondInp.len = num_columns;

  if (strcmp(search_tags_pp[5], "or")==0)
  	{
  		if (AppendStringsToByteBuffer (v2_p, "|| ", search_tags_pp [6], "'", search_tags_pp [7], "'", NULL))
  			{

  			}
  }



  cmdIx = 5;
  condIx = 2;
  while (strcmp(search_tags_pp[cmdIx], "and")==0) {
     i2a[condIx]=COL_META_DATA_ATTR_NAME;
     cmdIx++;
     snprintf(vstr[condIx],BIG_STR,"='%s'", search_tags_pp[cmdIx]);
     condVal[condIx]=vstr[condIx];
     condIx++;

     i2a[condIx]=COL_META_DATA_ATTR_VALUE;
     snprintf(vstr[condIx], BIG_STR,
	       "%s '%s'", search_tags_pp[cmdIx+1], search_tags_pp[cmdIx+2]);
     cmdIx+=3;
     condVal[condIx]=vstr[condIx];
     condIx++;
     input_query.sqlCondInp.len+=2;
  }

  if (*search_tags_pp[cmdIx] != '\0') {
     printf("Unrecognized input\n");
     return(-2);
  }

  input_query.maxRows=10;
  input_query.continueInx=0;
  input_query.condInput.len=0;

  if (zone_s)
  	{
  		addKeyVal (&input_query.condInput, ZONE_KW, zone_s);
  	}

  status = rcGenQuery (connection_p, &input_query, &query_output_p);

  //printGenQueryResults(Conn, status, genQueryOut, columnNames);

  while (status==0 && query_output_p->continueInx > 0) {
  		input_query.continueInx=query_output_p->continueInx;
     status = rcGenQuery(connection_p, &input_query, &query_output_p);
     if (query_output_p->rowCnt>0) printf("----\n");
     //printQueryResults(connection_p, status, query_output_p, NULL);
  }

  return (0);

}



#if 0

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


#endif
