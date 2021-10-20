/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: sqldb.c
**
** Date: 14-10-2021 16:38:55
**
************************************************************

*/


#include <sqlite3.h>
#include <stdio.h>

#include <proto/dos.h>

#include "globals.h"
#include "stringfuncs.h"
#include "sqldb.h"


static char dbFileName[] = "data1.db";

void printSqliteVer(void) 
{    
  printf("%s\n", sqlite3_libversion());
}

BOOL createDB(void)
{
  sqlite3 *db;
  char *err_msg = 0;
  
  int rc = sqlite3_open(dbFileName, &db);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }

  CONST_STRPTR sql =
      "CREATE TABLE version ( " \
      "     ver Integer); " \
      "INSERT into version VALUES (\"1\");" \
      "CREATE TABLE favourites ( " \
          "uuid    Text, " \
          "type    Text, " \
          "added TEXT, " \
          "lastepisode TEXT, " \
          "title TEXT, " \
          "url TEXT); " \
      "CREATE TABLE plays ( " \
          "uuid    Text, " \
          "type    Text, " \
          "count   Numeric);";

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return FALSE;
  }
  sqlite3_close(db);

  return TRUE;
}

BOOL sqlAddFavouriteRadio(
  STRPTR uuid,
  STRPTR title
)
{
  sqlite3 *db;
  char *err_msg = 0;
  int sqlSize = 512;

  int rc = sqlite3_open(dbFileName, &db);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }

  STRPTR sql = IExec->AllocVecTags(sizeof(char) * sqlSize,
          AVT_Type,            MEMF_SHARED,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

  snprintf(sql, sqlSize,
    "INSERT INTO favourites " \
    "(uuid, title, added, type) " \
    "VALUES " \
    "('%s', '%s', '%ld', 'radio');",
    uuid, title, now()
  );
  IDOS->Printf("DBG: addFavouriteRadio\n%s\n", sql);

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

  IExec->FreeVec(sql);

  if (rc != SQLITE_OK) {
    //fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return FALSE;
  }
  sqlite3_close(db);

  return TRUE;
}
