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
#include <proto/timer.h>
#include <proto/timezone.h>
#include <proto/utility.h>

#include <devices/timer.h>

#include <string.h>
#include <ctype.h>

#include <jansson/jansson.h>

#include "version.h"


static CONST_STRPTR stack USED = "$STACK:102400";
static CONST_STRPTR version USED = VERSTAG;

void die(const char *s);

#define IMAGES_PATH     "PROGDIR:images/"
#define LOGO_IMAGE      "logo_128.png"
#define LOGO_IMAGE_BIG  "logo_256.png"

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
  char name[128];
  char url_resolved[255];
  char country[64];
  char language[64];
  char homepage[255];
  char favicon[255];
  char codec[6];
  char bitrate[6];
  ULONG votes;
};

struct podcastInfo
{
  char id[42];
  char title[128];
  char description[512];
  char author[64];
  char language[8];
  char url[255];
  char originalUrl[255];
  char image[255];
};

struct podcastEpisodeInfo
{
  char  id[42];
  char  title[128];
  char  description[512];
  ULONG datePublished;
  char  datePublishedPretty[32];
  char  enclosureUrl[255];
  char  enclosureType[11];
  ULONG duration;
  uint8 episode;
  uint8 season;
  char  feedId[42];
  char  image[255];
};

#endif
