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

#define CACHE_DIR "PROGDIR:cache/"

void doHTTPRequest(STRPTR);
void cleanupHTTPRequest(void);
STRPTR getResponseBody(void);
LONG getResponseCode(void);
STRPTR getResponseType(void);
void cacheFileFromUrl(STRPTR, STRPTR);

#endif
