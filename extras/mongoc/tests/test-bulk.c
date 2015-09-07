#include <mongoc.h>
#include <mongoc-bulk-operation-private.h>

#include "TestSuite.h"

#include "test-libmongoc.h"
#include "mongoc-tests.h"


static mongoc_array_t gTmpBsonArray;
static char *gHugeString;
static int gHugeStringLength;
static char *gFourMBString;
static int gFourMB = 1024 * 1024 * 4;


void
test_bulk_init ()
{
   _mongoc_array_init (&gTmpBsonArray, sizeof (bson_t *));
}


void
test_bulk_cleanup ()
{
   typedef bson_t *bson_ptr;
   int i;
   bson_t *doc;

   for (i = 0; i < gTmpBsonArray.len; i++) {
      doc = _mongoc_array_index (&gTmpBsonArray, bson_ptr, i);
      bson_destroy (doc);
   }

   _mongoc_array_destroy (&gTmpBsonArray);

   bson_free (gHugeString);
}


/* copy str with single-quotes replaced by double. bson_free the return value.*/
char *
single_quotes_to_double (const char *str)
{
   char *result = bson_strdup (str);
   char *p;

   for (p = result; *p; p++) {
      if (*p == '\'') {
         *p = '"';
      }
   }

   return result;
}


bson_t *
tmp_bson (const char *json)
{
   bson_error_t error;
   char *double_quoted = single_quotes_to_double (json);
   bson_t *doc = bson_new_from_json ((const uint8_t *)double_quoted,
                                     -1, &error);

   if (!doc) {
      fprintf (stderr, "%s\n", error.message);
      abort ();
   }

   _mongoc_array_append_val (&gTmpBsonArray, doc);

   bson_free (double_quoted);

   return doc;
}


void
init_huge_string (mongoc_client_t *client)
{
   assert (client);

   if (!gHugeString) {
      gHugeStringLength = (int)mongoc_client_get_max_bson_size (client) - 37;
      gHugeString = bson_malloc ((size_t)gHugeStringLength);
      assert (gHugeString);
      memset (gHugeString, 'a', gHugeStringLength - 1);
      gHugeString[gHugeStringLength - 1] = '\0';
   }
}


const char *
huge_string (mongoc_client_t *client)
{
   init_huge_string (client);
   return gHugeString;
}


int
huge_string_length (mongoc_client_t *client)
{
   init_huge_string (client);
   return gHugeStringLength;
}



void
init_four_mb_string ()
{
   if (!gFourMBString) {
      gFourMBString = bson_malloc ((size_t)gFourMB);
      assert (gFourMBString);
      memset (gFourMBString, 'a', gFourMB - 1);
      gFourMBString[gFourMB - 1] = '\0';
   }
}


const char *
four_mb_string ()
{
   init_four_mb_string ();
   return gFourMBString;
}


/*--------------------------------------------------------------------------
 *
 * match_json --
 *
 *       Check that a document matches an expected pattern.
 *
 *       The provided JSON is fed to mongoc_matcher_t, so it can omit
 *       fields or use $gt, $in, $and, $or, etc. For convenience,
 *       single-quotes are synonymous with double-quotes.
 *
 * Returns:
 *       True or false.
 *
 * Side effects:
 *       Logs if no match.
 *
 *--------------------------------------------------------------------------
 */

bool
match_json (const bson_t *doc,
            const char   *json_query,
            const char   *filename,
            int           lineno,
            const char   *funcname)
{
   char *double_quoted = single_quotes_to_double (json_query);
   bson_error_t error;
   bson_t *query;
   mongoc_matcher_t *matcher;
   bool matches;

   query = bson_new_from_json ((const uint8_t *)double_quoted, -1, &error);

   if (!query) {
      fprintf (stderr, "couldn't parse JSON: %s\n", error.message);
      abort ();
   }

   if (!(matcher = mongoc_matcher_new (query, &error))) {
      fprintf (stderr, "couldn't parse JSON: %s\n", error.message);
      abort ();
   }

   matches = mongoc_matcher_match (matcher, doc);

   if (!matches) {
      fprintf (stderr,
               "ASSERT_MATCH failed with document:\n\n"
               "%s\n"
               "query:\n%s\n\n"
               "%s:%d  %s()\n",
               bson_as_json (doc, NULL), double_quoted,
               filename, lineno, funcname);
   }

   mongoc_matcher_destroy (matcher);
   bson_destroy (query);
   bson_free (double_quoted);

   return matches;
}

#define ASSERT_MATCH(doc, json_query) \
   do { \
      assert (match_json (doc, json_query, \
                          __FILE__, __LINE__, __FUNCTION__)); \
   } while (0)


/*--------------------------------------------------------------------------
 *
 * server_has_write_commands --
 *
 *       Decide with wire version if server supports write commands
 *
 * Returns:
 *       True or false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
server_has_write_commands (mongoc_client_t *client)
{
   bson_t *ismaster_cmd = tmp_bson ("{'ismaster': 1}");
   bson_t ismaster;
   bson_iter_t iter;
   bool expect;

   assert (mongoc_client_command_simple (client, "admin", ismaster_cmd,
                                         NULL, &ismaster, NULL));

   expect = (bson_iter_init_find_case (&iter, &ismaster, "maxWireVersion") &&
             BSON_ITER_HOLDS_INT32 (&iter) &&
             bson_iter_int32 (&iter) > 1);

   bson_destroy (&ismaster);

   return expect;
}


/*--------------------------------------------------------------------------
 *
 * check_n_modified --
 *
 *       Check a bulk operation reply's nModified field is correct or absent.
 *
 *       It may be omitted if we talked to a (<= 2.4.x) node, or a mongos
 *       talked to a (<= 2.4.x) node.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       Aborts if the field is incorrect.
 *
 *--------------------------------------------------------------------------
 */

void
check_n_modified (bool          has_write_commands,
                  const bson_t *reply,
                  int32_t       n_modified)
{
   bson_iter_t iter;

   if (bson_iter_init_find (&iter, reply, "nModified")) {
      assert (has_write_commands);
      assert (BSON_ITER_HOLDS_INT32 (&iter));
      assert (bson_iter_int32 (&iter) == n_modified);
   } else {
      assert (!has_write_commands);
   }
}


/*--------------------------------------------------------------------------
 *
 * assert_error_count --
 *
 *       Check the length of a bulk operation reply's writeErrors.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       Aborts if the array is the wrong length.
 *
 *--------------------------------------------------------------------------
 */

void
assert_error_count (int           len,
                    const bson_t *reply)
{
   bson_iter_t iter;
   bson_iter_t error_iter;
   int n = 0;

   assert (bson_iter_init_find (&iter, reply, "writeErrors"));
   assert (bson_iter_recurse (&iter, &error_iter));
   while (bson_iter_next (&error_iter)) {
      n++;
   }
   ASSERT_CMPINT (len, ==, n);
}


/*--------------------------------------------------------------------------
 *
 * assert_n_inserted --
 *
 *       Check a bulk operation reply's nInserted field.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       Aborts if the field is incorrect.
 *
 *--------------------------------------------------------------------------
 */

void
assert_n_inserted (int           n,
                   const bson_t *reply)
{
   bson_iter_t iter;

   assert (bson_iter_init_find (&iter, reply, "nInserted"));
   assert (BSON_ITER_HOLDS_INT32 (&iter));
   ASSERT_CMPINT (n, ==, bson_iter_int32 (&iter));
}


#define ASSERT_COUNT(n, collection) \
   do { \
      int count = (int)mongoc_collection_count (collection, MONGOC_QUERY_NONE, \
                                                NULL, 0, 0, NULL, NULL); \
      if ((n) != count) { \
         fprintf(stderr, "FAIL\n\nAssert Failure: count of %s is %d, not %d\n" \
                         "%s:%d  %s()\n", \
                         mongoc_collection_get_name (collection), count, n, \
                         __FILE__, __LINE__, __FUNCTION__); \
         abort(); \
      } \
   } while (0)


/*--------------------------------------------------------------------------
 *
 * oid_created_on_client --
 *
 *       Check that a document's _id contains this process's pid.
 *
 * Returns:
 *       True or false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
oid_created_on_client (const bson_t *doc)
{
   bson_oid_t new_oid;
   const uint8_t *new_pid;
   bson_iter_t iter;
   const bson_oid_t *oid;
   const uint8_t *pid;

   bson_oid_init (&new_oid, NULL);
   new_pid = &new_oid.bytes[7];

   bson_iter_init_find (&iter, doc, "_id");

   if (!BSON_ITER_HOLDS_OID (&iter)) {
      return false;
   }

   oid = bson_iter_oid (&iter);
   pid = &oid->bytes[7];

   return 0 == memcmp (pid, new_pid, 2);
}

static mongoc_collection_t *
get_test_collection (mongoc_client_t *client,
                     const char      *prefix)
{
   mongoc_collection_t *ret;
   char *str;

   str = gen_collection_name (prefix);
   ret = mongoc_client_get_collection (client, "test", str);
   bson_free (str);

   return ret;
}


void
create_unique_index (mongoc_collection_t *collection)
{
   mongoc_index_opt_t opt;
   bool r;

   mongoc_index_opt_init (&opt);
   opt.unique = true;
   r = mongoc_collection_create_index (collection, tmp_bson ("{'a': 1}"),
                                       &opt, NULL);
   assert (r);
}


static void
test_bulk (void)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;
   bson_error_t error;
   bson_t reply;
   bson_t child;
   bson_t del;
   bson_t up;
   bson_t doc = BSON_INITIALIZER;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_bulk");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   mongoc_bulk_operation_insert (bulk, &doc);
   mongoc_bulk_operation_insert (bulk, &doc);
   mongoc_bulk_operation_insert (bulk, &doc);
   mongoc_bulk_operation_insert (bulk, &doc);

   bson_init (&up);
   bson_append_document_begin (&up, "$set", -1, &child);
   bson_append_int32 (&child, "hello", -1, 123);
   bson_append_document_end (&up, &child);
   mongoc_bulk_operation_update (bulk, &doc, &up, false);
   bson_destroy (&up);

   bson_init (&del);
   BSON_APPEND_INT32 (&del, "hello", 123);
   mongoc_bulk_operation_remove (bulk, &del);
   bson_destroy (&del);

   r = mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 4,"
                         " 'nRemoved':  4,"
                         " 'nMatched':  4,"
                         " 'nUpserted': 0}");

   check_n_modified (has_write_cmds, &reply, 4);
   ASSERT_COUNT (0, collection);

   bson_destroy (&reply);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_insert (bool ordered)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;
   bson_error_t error;
   bson_t reply;
   bson_t doc = BSON_INITIALIZER;
   bson_t query = BSON_INITIALIZER;
   bool r;
   mongoc_cursor_t *cursor;
   const bson_t *inserted_doc;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_insert");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);
   assert (bulk->ordered == ordered);

   mongoc_bulk_operation_insert (bulk, &doc);
   mongoc_bulk_operation_insert (bulk, &doc);

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 2,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 0}");

   check_n_modified (has_write_cmds, &reply, 0);

   bson_destroy (&reply);
   ASSERT_COUNT (2, collection);

   cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0,
                                    &query, NULL, NULL);
   assert (cursor);

   while (mongoc_cursor_next (cursor, &inserted_doc)) {
      assert (oid_created_on_client (inserted_doc));
   }

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   mongoc_cursor_destroy (cursor);
   bson_destroy (&query);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   bson_destroy (&doc);
}


static void
test_insert_ordered (void)
{
   test_insert (true);
}


static void
test_insert_unordered (void)
{
   test_insert (false);
}


static void
test_insert_check_keys (void)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;
   bson_t *doc;
   bson_t reply;
   bson_error_t error;
   bool r;
   char *json_query;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_insert_check_keys");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   doc = tmp_bson ("{'$dollar': 1}");
   mongoc_bulk_operation_insert (bulk, doc);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);
   assert (error.code);
   /* TODO: CDRIVER-648, assert nInserted == 0 */
   json_query = bson_strdup_printf ("{'nRemoved':  0,"
                                    " 'nMatched':  0,"
                                    " 'nUpserted': 0,"
                                    " 'writeErrors.0.index': 0,"
                                    " 'writeErrors.0.code': %d}",
                                    error.code);
   ASSERT_MATCH (&reply, json_query);
   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (0, collection);

   bson_free (json_query);
   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_upsert (bool ordered)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;

   bson_error_t error;
   bson_t reply;
   bson_t *sel;
   bson_t *doc;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_upsert");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   sel = tmp_bson ("{'_id': 1234}");
   doc = tmp_bson ("{'$set': {'hello': 'there'}}");

   mongoc_bulk_operation_update (bulk, sel, doc, true);

   r = mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 1,"
                         " 'upserted':  [{'index': 0, '_id': 1234}],"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (1, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   /* non-upsert, no matches */
   sel = tmp_bson ("{'_id': 2}");
   doc = tmp_bson ("{'$set': {'hello': 'there'}}");

   mongoc_bulk_operation_update (bulk, sel, doc, false);
   r = mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 0,"
                         " 'upserted':  {'$exists': false},"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (1, collection);  /* doc remains from previous operation */

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_upsert_ordered (void)
{
   test_upsert (true);
}


static void
test_upsert_unordered (void)
{
   test_upsert (false);
}


static void
test_upserted_index (bool ordered)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;

   bson_error_t error;
   bson_t reply;
   bson_t *emp = tmp_bson ("{}");
   bson_t *inc = tmp_bson ("{'$inc': {'b': 1}}");
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_upserted_index");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);
   
   mongoc_bulk_operation_insert (bulk, emp);
   mongoc_bulk_operation_insert (bulk, emp);
   mongoc_bulk_operation_remove (bulk, tmp_bson ("{'i': 2}"));  
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 3}"),
                                 inc, false);
   /* upsert */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 4}"),
                                 inc, true);
   mongoc_bulk_operation_remove (bulk, tmp_bson ("{'i': 5}"));  
   mongoc_bulk_operation_remove_one (bulk, tmp_bson ("{'i': 6}"));  
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 7}"), emp, false);
   /* upsert */
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 8}"), emp, true);
   /* upsert */
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 9}"), emp, true);
   mongoc_bulk_operation_remove (bulk, tmp_bson ("{'i': 10}"));
   mongoc_bulk_operation_insert (bulk, emp);
   mongoc_bulk_operation_insert (bulk, emp);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 13}"),
                                 inc, false);
   /* upsert */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 14}"),
                                 inc, true);
   mongoc_bulk_operation_insert (bulk, emp);
   /* upserts */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 16}"),
                                 inc, true);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 17}"),
                                 inc, true);
   /* non-upsert */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 18}"),
                                 inc, false);
   /* upserts */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 19}"),
                                 inc, true);
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 20}"), emp, true);
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 21}"), emp, true);
   mongoc_bulk_operation_replace_one (bulk, tmp_bson ("{'i': 22}"), emp, true);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'i': 23}"),
                                 inc, true);
   /* non-upsert */
   mongoc_bulk_operation_update_one (bulk,
                                     tmp_bson ("{'i': 24}"),
                                     inc, false);
   /* upsert */
   mongoc_bulk_operation_update_one (bulk,
                                     tmp_bson ("{'i': 25}"),
                                     inc, true);
   /* non-upserts */
   mongoc_bulk_operation_remove (bulk, tmp_bson ("{'i': 26}"));
   mongoc_bulk_operation_remove (bulk, tmp_bson ("{'i': 27}"));
   mongoc_bulk_operation_update_one (bulk,
                                     tmp_bson ("{'i': 28}"),
                                     inc, false);
   mongoc_bulk_operation_update_one (bulk,
                                     tmp_bson ("{'i': 29}"),
                                     inc, false);
   /* each update modifies existing 16 docs, but only increments index by one */
   mongoc_bulk_operation_update (bulk, emp, inc, false);
   mongoc_bulk_operation_update (bulk, emp, inc, false);
   /* upsert */
   mongoc_bulk_operation_update_one (bulk,
                                     tmp_bson ("{'i': 32}"),
                                     inc, true);



   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   if (!r) {
      fprintf (stderr, "bulk failed: %s\n", error.message);
      abort ();
   }

   ASSERT_MATCH (&reply, "{'nInserted':          5,"
                         " 'nRemoved':           0,"
                         " 'nMatched':          34,"
                         " 'nUpserted':         13,"
                         " 'upserted.0.index':   4,"
                         " 'upserted.1.index':   8,"
                         " 'upserted.2.index':   9,"
                         " 'upserted.3.index':  14,"
                         " 'upserted.4.index':  16,"
                         " 'upserted.5.index':  17,"
                         " 'upserted.6.index':  19,"
                         " 'upserted.7.index':  20,"
                         " 'upserted.8.index':  21,"
                         " 'upserted.9.index':  22,"
                         " 'upserted.10.index': 23,"
                         " 'upserted.11.index': 25,"
                         " 'upserted.12.index': 32,"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 34);
   ASSERT_COUNT (18, collection);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_upserted_index_ordered (void)
{
   test_upserted_index (true);
}


static void
test_upserted_index_unordered (void)
{
   test_upserted_index (false);
}


static void
test_update_one (bool ordered)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;

   bson_error_t error;
   bson_t reply;
   bson_t *sel;
   bson_t *doc;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_update_one");
   assert (collection);

   doc = bson_new ();
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc,
                                 NULL, NULL);
   assert (r);
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc,
                                 NULL, NULL);
   assert (r);
   bson_destroy (doc);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   sel = tmp_bson ("{}");
   doc = tmp_bson ("{'$set': {'hello': 'there'}}");
   mongoc_bulk_operation_update_one (bulk, sel, doc, true);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  1,"
                         " 'nUpserted': 0,"
                         " 'upserted': {'$exists': false},"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 1);
   ASSERT_COUNT (2, collection);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_update_one_ordered ()
{
   test_update_one (true);
}


static void
test_update_one_unordered ()
{
   test_update_one (false);
}


static void
test_replace_one (bool ordered)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;

   bson_error_t error;
   bson_t reply;
   bson_t *sel;
   bson_t *doc;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_replace_one");
   assert (collection);

   doc = bson_new ();
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc,
                                 NULL, NULL);
   assert (r);
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc,
                                 NULL, NULL);
   assert (r);
   bson_destroy (doc);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   sel = tmp_bson ("{}");
   doc = tmp_bson ("{'hello': 'there'}");
   mongoc_bulk_operation_replace_one (bulk, sel, doc, true);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  1,"
                         " 'nUpserted': 0,"
                         " 'upserted': {'$exists': false},"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 1);
   ASSERT_COUNT (2, collection);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_upsert_large ()
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;
   bson_t *sel = tmp_bson ("{'_id': 1}");
   bson_t doc = BSON_INITIALIZER;
   bson_t child = BSON_INITIALIZER;
   bson_error_t error;
   bson_t reply;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_upsert_large");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   bson_append_document_begin (&doc, "$set", -1, &child);
   assert (bson_append_utf8 (&child, "x", -1,
                             huge_string (client),
                             huge_string_length (client)));
   bson_append_document_end (&doc, &child);

   mongoc_bulk_operation_update (bulk, sel, &doc, true);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 1,"
                         " 'upserted':  [{'index': 0, '_id': 1}],"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (1, collection);

   bson_destroy (&reply);
   bson_destroy (&doc);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_replace_one_ordered ()
{
   test_replace_one (true);
}


static void
test_replace_one_unordered ()
{
   test_replace_one (false);
}


static void
test_update (bool ordered)
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   bson_t *docs_inserted[] = {
      tmp_bson ("{'a': 1}"),
      tmp_bson ("{'a': 2}"),
      tmp_bson ("{'a': 3, 'foo': 'bar'}"),
   };
   int i;
   mongoc_bulk_operation_t *bulk;
   bson_error_t error;
   bson_t reply;
   bson_t *sel;
   bson_t *bad_update_doc = tmp_bson ("{'foo': 'bar'}");
   bson_t *update_doc;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_update");
   assert (collection);

   for (i = 0; i < sizeof docs_inserted / sizeof (bson_t *); i++) {
      assert (mongoc_collection_insert (collection, MONGOC_INSERT_NONE,
                                        docs_inserted[i], NULL, NULL));
   }

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   /* update doc without $-operators rejected */
   sel = tmp_bson ("{'a': {'$gte': 2}}");
   suppress_one_message ();
   mongoc_bulk_operation_update (bulk, sel, bad_update_doc, false);
   ASSERT_CMPINT (0, ==, (int)bulk->commands.len);

   update_doc = tmp_bson ("{'$set': {'foo': 'bar'}}");
   mongoc_bulk_operation_update (bulk, sel, update_doc, false);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  2,"
                         " 'nUpserted': 0,"
                         " 'upserted':  {'$exists': false},"
                         " 'writeErrors': []}");

   /* one doc already had "foo": "bar" */
   check_n_modified (has_write_cmds, &reply, 1);
   ASSERT_COUNT (3, collection);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   mongoc_bulk_operation_destroy (bulk);
   bson_destroy (&reply);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_update_ordered (void)
{
   test_update (true);
}


static void
test_update_unordered (void)
{
   test_update (false);
}


static void
test_index_offset (void)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bool has_write_cmds;
   bson_error_t error;
   bson_t reply;
   bson_t *sel;
   bson_t *doc;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_index_offset");
   assert (collection);

   doc = tmp_bson ("{}");
   BSON_APPEND_INT32 (doc, "_id", 1234);
   r = mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc,
                                 NULL, &error);
   assert (r);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   sel = tmp_bson ("{'_id': 1234}");
   doc = tmp_bson ("{'$set': {'hello': 'there'}}");

   mongoc_bulk_operation_remove_one (bulk, sel);
   mongoc_bulk_operation_update (bulk, sel, doc, true);

   r = mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  1,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 1,"
                         " 'upserted': [{'index': 1, '_id': 1234}],"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (1, collection);

   bson_destroy (&reply);

   r = mongoc_collection_drop (collection, &error);
   assert (r);

   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_single_ordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_single_ordered_bulk");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'a': 1}"));
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'a': 1}"),
                                 tmp_bson ("{'$set': {'b': 1}}"), false);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'a': 2}"),
                                 tmp_bson ("{'$set': {'b': 2}}"), true);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'a': 3}"));
   mongoc_bulk_operation_remove (bulk,
                                 tmp_bson ("{'a': 3}"));
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nMatched':  1,"
                         " 'nUpserted': 1,"
                         " 'nInserted': 2,"
                         " 'nRemoved':  1,"
                         " 'upserted.0.index': 2,"
                         " 'upserted.0._id':   {'$exists': true}}");

   check_n_modified (has_write_cmds, &reply, 1);
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_insert_continue_on_error ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t *doc0 = tmp_bson ("{'a': 1}");
   bson_t *doc1 = tmp_bson ("{'a': 2}");
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_insert_continue_on_error");
   assert (collection);

   create_unique_index (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   mongoc_bulk_operation_insert (bulk, doc0);
   mongoc_bulk_operation_insert (bulk, doc0);
   mongoc_bulk_operation_insert (bulk, doc1);
   mongoc_bulk_operation_insert (bulk, doc1);
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);

   /* TODO: CDRIVER-654, assert nInserted == 2, not 4 */
   ASSERT_MATCH (&reply, "{'nMatched':  0,"
                         " 'nUpserted': 0,"
/*
 *                       " 'nInserted': 2,"
 */
                         " 'nRemoved':  0,"
                         " 'writeErrors.0.index': 1,"
                         " 'writeErrors.1.index': 3}");

   check_n_modified (has_write_cmds, &reply, 0);
   assert_error_count (2, &reply);
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_update_continue_on_error ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t *doc0 = tmp_bson ("{'a': 1}");
   bson_t *doc1 = tmp_bson ("{'a': 2}");
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_update_continue_on_error");
   assert (collection);

   create_unique_index (collection);
   mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc0, NULL, NULL);
   mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc1, NULL, NULL);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   /* succeeds */
   mongoc_bulk_operation_update (bulk,
                                 doc0,
                                 tmp_bson ("{'$inc': {'b': 1}}"), false);
   /* fails */
   mongoc_bulk_operation_update (bulk,
                                 doc0,
                                 tmp_bson ("{'$set': {'a': 2}}"), false);
   /* succeeds */
   mongoc_bulk_operation_update (bulk,
                                 doc1,
                                 tmp_bson ("{'$set': {'b': 2}}"), false);

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);

   /* TODO: CDRIVER-654, assert nInserted == 2, not 4 */
   ASSERT_MATCH (&reply, "{'nMatched':  2,"
                         " 'nUpserted': 0,"
                         " 'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'writeErrors.0.index': 1}");

   check_n_modified (has_write_cmds, &reply, 2);
   assert_error_count (1, &reply);
   ASSERT_COUNT (2, collection);
   ASSERT_CMPINT (
      1,
      ==,
      (int)mongoc_collection_count (collection, MONGOC_QUERY_NONE,
                                    tmp_bson ("{'b': 2}"), 0, 0, NULL, NULL));

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_remove_continue_on_error ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t *doc0 = tmp_bson ("{'a': 1}");
   bson_t *doc1 = tmp_bson ("{'a': 2}");
   bson_t *doc2 = tmp_bson ("{'a': 3}");
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_remove_continue_on_error");
   assert (collection);

   mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc0, NULL, NULL);
   mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc1, NULL, NULL);
   mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc2, NULL, NULL);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   /* succeeds */
   mongoc_bulk_operation_remove_one (bulk, doc0);
   /* fails */
   mongoc_bulk_operation_remove_one (bulk, tmp_bson ("{'a': {'$bad': 1}}"));
   /* succeeds */
   mongoc_bulk_operation_remove_one (bulk, doc1);

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);

   ASSERT_MATCH (&reply, "{'nMatched':  0,"
                         " 'nUpserted': 0,"
                         " 'nInserted': 0,"
                         " 'nRemoved':  2,"
                         " 'writeErrors.0.index': 1}");

   check_n_modified (has_write_cmds, &reply, 0);
   assert_error_count (1, &reply);
   ASSERT_COUNT (1, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_single_error_ordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_single_error_ordered_bulk");
   assert (collection);

   create_unique_index (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 1, 'a': 1}"));
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 2}"),
                                 tmp_bson ("{'$set': {'a': 1}}"), true);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 3, 'a': 2}"));

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);

   /* TODO: CDRIVER-651, assert contents of the 'op' field */
   /* TODO: CDRIVER-656, assert index is 1 */
   ASSERT_MATCH (&reply, "{'nMatched':  0,"
                         " 'nUpserted': 0,"
                         " 'nInserted': 1,"
                         " 'nRemoved':  0,"
/*
                         " 'writeErrors.0.index':  1,"
*/
                         " 'writeErrors.0.code':   {'$exists': true},"
                         " 'writeErrors.0.errmsg': {'$exists': true}"
/*
 *                       " 'writeErrors.0.op':     ...,"
 */
                         "}");
   assert_error_count (1, &reply);
   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (1, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_multiple_error_ordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client,
                                     "test_multiple_error_ordered_bulk");
   assert (collection);

   create_unique_index (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 1, 'a': 1}"));
   /* succeeds */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 3}"),
                                 tmp_bson ("{'$set': {'a': 2}}"), true);
   /* fails, duplicate value for 'a' */
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 2}"),
                                 tmp_bson ("{'$set': {'a': 1}}"), true);
   /* not attempted, bulk is already aborted */
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 4, 'a': 3}"));

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);
   assert (error.code);

   /* TODO: CDRIVER-651, assert contents of the 'op' field */
   /* TODO: CDRIVER-656, assert writeErrors index is 1 */
   ASSERT_MATCH (&reply, "{'nMatched':  0,"
                         " 'nUpserted': 1,"
                         " 'nInserted': 1,"
                         " 'nRemoved':  0,"
/*
                         " 'writeErrors.0.index':  1,"
*/
                         " 'writeErrors.0.errmsg': {'$exists': true}"
/*
 *                       " 'writeErrors.0.op': {'q': {'b': 2}, 'u': {'$set': {'a': 1}}, 'multi': false}"
 */
                         "}");
   check_n_modified (has_write_cmds, &reply, 0);
   assert_error_count (1, &reply);
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_single_unordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "test_single_unordered_bulk");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'a': 1}"));
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'a': 1}"),
                                 tmp_bson ("{'$set': {'b': 1}}"), false);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'a': 2}"),
                                 tmp_bson ("{'$set': {'b': 2}}"), true);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'a': 3}"));
   mongoc_bulk_operation_remove (bulk,
                                 tmp_bson ("{'a': 3}"));
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   ASSERT_MATCH (&reply, "{'nMatched': 1,"
                         " 'nUpserted': 1,"
                         " 'nInserted': 2,"
                         " 'nRemoved': 1,"
                         " 'upserted.0.index': 2,"
                         " 'upserted.0._id': {'$exists': true},"
                         " 'writeErrors': []}");
   check_n_modified (has_write_cmds, &reply, 1);
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_single_error_unordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client,
                                     "test_single_error_unordered_bulk");
   assert (collection);

   create_unique_index (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 1, 'a': 1}"));
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 2}"),
                                 tmp_bson ("{'$set': {'a': 1}}"), true);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 3, 'a': 2}"));
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);

   assert (!r);
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);
   assert (error.code);

   /* TODO: CDRIVER-651, assert contents of the 'op' field */
   /* TODO: CDRIVER-656, assert writeErrors index is 1 */
   ASSERT_MATCH (&reply, "{'nMatched': 0,"
                         " 'nUpserted': 0,"
                         " 'nInserted': 2,"
                         " 'nRemoved': 0,"
/*
                         " 'writeErrors.0.op': {'q': {'b': 2},"
                         " 'writeErrors.0.index': 1,"
 */
                         " 'writeErrors.0.code': {'$exists': true},"
                         " 'writeErrors.0.errmsg': {'$exists': true}}");
   assert_error_count (1, &reply);
   check_n_modified (has_write_cmds, &reply, 0);
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_multiple_error_unordered_bulk ()
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client,
                                     "test_multiple_error_unordered_bulk");
   assert (collection);

   create_unique_index (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 1, 'a': 1}"));
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 2}"),
                                 tmp_bson ("{'$set': {'a': 3}}"), true);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 3}"),
                                 tmp_bson ("{'$set': {'a': 4}}"), true);
   mongoc_bulk_operation_update (bulk,
                                 tmp_bson ("{'b': 4}"),
                                 tmp_bson ("{'$set': {'a': 3}}"), true);
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 5, 'a': 2}"));
   mongoc_bulk_operation_insert (bulk,
                                 tmp_bson ("{'b': 6, 'a': 1}"));
   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);

   assert (!r);
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);
   assert (error.code);

   /* Assume the update at index 1 runs before the update at index 3,
    * although the spec does not require it. Same for inserts.
    */
   /* TODO: CDRIVER-651, assert contents of the 'op' field */
   ASSERT_MATCH (&reply, "{'nMatched': 0,"
                         " 'nUpserted': 2,"
                         " 'nInserted': 2,"
                         " 'nRemoved': 0,"
                         " 'upserted.0.index': 1,"
                         " 'upserted.0._id':   {'$exists': true},"
                         " 'upserted.1.index': 2,"
                         " 'upserted.1._id':   {'$exists': true},"
                         " 'writeErrors.0.index': 3,"
/*
 *                       " 'writeErrors.0.op': {'q': {'b': 4}, 'u': {'$set': {'a': 3}}, 'multi': false, 'upsert': true}},"
 */
                         " 'writeErrors.0.code':  {'$exists': true},"
                         " 'writeErrors.0.error': {'$exists': true},"
                         " 'writeErrors.1.index': 5,"
/*
 *                       " 'writeErrors.1.op': {'_id': '...', 'b': 6, 'a': 1},"
 */
                         " 'writeErrors.1.code':  {'$exists': true},"
                         " 'writeErrors.1.error': {'$exists': true}}");
   assert_error_count (2, &reply);
   check_n_modified (has_write_cmds, &reply, 0);

   /*
    * assume the update at index 1 runs before the update at index 3,
    * although the spec does not require it. Same for inserts.
    */
   ASSERT_COUNT (2, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_large_inserts_ordered ()
{
   mongoc_client_t *client;
   bson_t *huge_doc;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;
   bson_t *big_doc;
   bson_iter_t iter;
   int i;

   client = test_framework_client_new (NULL);
   assert (client);

   huge_doc = BCON_NEW ("a", BCON_INT32 (1));
   bson_append_utf8 (huge_doc, "long-key-to-make-this-fail", -1,
                     huge_string (client), huge_string_length (client));

   collection = get_test_collection (client, "test_large_inserts_ordered");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);
   mongoc_bulk_operation_insert (bulk, tmp_bson ("{'b': 1, 'a': 1}"));
   mongoc_bulk_operation_insert (bulk, huge_doc);
   mongoc_bulk_operation_insert (bulk, tmp_bson ("{'b': 2, 'a': 2}"));

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);
   /* TODO: CDRIVER-662, should always be MONGOC_ERROR_BSON */
   ASSERT_CMPINT (error.domain, ==, MONGOC_ERROR_COMMAND);
   assert (error.code);

   /* TODO: CDRIVER-654, assert nInserted == 1 */
   /*assert_n_inserted (1, &reply);*/
   /*ASSERT_COUNT (1, collection);*/

   mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, tmp_bson ("{}"),
                             NULL, NULL);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   big_doc = tmp_bson ("{'a': 1}");
   bson_append_utf8 (big_doc, "big", -1, four_mb_string (), gFourMB);
   bson_iter_init_find (&iter, big_doc, "a");

   for (i = 1; i <= 6; i++) {
      bson_iter_overwrite_int32 (&iter, i);
      mongoc_bulk_operation_insert (bulk, big_doc);
   }

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);
   assert_n_inserted (6, &reply);
   ASSERT_COUNT (6, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   bson_destroy (huge_doc);
   mongoc_client_destroy (client);
}


static void
test_large_inserts_unordered ()
{
   mongoc_client_t *client;
   bson_t *huge_doc;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;
   bson_t *big_doc;
   bson_iter_t iter;
   int i;

   client = test_framework_client_new (NULL);
   assert (client);

   huge_doc = BCON_NEW ("a", BCON_INT32 (1));
   bson_append_utf8 (huge_doc, "long-key-to-make-this-fail", -1,
                     huge_string (client), huge_string_length (client));

   collection = get_test_collection (client, "test_large_inserts_unordered");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   assert (bulk);
   mongoc_bulk_operation_insert (bulk, tmp_bson ("{'b': 1, 'a': 1}"));
   mongoc_bulk_operation_insert (bulk, huge_doc);
   mongoc_bulk_operation_insert (bulk, tmp_bson ("{'b': 2, 'a': 2}"));

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (!r);
   assert ((error.domain == MONGOC_ERROR_COMMAND) ||
           (error.domain == MONGOC_ERROR_BSON &&
            error.code == MONGOC_ERROR_BSON_INVALID));

   /* TODO: CDRIVER-654, assert nInserted == 2 */
   /*assert_n_inserted (1, &reply);*/
   ASSERT_COUNT (2, collection);

   mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, tmp_bson ("{}"),
                             NULL, NULL);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   bulk = mongoc_collection_create_bulk_operation (collection, false, NULL);
   assert (bulk);

   big_doc = tmp_bson ("{'a': 1}");
   bson_append_utf8 (big_doc, "big", -1, four_mb_string (), gFourMB);
   bson_iter_init_find (&iter, big_doc, "a");

   for (i = 1; i <= 6; i++) {
      bson_iter_overwrite_int32 (&iter, i);
      mongoc_bulk_operation_insert (bulk, big_doc);
   }

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);
   assert_n_inserted (6, &reply);
   ASSERT_COUNT (6, collection);

   bson_destroy (huge_doc);
   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_numerous_inserts ()
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_t reply;
   bson_error_t error;
   bool r;
   int n_docs = 2100;
   bson_t *doc = tmp_bson ("{}");
   int i;

   client = test_framework_client_new (NULL);
   assert (client);

   collection = get_test_collection (client, "test_numerous_inserts");
   assert (collection);

   /* ensure we don't exceed server's 1000-document bulk size limit */
   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);

   for (i = 0; i < n_docs; i++) {
      mongoc_bulk_operation_insert (bulk, doc);
   }

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   assert_n_inserted (n_docs, &reply);
   ASSERT_COUNT (n_docs, collection);

   /* same with ordered bulk */
   mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, tmp_bson ("{}"),
                             NULL, NULL);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
   assert (bulk);

   for (i = 0; i < n_docs; i++) {
      mongoc_bulk_operation_insert (bulk, doc);
   }

   r = (bool)mongoc_bulk_operation_execute (bulk, &reply, &error);
   assert (r);

   assert_n_inserted (n_docs, &reply);
   ASSERT_COUNT (n_docs, collection);

   bson_destroy (&reply);
   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_bulk_edge_over_1000 (void)
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t * bulk_op;
   mongoc_write_concern_t * wc = mongoc_write_concern_new();
   bson_iter_t iter, error_iter, index;
   bson_t doc, result;
   bson_error_t error;
   int i;

   client = test_framework_client_new (NULL);
   assert (client);

   collection = get_test_collection (client, "OVER_1000");
   assert (collection);

   mongoc_write_concern_set_w(wc, 1);

   bulk_op = mongoc_collection_create_bulk_operation(collection, false, wc);

   for (i = 0; i < 1010; i+=3) {
      bson_init(&doc);
      bson_append_int32(&doc, "_id", -1, i);

      mongoc_bulk_operation_insert(bulk_op, &doc);

      bson_destroy(&doc);
   }

   mongoc_bulk_operation_execute(bulk_op, NULL, &error);

   mongoc_bulk_operation_destroy(bulk_op);

   bulk_op = mongoc_collection_create_bulk_operation(collection, false, wc);
   for (i = 0; i < 1010; i++) {
      bson_init(&doc);
      bson_append_int32(&doc, "_id", -1, i);

      mongoc_bulk_operation_insert(bulk_op, &doc);

      bson_destroy(&doc);
   }

   mongoc_bulk_operation_execute(bulk_op, &result, &error);

   bson_iter_init_find(&iter, &result, "writeErrors");
   assert(bson_iter_recurse(&iter, &error_iter));
   assert(bson_iter_next(&error_iter));

   for (i = 0; i < 1010; i+=3) {
      assert(bson_iter_recurse(&error_iter, &index));
      assert(bson_iter_find(&index, "index"));
      if (bson_iter_int32(&index) != i) {
          fprintf(stderr, "index should be %d, but is %d\n", i, bson_iter_int32(&index));
      }
      assert(bson_iter_int32(&index) == i);
      bson_iter_next(&error_iter);
   }

   mongoc_bulk_operation_destroy(bulk_op);
   bson_destroy (&result);

   mongoc_write_concern_destroy(wc);

   mongoc_collection_destroy(collection);
   mongoc_client_destroy(client);
}

static void
test_bulk_edge_case_372 (bool ordered)
{
   mongoc_client_t *client;
   bool has_write_cmds;
   mongoc_collection_t *collection;
   mongoc_bulk_operation_t *bulk;
   bson_error_t error;
   bson_iter_t iter;
   bson_iter_t citer;
   bson_t *selector;
   bson_t *update;
   bson_t reply;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);
   has_write_cmds = server_has_write_commands (client);

   collection = get_test_collection (client, "CDRIVER_372");
   assert (collection);

   bulk = mongoc_collection_create_bulk_operation (collection, ordered, NULL);
   assert (bulk);

   selector = tmp_bson ("{'_id': 0}");
   update = tmp_bson ("{'$set': {'a': 0}}");
   mongoc_bulk_operation_update_one (bulk, selector, update, true);

   selector = tmp_bson ("{'a': 1}");
   update = tmp_bson ("{'_id': 1}");
   mongoc_bulk_operation_replace_one (bulk, selector, update, true);

   if (has_write_cmds) {
      /* This is just here to make the counts right in all cases. */
      selector = tmp_bson ("{'_id': 2}");
      update = tmp_bson ("{'_id': 2}");
      mongoc_bulk_operation_replace_one (bulk, selector, update, true);
   } else {
      /* This case is only possible in MongoDB versions before 2.6. */
      selector = tmp_bson ("{'_id': 3}");
      update = tmp_bson ("{'_id': 2}");
      mongoc_bulk_operation_replace_one (bulk, selector, update, true);
   }

   r = mongoc_bulk_operation_execute (bulk, &reply, &error);
   if (!r) fprintf (stderr, "%s\n", error.message);
   assert (r);

#if 0
   printf ("%s\n", bson_as_json (&reply, NULL));
#endif

   ASSERT_MATCH (&reply, "{'nInserted': 0,"
                         " 'nRemoved':  0,"
                         " 'nMatched':  0,"
                         " 'nUpserted': 3,"
                         " 'upserted': ["
                         "     {'index': 0, '_id': 0},"
                         "     {'index': 1, '_id': 1},"
                         "     {'index': 2, '_id': 2}"
                         " ],"
                         " 'writeErrors': []}");

   check_n_modified (has_write_cmds, &reply, 0);

   assert (bson_iter_init_find (&iter, &reply, "upserted") &&
           BSON_ITER_HOLDS_ARRAY (&iter) &&
           bson_iter_recurse (&iter, &citer));

   bson_destroy (&reply);

   mongoc_collection_drop (collection, NULL);

   mongoc_bulk_operation_destroy (bulk);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


static void
test_bulk_edge_case_372_ordered ()
{
   test_bulk_edge_case_372 (true);
}


static void
test_bulk_edge_case_372_unordered ()
{
   test_bulk_edge_case_372 (false);
}


static void
test_bulk_new (void)
{
   mongoc_bulk_operation_t *bulk;
   mongoc_collection_t *collection;
   mongoc_client_t *client;
   bson_error_t error;
   bson_t empty = BSON_INITIALIZER;
   bool r;

   client = test_framework_client_new (NULL);
   assert (client);

   collection = get_test_collection (client, "bulk_new");
   assert (collection);

   bulk = mongoc_bulk_operation_new (true);
   mongoc_bulk_operation_destroy (bulk);

   bulk = mongoc_bulk_operation_new (true);

   r = mongoc_bulk_operation_execute (bulk, NULL, &error);
   assert (!r);
   assert (error.domain = MONGOC_ERROR_CLIENT);
   assert (error.code = MONGOC_ERROR_COMMAND_INVALID_ARG);

   mongoc_bulk_operation_set_database (bulk, "test");
   r = mongoc_bulk_operation_execute (bulk, NULL, &error);
   assert (!r);
   assert (error.domain = MONGOC_ERROR_CLIENT);
   assert (error.code = MONGOC_ERROR_COMMAND_INVALID_ARG);

   mongoc_bulk_operation_set_collection (bulk, "test");
   r = mongoc_bulk_operation_execute (bulk, NULL, &error);
   assert (!r);
   assert (error.domain = MONGOC_ERROR_CLIENT);
   assert (error.code = MONGOC_ERROR_COMMAND_INVALID_ARG);

   mongoc_bulk_operation_set_client (bulk, client);
   r = mongoc_bulk_operation_execute (bulk, NULL, &error);
   assert (!r);
   assert (error.domain = MONGOC_ERROR_CLIENT);
   assert (error.code = MONGOC_ERROR_COMMAND_INVALID_ARG);

   mongoc_bulk_operation_insert (bulk, &empty);
   r = mongoc_bulk_operation_execute (bulk, NULL, &error);
   assert (r);

   mongoc_bulk_operation_destroy (bulk);

   mongoc_collection_drop (collection, NULL);

   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
}


void
test_bulk_install (TestSuite *suite)
{
   test_bulk_init ();
   atexit (test_bulk_cleanup);

   TestSuite_Add (suite, "/BulkOperation/basic",
                  test_bulk);
   TestSuite_Add (suite, "/BulkOperation/insert_ordered",
                  test_insert_ordered);
   TestSuite_Add (suite, "/BulkOperation/insert_unordered",
                  test_insert_unordered);
   TestSuite_Add (suite, "/BulkOperation/insert_check_keys",
                  test_insert_check_keys);
   TestSuite_Add (suite, "/BulkOperation/update_ordered",
                  test_update_ordered);
   TestSuite_Add (suite, "/BulkOperation/update_unordered",
                  test_update_unordered);
   TestSuite_Add (suite, "/BulkOperation/upsert_ordered",
                  test_upsert_ordered);
   TestSuite_Add (suite, "/BulkOperation/upsert_unordered",
                  test_upsert_unordered);
   TestSuite_Add (suite, "/BulkOperation/upsert_large",
                  test_upsert_large);
   TestSuite_Add (suite, "/BulkOperation/upserted_index_ordered",
                  test_upserted_index_ordered);
   TestSuite_Add (suite, "/BulkOperation/upserted_index_unordered",
                  test_upserted_index_unordered);
   TestSuite_Add (suite, "/BulkOperation/update_one_ordered",
                  test_update_one_ordered);
   TestSuite_Add (suite, "/BulkOperation/update_one_unordered",
                  test_update_one_unordered);
   TestSuite_Add (suite, "/BulkOperation/replace_one_ordered",
                  test_replace_one_ordered);
   TestSuite_Add (suite, "/BulkOperation/replace_one_unordered",
                  test_replace_one_unordered);
   TestSuite_Add (suite, "/BulkOperation/index_offset",
                  test_index_offset);
   TestSuite_Add (suite, "/BulkOperation/single_ordered_bulk",
                  test_single_ordered_bulk);
   TestSuite_Add (suite, "/BulkOperation/insert_continue_on_error",
                  test_insert_continue_on_error);
   TestSuite_Add (suite, "/BulkOperation/update_continue_on_error",
                  test_update_continue_on_error);
   TestSuite_Add (suite, "/BulkOperation/remove_continue_on_error",
                  test_remove_continue_on_error);
   TestSuite_Add (suite, "/BulkOperation/single_error_ordered_bulk",
                  test_single_error_ordered_bulk);
   TestSuite_Add (suite, "/BulkOperation/multiple_error_ordered_bulk",
                  test_multiple_error_ordered_bulk);
   TestSuite_Add (suite, "/BulkOperation/single_unordered_bulk",
                  test_single_unordered_bulk);
   TestSuite_Add (suite, "/BulkOperation/single_error_unordered_bulk",
                  test_single_error_unordered_bulk);

   /* TODO: CDRIVER-653 */
/*
   TestSuite_Add (suite, "/BulkOperation/multiple_error_unordered_bulk",
                  test_multiple_error_unordered_bulk);
*/
   TestSuite_Add (suite, "/BulkOperation/large_inserts_ordered",
                  test_large_inserts_ordered);
   TestSuite_Add (suite, "/BulkOperation/large_inserts_unordered",
                  test_large_inserts_unordered);
   TestSuite_Add (suite, "/BulkOperation/numerous_inserts",
                  test_numerous_inserts);
   TestSuite_Add (suite, "/BulkOperation/CDRIVER-372_ordered",
                  test_bulk_edge_case_372_ordered);
   TestSuite_Add (suite, "/BulkOperation/CDRIVER-372_unordered",
                  test_bulk_edge_case_372_unordered);
   TestSuite_Add (suite, "/BulkOperation/new",
                  test_bulk_new);
   TestSuite_Add (suite, "/BulkOperation/over_1000",
                  test_bulk_edge_over_1000);
}
