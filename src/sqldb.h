/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: sqldb.h
**
** Date: 14-10-2021 16:39:23
**
************************************************************

*/


#ifndef _SQLDB_H
#define _SQLDB_H

void printSqliteVer(void); 
BOOL createDB(void);
BOOL sqlAddFavouriteRadio(STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR);
BOOL sqlCheckExist(STRPTR, CONST_STRPTR);
BOOL sqlGetFavourites(STRPTR, int (*)(void *, int, char **, char **));

#endif
