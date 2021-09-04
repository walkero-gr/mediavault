/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: fsfuncs.h
**
** Date: 17-04-2021 10:59:22
**
************************************************************

*/


#ifndef _FSFUNCS_H
#define _FSFUNCS_H

STRPTR getFilePath(STRPTR);
STRPTR getParentPath(STRPTR);
BOOL downloadLatestUpdate(STRPTR);
int32 unarcFile(STRPTR, STRPTR);

#endif
