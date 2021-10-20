/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File:
**
** Date: 23-05-2021 13:48:52
**
************************************************************

*/


#ifndef _STRING_FUNCS_H
#define _STRING_FUNCS_H


STRPTR charConv(CONST_STRPTR);
STRPTR urlEncode(STRPTR);
ULONG convertVersionToInt(STRPTR);
ULONG now(void);
ULONG nowUnix(ULONG);
int32 getOffsetUTC(void);
STRPTR SHA1Encrypt(STRPTR);
void stringToLower(STRPTR string);

#endif
