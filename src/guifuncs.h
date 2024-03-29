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

struct RenderHook {
    struct Hook hook;
    Object *img;
    UWORD w;
    UWORD h;
    BOOL fill;
};

Object *buildImage(CONST_STRPTR, struct Screen *);
struct Image *MenuImage(CONST_STRPTR, struct Screen *);
void windowBlocking(Object *, BOOL);
void gadgetBlocking(struct Window *, Object *, BOOL);
LONG showMsgReq(Object *, CONST_STRPTR, CONST_STRPTR, ULONG, CONST_STRPTR, ULONG);
int listCount(struct List *);
BOOL appHide(uint32, Object *, uint32);
struct Window *appUnhide(uint32, Object *);
void freeList(struct List *, int);
void showAvatarImage(STRPTR, STRPTR, Object *, Object *, struct RenderHook *);
ULONG renderfunct(struct RenderHook *, Object *, struct gpRender *);
void switchPage(ULONG, Object *);

#endif
