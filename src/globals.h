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

#include <proto/amijansson.h>
#include <proto/dos.h>
#include <proto/exec.h>   
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <proto/oo.h>          
#include <proto/requester.h>
#include <proto/utility.h>

#include <string.h>
#include <ctype.h>
                          
#include <jansson/jansson.h>
#include <oo/ooNetwork.h>

#include "version.h"

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;

struct Library *ApplicationBase;  struct ApplicationIFace   *IApplication;
struct Library *IntuitionBase;    struct IntuitionIFace     *IIntuition;
struct Library *LabelBase;        struct LabelIFace         *ILabel;
struct Library *LayoutBase;       struct LayoutIFace        *ILayout;
struct Library *UtilityBase;      struct UtilityIFace       *IUtility;
struct Library *ListBrowserBase;  struct ListBrowserIFace   *IListBrowser;
struct Library *OOBase;           struct OOIFace            *IOO;
struct Library *JanssonBase;      struct JanssonIFace       *IJansson;
struct Library *AmiSSLMasterBase;

Class *LayoutClass;
Class *LabelClass;
Class *ListBrowserClass;

static CONST_STRPTR stack USED = "$STACK:102400";
static CONST_STRPTR version USED = VERSTAG;

void die(const char *s);

struct filters
{
  char name[32];
  char genre[32];
  char country[32];
  char language[32];
};

/*
struct stationInfo
{
  char name[64];
  char url[255];
};
*/

#endif
