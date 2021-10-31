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
BOOL sqlAddFavouriteRadio(STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR);
BOOL sqlAddFavouritePodcast(STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR);
BOOL sqlAddListenLaterPodcast(STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, uint8, ULONG, ULONG);
BOOL sqlRemoveFavourite(STRPTR, CONST_STRPTR);
BOOL sqlRemoveListenLater(STRPTR);
BOOL sqlCheckFavouriteExist(STRPTR, CONST_STRPTR);
BOOL sqlCheckListenLaterExist(STRPTR);
BOOL sqlGetFavourites(CONST_STRPTR, int (*)(void *, int, char **, char **));
BOOL sqlGetListenLater(int (*)(void *, int, char **, char **));

#endif
