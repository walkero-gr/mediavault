 /**
  * File:    serverpages.c
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     November 2021
  *
  * Summary of File:
  *
  *   This file contains code for the different
  *   pages for the radio stations sections.
  *
  */ 


#include <gadgets/space.h>

#include "globals.h"
#include "gui.h"
#include "guifuncs.h"
#include "serverpages.h"

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ChooserClass;
extern Class *LabelClass;
extern Class *LayoutClass;
extern Class *ListBrowserClass;
extern Class *SpaceClass;
extern Class *StringClass;
extern Class *TextEditorClass;

Object *buildMediaServersPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Media Servers",

        LAYOUT_AddChild, gadgets[GID_MEDIA_SERVER_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_MEDIA_SERVER_LISTBROWSER,
          GA_RelVerify,               TRUE,
          GA_TabCycle,                TRUE,
          LISTBROWSER_ColumnTitles,   TRUE,
          LISTBROWSER_HorizontalProp, TRUE,
          LISTBROWSER_Separators,     TRUE,
          LISTBROWSER_ShowSelected,   TRUE,
          LISTBROWSER_Striping,       LBS_ROWS,
          LISTBROWSER_SortColumn,     0,
          LISTBROWSER_TitleClickable, TRUE,
          TAG_DONE),

        TAG_DONE);
}
