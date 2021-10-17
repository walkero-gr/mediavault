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

#include "version.h"
#include "sqldb.h"


void printSqliteVer(void) 
{    
  printf("%s\n", sqlite3_libversion());
}

BOOL createDB(void)
{
  sqlite3 *db;
  char *err_msg = 0;
  
  int rc = sqlite3_open("data1.db", &db);
  if (rc != SQLITE_OK) {
    //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }

  CONST_STRPTR sql =
      "CREATE TABLE version ( " \
      "     ver Integer); " \
      "INSERT into version VALUES (\"1\");" \
      "CREATE TABLE favorites ( " \
          "uuid    Text, " \
          "type    Text, " \
          "added TEXT, " \
          "lastepisode TEXT, " \
          "title TEXT, " \
          "country TEXT, " \
          "language TEXT, " \
          "bitrate TEXT, " \
          "votes INTEGER, " \
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

