 /**
  * File:    radiopages.c
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     October 2021
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
#include "radiopages.h"

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ChooserClass;
extern Class *LabelClass;
extern Class *LayoutClass;
extern Class *ListBrowserClass;
extern Class *SpaceClass;
extern Class *StringClass;
extern Class *TextEditorClass;


Object *buildRadioSearchPage(CONST_STRPTR *genres, CONST_STRPTR *countries, CONST_STRPTR *languages)
{
  return IIntuition->NewObject(LayoutClass, NULL,
        LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

        // START - Top Filter Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_BevelStyle,      BVS_GROUP,
          LAYOUT_Label,           "Filter stations by",

          // Top filter section with the Name Text Box with Label
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
            LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
              //LABEL_DrawInfo, drInfo,
              LABEL_Text, "_Name",
              TAG_END),
            LAYOUT_AddChild, gadgets[GID_FILTERS_NAME] = IIntuition->NewObject(StringClass, NULL,
              GA_ID,                GID_FILTERS_NAME,
              GA_RelVerify,         TRUE,
              GA_TabCycle,          TRUE,
              GA_ActivateKey,       "n",
              STRINGA_MinVisible,   10,
              STRINGA_MaxChars,     40,
              TAG_DONE),
            TAG_DONE),

          // Top filter section with the select boxes
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            // Genres Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Genre",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_GENRES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "g",
                CHOOSER_LabelArray,   genres,
                CHOOSER_MaxLabels,    40,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),

            // Countries Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Country",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_COUNTRIES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "c",
                CHOOSER_LabelArray,   countries,
                CHOOSER_MaxLabels,    40,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),

            // Languages Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Language",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_LANGUAGES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "l",
                CHOOSER_LabelArray,   languages,
                CHOOSER_MaxLabels,    33,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),
            TAG_DONE),

            // Filters Button
            LAYOUT_AddChild, gadgets[GID_FILTER_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,              GID_FILTER_BUTTON,
              GA_Text,            "_Discover",
              GA_TabCycle,        TRUE,
              GA_RelVerify,       TRUE,
              GA_ActivateKey,     "d",
              TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,
        // END - Top Filter Section

        // START - Bottom List Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_AddChild, gadgets[GID_RADIO_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                      GID_RADIO_LISTBROWSER,
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
          TAG_DONE),
          CHILD_MinHeight,  300,
          // END - Bottom List Section
        TAG_DONE);
}

Object *buildRadioFavouritePage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Favourite Radio Stations",

        LAYOUT_AddChild, gadgets[GID_RADIO_FAVOURITE_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_RADIO_FAVOURITE_LISTBROWSER,
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

Object *buildRadioPopularPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Most listened to over 24 hours",

        LAYOUT_AddChild, gadgets[GID_RADIO_POPULAR_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_RADIO_POPULAR_LISTBROWSER,
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

Object *buildRadioTrendPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Trending in the last 24 hours",

        LAYOUT_AddChild, gadgets[GID_RADIO_TREND_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_RADIO_TREND_LISTBROWSER,
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

Object *buildRadioRightSidebar(struct Screen *screen, struct RenderHook *renderhook)
{
  objects[OID_AVATAR_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        GA_ID,              OID_AVATAR_IMAGE,
        IA_Scalable,        FALSE,
        BITMAP_Screen,      screen,
        BITMAP_SourceFile,  LOGO_IMAGE,
        BITMAP_Masking,     TRUE,
        TAG_END);

  if (renderhook && objects[OID_AVATAR_IMAGE])
  {
    renderhook->img  = objects[OID_AVATAR_IMAGE];
    renderhook->w    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Width;
    renderhook->h    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Height;
    renderhook->fill = FALSE;
  }

  return IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

          LAYOUT_AddChild, gadgets[GID_INFO_AVATAR] = IIntuition->NewObject(SpaceClass, NULL,
            GA_ID,                      GID_INFO_AVATAR,
            SPACE_MinWidth,             128,
            SPACE_MinHeight,            128,
            SPACE_RenderHook,           renderhook,
            //SPACE_BevelStyle,           BVS_GROUP,
            GA_Image,                   objects[OID_AVATAR_IMAGE],
            TAG_DONE),
            CHILD_WeightedHeight, 30,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,
            //LAYOUT_BevelStyle,      BVS_GROUP,
            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 70,

            LAYOUT_AddChild, gadgets[GID_RADIO_FAVOURITE_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,                      GID_RADIO_FAVOURITE_BUTTON,
              GA_Disabled,                TRUE,
              BUTTON_Transparent,         TRUE,
              BUTTON_AutoButton,          0,
              BUTTON_BevelStyle,          BVS_NONE,
              BUTTON_Justification,       BCJ_CENTER,
              BUTTON_RenderImage,         objects[OID_FAVOURITES_ADD_IMAGE],
              TAG_DONE),
              //CHILD_MaxHeight, 40,
              CHILD_WeightedWidth, 10,

            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 20,
            TAG_DONE),
            //CHILD_MaxHeight, 30,
            CHILD_WeightedHeight, 5,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,
            //LAYOUT_BevelStyle,      BVS_GROUP,
            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 30,

            LAYOUT_AddChild, gadgets[GID_INFO_PLAY_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,                      GID_INFO_PLAY_BUTTON,
              GA_Disabled,                TRUE,
              GA_HintInfo,                "When this is enabled,\nclick to listen to the\nselected radio station",
              BUTTON_Transparent,         TRUE,
              BUTTON_AutoButton,          0,
              BUTTON_BevelStyle,          BVS_NONE,
              BUTTON_Justification,       BCJ_CENTER,
              BUTTON_RenderImage,         objects[OID_PLAY_IMAGE],
              TAG_DONE),
              CHILD_MaxHeight, 60,
              CHILD_WeightedWidth, 40,

            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 25,
            TAG_DONE),
            //CHILD_MaxHeight, 60,
            CHILD_WeightedHeight, 10,

          LAYOUT_AddChild, gadgets[GID_INFO_RADIO_DATA] = IIntuition->NewObject(TextEditorClass, NULL,
            GA_ID,                      GID_INFO_RADIO_DATA,
            GA_RelVerify,               TRUE,
            GA_TEXTEDITOR_BevelStyle,   BVS_NONE,
            GA_TEXTEDITOR_Contents,     VERS,
            GA_TEXTEDITOR_CursorX,      0,
            GA_TEXTEDITOR_CursorY,      0,
            GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Center,
            GA_TEXTEDITOR_ReadOnly,     TRUE,
            GA_TEXTEDITOR_Transparent,  TRUE,
            TAG_DONE),
            CHILD_WeightedHeight, 25,

          LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
            //SPACE_BevelStyle,           BVS_GROUP,
            TAG_DONE),
            CHILD_WeightedHeight, 30,

          TAG_DONE);
}

