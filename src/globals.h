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

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;
struct Library 	*IntuitionBase; 	struct IntuitionIFace 	*IIntuition;
struct Library 	*LabelBase;				struct LabelIFace 			*ILabel;
struct Library 	*LayoutBase;			struct LayoutIFace 			*ILayout;
struct Library 	*UtilityBase;			struct UtilityIFace 		*IUtility;

Class *LayoutClass;
Class *LabelClass;
//Class *PageClass;

static CONST_STRPTR stack USED = "$STACK:102400";

void die(const char *s);


#endif
