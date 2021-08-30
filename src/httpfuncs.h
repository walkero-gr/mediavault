/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: oofuncs.h
**
** Date: 04-04-2021 22:33:29
**
************************************************************

*/

#ifndef _HTTPFUNCS_H
#define _HTTPFUNCS_H

void doHTTPRequest(STRPTR);
void cacheFileFromUrl(STRPTR, ULONG, STRPTR);

struct memory
{
  char    *body;
  char    *type;
  size_t  size;
  LONG    code;
};


#endif
