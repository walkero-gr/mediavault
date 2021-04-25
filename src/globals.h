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

#include <oo/ooNetwork.h>


#include <proto/amijansson.h>
// #include <proto/amisslmaster.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/oo.h>
#include <proto/listbrowser.h>

#include <string.h>
#include <jansson/jansson.h>

#include "version.h"

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;

struct Library *ApplicationBase; 	struct ApplicationIFace 	*IApplication;
struct Library *IntuitionBase; 		struct IntuitionIFace 		*IIntuition;
struct Library *LabelBase;				struct LabelIFace 				*ILabel;
struct Library *LayoutBase;				struct LayoutIFace 				*ILayout;
struct Library *UtilityBase;			struct UtilityIFace 			*IUtility;
struct Library *ListBrowserBase;	struct ListBrowserIFace 	*IListBrowser;
struct Library *OOBase;						struct OOIFace 						*IOO;
//struct Library *SocketBase; 			struct SocketIFace 				*ISocket;
//struct Library *AmiSSLMasterBase; struct AmiSSLMasterIFace	*IAmiSSLMaster;
//struct Library *AmiSSLBase; 			struct AmiSSLIFace				*IAmiSSL;
struct Library *JanssonBase;			struct JanssonIFace 			*IJansson;

Class *LayoutClass;
Class *LabelClass;
Class *ListBrowserClass;

static CONST_STRPTR stack USED = "$STACK:102400";
static CONST_STRPTR version USED = VERSTAG;

void die(const char *s);


#endif
