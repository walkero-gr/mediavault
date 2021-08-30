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
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/utility.h>

#include <string.h>
#include <ctype.h>
                          
#include <jansson/jansson.h>

#include "version.h"


static CONST_STRPTR stack USED = "$STACK:102400";
static CONST_STRPTR version USED = VERSTAG;

void die(const char *s);

#define CACHE_DIR "PROGDIR:cache/"
#define LOGO_IMAGE "PROGDIR:images/logo_128.png"

struct filters
{
  char name[32];
  char genre[32];
  char country[32];
  char language[32];
};

struct stationInfo
{
  char uuid[42];
  char name[64];
  char url_resolved[255];
  char country[64];
  char language[64];
  char homepage[255];
  char favicon[255];
  char codec[12];
  char bitrate[6];
  ULONG votes;
};


#endif
