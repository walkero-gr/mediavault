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

#include <images/bitmap.h>
#include <images/label.h>
#include <intuition/icclass.h> 
#include <intuition/menuclass.h>

#include <libraries/gadtools.h>

#include <gadgets/button.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <gadgets/scroller.h> 
#include <gadgets/string.h>
#include <gadgets/texteditor.h>


enum
{
  WID_MAIN,
  WID_ABOUT,
  WID_LAST
};

enum
{
  OID_MAIN,
  OID_ABOUT,
  OID_LAST
};

enum
{
  GID_FILTERS_LAYOUT,
  GID_FILTERS_NAME,
  GID_FILTER_BUTTON,
  GID_CHOOSER_GENRES,
  GID_CHOOSER_COUNTRIES,
  GID_CHOOSER_LANGUAGES,
  GID_RADIO_LISTBROWSER,
  GID_ABOUT_LAYOUT_ROOT,
  GID_ABOUT_LAYOUT_TEXT,
  GID_ABOUT_TEXT,
  GID_ABOUT_TEXT_SCROLLER,
  GID_ABOUT_BUTTON_OK,
  GID_ABOUT_LOGO,
  GID_MSG_REQ,
  GID_LEFT_SIDEBAR,
  GID_RIGHT_SIDEBAR,
  GID_PAGES,
  GID_PAGE_1,
  GID_PAGE_2,
  GID_PAGE_3,
  GID_RADIO_POPULAR_LISTBROWSER,
  GID_RADIO_TREND_LISTBROWSER,
  GID_LBL_INFO_RADIO,
  GID_LAST
};

enum
{
  MID_PROJECT = 1,  /* Zero is an invalid value for a menu ID */
  MID_ABOUT,
  MID_ICONIFY,
  MID_QUIT,
  MID_LAST
};

struct Window *windows[WID_LAST];
//struct ColumnInfo   *columninfo = NULL;
//struct DrawInfo     *drInfo;
struct List stationsList;

Object *gadgets[GID_LAST];
Object *objects[OID_LAST];
Object *menus[MID_LAST];


void showGUI(void);

#endif
