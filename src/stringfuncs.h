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
STRPTR now(void);
STRPTR SHA1Encrypt(STRPTR);

#endif
