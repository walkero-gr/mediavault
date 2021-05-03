/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: guifuncs.h
**
** Date: 10-04-2021 23:45:55
**
************************************************************

*/

#ifndef _GUIFUNCS_H
#define _GUIFUNCS_H

#include <classes/requester.h>


struct Image *MenuImage(CONST_STRPTR name, struct Screen *);
void windowBlocking(struct Window *, Object *, BOOL);
void gadgetBlocking(struct Window *, Object *, BOOL);
void showMsgReq(Object *, CONST_STRPTR, CONST_STRPTR);

#endif
