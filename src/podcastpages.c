 /**
  * File:    podcastpages.c
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
  *   pages for the podcast sections.
  *
  */  


#include <gadgets/space.h>

#include "globals.h"
#include "gui.h"
#include "guifuncs.h"
#include "podcastpages.h"

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ChooserClass;
extern Class *LabelClass;
extern Class *LayoutClass;
extern Class *ListBrowserClass;
extern Class *SpaceClass;
extern Class *StringClass;
extern Class *TextEditorClass;


Object *buildPodcastSearchPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
        LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

        // START - Top Filter Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_BevelStyle,      BVS_GROUP,
          LAYOUT_Label,           "Search Podcasts by",

          // Top filter section with the Name Text Box with Label
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

            LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
              //LABEL_DrawInfo, drInfo,
              LABEL_Text, "_Title",
              TAG_END),

            LAYOUT_AddChild, gadgets[GID_PODCAST_FILTERS_NAME] = IIntuition->NewObject(StringClass, NULL,
              GA_ID,                GID_PODCAST_FILTERS_NAME,
              GA_RelVerify,         TRUE,
              GA_TabCycle,          TRUE,
              GA_ActivateKey,       "t",
              STRINGA_MinVisible,   10,
              STRINGA_MaxChars,     40,
              TAG_DONE),

            // Filters Button
            LAYOUT_AddChild, gadgets[GID_PODCAST_FILTER_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,              GID_PODCAST_FILTER_BUTTON,
              GA_Text,            "_Discover",
              GA_TabCycle,        TRUE,
              GA_RelVerify,       TRUE,
              GA_ActivateKey,     "d",
              TAG_DONE),

            TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,
        // END - Top Filter Section

        // START - Bottom List Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

          LAYOUT_AddChild, gadgets[GID_PODCAST_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                      GID_PODCAST_LISTBROWSER,
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

Object *buildPodcastFavouritePage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Favourite Podcasts",

        LAYOUT_AddChild, gadgets[GID_PODCAST_FAVOURITE_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_PODCAST_FAVOURITE_LISTBROWSER,
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

Object *buildPodcastListenLaterPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Listen Later Podcast Episodes",

        LAYOUT_AddChild, gadgets[GID_PODCAST_LISTEN_LATER_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_PODCAST_LISTEN_LATER_LISTBROWSER,
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

Object *buildPodcastTrendingPage(CONST_STRPTR *podcastCategories, CONST_STRPTR *languages)
{
  return IIntuition->NewObject(LayoutClass, NULL,
        LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

        // START - Top Filter Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_BevelStyle,      BVS_GROUP,
          LAYOUT_Label,           "Trending Podcasts by",

          // Top filter section with the select boxes
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            // Genres Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                LABEL_Text, "_Genre",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_PODCAST_TRENDING_CHOOSER_GENRES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "g",
                CHOOSER_LabelArray,   podcastCategories,
                CHOOSER_MaxLabels,    40,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),

            // Languages Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                LABEL_Text, "_Language",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_PODCAST_TRENDING_CHOOSER_LANGUAGES,
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
            LAYOUT_AddChild, gadgets[GID_PODCAST_TRENDING_FILTER_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,              GID_PODCAST_TRENDING_FILTER_BUTTON,
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

          LAYOUT_AddChild, gadgets[GID_PODCAST_TRENDING_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                      GID_PODCAST_TRENDING_LISTBROWSER,
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

Object *buildPodcastRightSidebar(struct Screen *screen, struct RenderHook *renderhook)
{
  objects[OID_PODCAST_AVATAR_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        GA_ID,              OID_PODCAST_AVATAR_IMAGE,
        IA_Scalable,        FALSE,
        BITMAP_Screen,      screen,
        BITMAP_SourceFile,  IMAGES_PATH LOGO_IMAGE,
        BITMAP_Masking,     TRUE,
        TAG_END);

  if (renderhook && objects[OID_PODCAST_AVATAR_IMAGE])
  {
    renderhook->img  = objects[OID_PODCAST_AVATAR_IMAGE];
    renderhook->w    = ((struct Image *)objects[OID_PODCAST_AVATAR_IMAGE])->Width;
    renderhook->h    = ((struct Image *)objects[OID_PODCAST_AVATAR_IMAGE])->Height;
    renderhook->fill = FALSE;
  }

  return IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

          LAYOUT_AddChild, gadgets[GID_PODCAST_INFO_AVATAR] = IIntuition->NewObject(SpaceClass, NULL,
            GA_ID,                      GID_PODCAST_INFO_AVATAR,
            SPACE_MinWidth,             128,
            SPACE_MinHeight,            128,
            SPACE_RenderHook,           renderhook,
            GA_Image,                   objects[OID_PODCAST_AVATAR_IMAGE],
            TAG_DONE),
            CHILD_WeightedHeight, 30,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,
            //LAYOUT_BevelStyle,      BVS_GROUP,
            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 70,

            LAYOUT_AddChild, gadgets[GID_PODCAST_FAVOURITE_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,                      GID_PODCAST_FAVOURITE_BUTTON,
              GA_Disabled,                TRUE,
              BUTTON_Transparent,         TRUE,
              BUTTON_AutoButton,          0,
              BUTTON_BevelStyle,          BVS_NONE,
              BUTTON_Justification,       BCJ_CENTER,
              BUTTON_RenderImage,         objects[OID_FAVOURITES_ADD_IMAGE],
              TAG_DONE),
              CHILD_WeightedWidth, 10,

            LAYOUT_AddChild, gadgets[GID_PODCAST_LISTEN_LATER_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,                      GID_PODCAST_LISTEN_LATER_BUTTON,
              GA_Disabled,                TRUE,
              BUTTON_Transparent,         TRUE,
              BUTTON_AutoButton,          0,
              BUTTON_BevelStyle,          BVS_NONE,
              BUTTON_Justification,       BCJ_CENTER,
              BUTTON_RenderImage,         objects[OID_BOOKMARK_ADD_IMAGE],
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
            LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
              TAG_DONE),
              CHILD_WeightedWidth, 30,

            LAYOUT_AddChild, gadgets[GID_PODCAST_PLAY_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,                      GID_PODCAST_PLAY_BUTTON,
              GA_Disabled,                TRUE,
              GA_HintInfo,                "When this is enabled,\nclick to listen to the\nselected podcast episode",
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
              CHILD_WeightedWidth, 30,
            TAG_DONE),
            CHILD_MaxHeight, 60,

          LAYOUT_AddChild, gadgets[GID_PODCAST_INFO_DATA] = IIntuition->NewObject(TextEditorClass, NULL,
            GA_ID,                      GID_PODCAST_INFO_DATA,
            GA_RelVerify,               TRUE,
            GA_TEXTEDITOR_BevelStyle,   BVS_NONE,
            GA_TEXTEDITOR_Contents,     VERS,
            GA_TEXTEDITOR_CursorX,      0,
            GA_TEXTEDITOR_CursorY,      0,
            GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Center,
            GA_TEXTEDITOR_ReadOnly,     TRUE,
            GA_TEXTEDITOR_Transparent,  TRUE,
            TAG_DONE),
            CHILD_WeightedHeight, 13,

          LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
            TAG_DONE),
            CHILD_WeightedHeight, 2,

          LAYOUT_AddChild, gadgets[GID_PODCAST_EPISODES_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                      GID_PODCAST_EPISODES_LISTBROWSER,
            GA_RelVerify,               TRUE,
            GA_TabCycle,                TRUE,
            LISTBROWSER_ColumnTitles,   TRUE,
            LISTBROWSER_HorizontalProp, TRUE,
            LISTBROWSER_Separators,     TRUE,
            LISTBROWSER_ShowSelected,   TRUE,
            LISTBROWSER_Striping,       LBS_ROWS,
            TAG_DONE),
            CHILD_WeightedHeight, 40,

          TAG_DONE);
}

