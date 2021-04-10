/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: globals.h
**
** Date: 01-12-2020 22:32:40
**
************************************************************

*/

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>

#include <string.h>

#include "version.h"

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;

struct Library *ApplicationBase; 	struct ApplicationIFace *IApplication;
struct Library *IntuitionBase; 		struct IntuitionIFace 	*IIntuition;
struct Library *LabelBase;				struct LabelIFace 			*ILabel;
struct Library *LayoutBase;				struct LayoutIFace 			*ILayout;
struct Library *UtilityBase;			struct UtilityIFace 		*IUtility;
struct Library *ListBrowserBase;	struct ListBrowserIFace *IListBrowser;

Class *LayoutClass;
Class *LabelClass;
Class *ListBrowserClass;

static CONST_STRPTR stack USED = "$STACK:102400";
static CONST_STRPTR version USED = VERSTAG;

void die(const char *s);


#endif
