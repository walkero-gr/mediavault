/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: gui.h
**
** Date: 06-04-2021 16:39:23
**
************************************************************

*/


#ifndef _GUI_H
#define _GUI_H

#include <classes/window.h>

#include <images/label.h>

#include <libraries/gadtools.h>

#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/string.h>

#include <stdio.h>

struct Screen 	*screen;
struct MsgPort 	*appPort;

enum
{
  WID_MAIN,
  WID_ABOUT,
  WID_LAST
};

enum
{
  OID_MAIN,
  OID_ABOUT_MAIN,
  OID_LAST
};

enum
{
  GID_FILTERS_LAYOUT,
  GID_FILTERS_LAYOUT_LINE1,
  GID_FILTERS_LAYOUT_LINE2,
  GID_FILTERS_NAME,
  GID_CHOOSER_GENRES,
  GID_CHOOSER_COUNTRIES,
  GID_CHOOSER_LANGUAGES,
  GID_LAST
};

enum
{
    PROJECT_MENU
};

struct Window *windows[WID_LAST];
struct Gadget *gadgets[GID_LAST];
Object *objects[OID_LAST];


static CONST char* screenTitle = "MediaVault";
static CONST char* windowTitle = "MediaVault";

struct NewMenu mainMenu[] =
{
    { NM_TITLE, "Project ", 0, 0, 0, 0 },
    { NM_ITEM, "About...", "?", 0, 0, 0 },
    { NM_ITEM, "Iconify", "I", 0, 0, 0 },
    { NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
    { NM_ITEM, "Quit...", "Q", 0, 0, 0 },

    { NM_END, NULL, 0, 0, 0, 0 }
}; 

CONST_STRPTR genres[] =
{
  "All Genres",
  "Rock",
  "Classic Rock",
  "Heavy Metal",
  NULL
};

CONST_STRPTR languages[] =
{
  "All Languages",
  "Greek",
  "English",
  "Irish",
  NULL
};

CONST_STRPTR countries[] =
{
  "All Countries",
  "Britain",
  "Greece",
  "Ireland",
  "Sweden",
  "Germany",
  NULL
};

#include "globals.h"

#endif