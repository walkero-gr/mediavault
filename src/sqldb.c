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
#include "radiofuncs.h"
#include "stringfuncs.h"
#include "sqldb.h"


static char dbFileName[] = "data1.db";

void printSqliteVer(void) 
{    
  printf("%s\n", sqlite3_libversion());
}
/*
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  if (NotUsed)
    NotUsed = 0;
  
  for (int i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}
*/
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
          "country TEXT, " \
          "language TEXT, " \
          "bitrate TEXT, " \
          "codec TEXT, " \
          "url TEXT, " \
          "image TEXT, " \
          "description TEXT); " \
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
  STRPTR title,
  STRPTR country,
  STRPTR bitrate,
  STRPTR codec,
  STRPTR url
) {
  sqlite3 *db;
  char *err_msg = 0;
  int sqlSize = 1024;

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
    "(uuid, title, added, type, country, bitrate, codec, url) " \
    "VALUES " \
    "('%s', '%s', '%ld', 'radio', '%s', '%s', '%s', '%s');",
    uuid, title, now(), country, bitrate, codec, url
  );

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

BOOL sqlAddFavouritePodcast(
  STRPTR uuid,
  STRPTR title,
  STRPTR language,
  STRPTR image,
  STRPTR description
) {
  sqlite3 *db;
  char *err_msg = 0;
  int sqlSize = 1024;

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
    "(uuid, title, added, type, language, image, description) " \
    "VALUES " \
    "('%s', '%s', '%ld', 'podcast', '%s', '%s', '%s');",
    uuid, title, now(), language, image, description
  );

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

BOOL sqlRemoveFavourite(STRPTR uuid, CONST_STRPTR type)
{
  sqlite3 *db;
  sqlite3_stmt *res;

  int rc = sqlite3_open(dbFileName, &db);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }

  CONST_STRPTR sql = "DELETE FROM favourites " \
        "WHERE uuid = :uuid AND type = :type ";

  rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }

  int uuidIdx = sqlite3_bind_parameter_index(res, ":uuid");
  int typeIdx = sqlite3_bind_parameter_index(res, ":type");
  sqlite3_bind_text(res, uuidIdx, uuid, -1, 0);
  sqlite3_bind_text(res, typeIdx, type, -1, 0);

  sqlite3_step(res);
  sqlite3_finalize(res);
  sqlite3_close(db);

  return TRUE;
}

BOOL sqlCheckExist(STRPTR uuid, CONST_STRPTR type)
{
  BOOL result = FALSE;
  sqlite3 *db;
  //char *err_msg = 0;
  sqlite3_stmt *res;

  int rc = sqlite3_open(dbFileName, &db);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }
  
  CONST_STRPTR sql = "SELECT COUNT(*) AS cnt " \
        "FROM favourites " \
        "WHERE uuid = :uuid AND type = :type " \
        "LIMIT 1";

  rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    //sqlite3_free(err_msg);
    sqlite3_close(db);
    return FALSE;
  }
    
  int uuidIdx = sqlite3_bind_parameter_index(res, ":uuid");
  int typeIdx = sqlite3_bind_parameter_index(res, ":type");
  sqlite3_bind_text(res, uuidIdx, uuid, -1, 0);
  sqlite3_bind_text(res, typeIdx, type, -1, 0);
  
  int step = sqlite3_step(res);
  if (step == SQLITE_ROW) {
    if(sqlite3_column_int(res, 0) == 1)
    {
      result = TRUE;
    }
  }
  sqlite3_finalize(res);
  sqlite3_close(db);

  return result;
}

BOOL sqlGetFavourites(CONST_STRPTR type, int (*resultCallback)(void *, int, char **, char **))
{
  sqlite3 *db;
  char *err_msg = 0;
  int sqlSize = 64;

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
    "SELECT * FROM favourites " \
    "WHERE type = '%s';", type
  );

  rc = sqlite3_exec(db, sql, resultCallback, 0, &err_msg);

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
