/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: gui.c
**
** Date: 06-04-2021 16:38:55
**
************************************************************

*/

#include <proto/listbrowser.h>
#include <gadgets/space.h>

#include "globals.h"
#include "gui.h"
#include "lists.h"
#include "mainWin.h"
#include "guifuncs.h"

extern struct List leftSidebarList;
extern struct RenderHook *renderhook;
extern struct RenderHook *podcastImageRenderHook;

static Object *buildRadioSearchPage(void);
static Object *buildRadioFavouritePage(void);
static Object *buildRadioPopularPage(void);
static Object *buildRadioTrendPage(void);
static Object *buildPodcastSearchPage(void);
static Object *buildPodcastFavouritePage(void);
static Object *buildPodcastListenLaterPage(void);
static Object *buildPodcastTrendingPage(void);
static Object *buildRadioRightSidebar(struct Screen *, struct RenderHook *);
static Object *buildPodcastRightSidebar(struct Screen *, struct RenderHook *);

static const ULONG listToPage[] = {
    LISTBROWSER_Selected,    PAGE_Current,
    TAG_END
};

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ChooserClass;
extern Class *LabelClass;
extern Class *LayoutClass;
extern Class *ListBrowserClass;
extern Class *SpaceClass;
extern Class *StringClass;
extern Class *TextEditorClass;
extern Class *WindowClass;


Object *buildMainWindow(struct MsgPort *appPort, Object *winMenu, struct Screen *screen)
{                                           
  objects[OID_FAVOURITES_ADD_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        BITMAP_SourceFile,            "PROGDIR:images/favouritesadd",
        BITMAP_DisabledSourceFile,    "PROGDIR:images/favouritesadd_g",
        BITMAP_SelectSourceFile,      "PROGDIR:images/favouritesadd_s",
        BITMAP_Screen,                screen,
        BITMAP_Precision,             PRECISION_EXACT,
        BITMAP_Masking,               TRUE,
        TAG_END);

  objects[OID_FAVOURITES_REMOVE_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        BITMAP_SourceFile,            "PROGDIR:images/favouritesremove",
        BITMAP_DisabledSourceFile,    "PROGDIR:images/favouritesremove_g",
        BITMAP_SelectSourceFile,      "PROGDIR:images/favouritesremove_s",
        BITMAP_Screen,                screen,
        BITMAP_Precision,             PRECISION_EXACT,
        BITMAP_Masking,               TRUE,
        TAG_END);

  objects[OID_PLAY_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        BITMAP_SourceFile,            "PROGDIR:images/td_tn_play",
        BITMAP_DisabledSourceFile,    "PROGDIR:images/td_tn_play_g",
        BITMAP_SelectSourceFile,      "PROGDIR:images/td_tn_play_s",
        BITMAP_Screen,                screen,
        BITMAP_Precision,             PRECISION_EXACT,
        BITMAP_Masking,               TRUE,
        TAG_END),

  objects[OID_BOOKMARK_ADD_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        BITMAP_SourceFile,            "tbimages:bookmarkadd",
        BITMAP_DisabledSourceFile,    "tbimages:bookmarkadd_g",
        BITMAP_SelectSourceFile,      "tbimages:bookmarkadd_s",
        BITMAP_Screen,                screen,
        BITMAP_Precision,             PRECISION_EXACT,
        BITMAP_Masking,               TRUE,
        TAG_END),

  objects[OID_BOOKMARK_REMOVE_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        BITMAP_SourceFile,            "tbimages:bookmarkremove",
        BITMAP_DisabledSourceFile,    "tbimages:bookmarkremove_g",
        BITMAP_SelectSourceFile,      "tbimages:bookmarkremove_s",
        BITMAP_Screen,                screen,
        BITMAP_Precision,             PRECISION_EXACT,
        BITMAP_Masking,               TRUE,
        TAG_END),
  
  //struct DrawInfo *drInfo = IIntuition->GetScreenDrawInfo(screen);
  renderhook = (struct RenderHook *) IExec->AllocSysObjectTags (ASOT_HOOK,
        ASOHOOK_Size,  sizeof(struct RenderHook),
        ASOHOOK_Entry, (HOOKFUNC)renderfunct,
        TAG_END);

  podcastImageRenderHook = (struct RenderHook *) IExec->AllocSysObjectTags (ASOT_HOOK,
        ASOHOOK_Size,  sizeof(struct RenderHook),
        ASOHOOK_Entry, (HOOKFUNC)renderfunct,
        TAG_END);

  // TODO: Move the following pages to different
  Object *radioPages = IIntuition->NewObject(NULL, "page.gadget",
        LAYOUT_DeferLayout, TRUE,
        PAGE_Add, gadgets[GID_PAGE_1] = buildRadioSearchPage(),
        PAGE_Add, gadgets[GID_PAGE_2] = buildRadioFavouritePage(),
        PAGE_Add, gadgets[GID_PAGE_3] = buildRadioPopularPage(),
        PAGE_Add, gadgets[GID_PAGE_4] = buildRadioTrendPage(),
        PAGE_Add, gadgets[GID_PAGE_5] = buildPodcastSearchPage(),
        PAGE_Add, gadgets[GID_PAGE_6] = buildPodcastFavouritePage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_LISTEN_LATER]  = buildPodcastListenLaterPage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_TRENDING]      = buildPodcastTrendingPage(),
        TAG_DONE);

  //Object *rightSidebarPages = IIntuition->NewObject(NULL, "page.gadget",
  objects[OID_RIGHT_SIDEBAR_PAGES] = IIntuition->NewObject(NULL, "page.gadget",
        LAYOUT_DeferLayout, TRUE,
        PAGE_Add, gadgets[GID_RIGHT_SIDEBAR_PAGE_1] = buildRadioRightSidebar(screen, renderhook),
        PAGE_Add, gadgets[GID_RIGHT_SIDEBAR_PAGE_2] = buildPodcastRightSidebar(screen, podcastImageRenderHook),
        TAG_DONE);

  return IIntuition->NewObject(WindowClass, NULL,
    WA_ScreenTitle,         VSTRING,
    WA_Title,               APPNAME,
    WA_Activate,            TRUE,
    WA_CloseGadget,         TRUE,
    WA_DepthGadget,         TRUE,
    WA_DragBar,             TRUE,
    WA_SizeGadget,          TRUE,
    //// TODO: Make it configurable at preferences
    //WA_Opaqueness,          255,    /* Initial opaqueness on opening (0..255) */
    //WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    //WA_FadeTime,            250000, /* Duration of transition in microseconds */
    WA_NewLookMenus,        TRUE,
    WINDOW_AppPort,         appPort,
    WINDOW_GadgetHelp,      TRUE,
    WINDOW_Iconifiable,     TRUE,
    WINDOW_IconifyGadget,   TRUE,
    WINDOW_Icon,            IIcon->GetDiskObject("PROGDIR:MediaVault"),
    WINDOW_JumpScreensMenu, TRUE,
    WINDOW_MenuStrip,       winMenu,
    WINDOW_PopupGadget,     TRUE,
    WINDOW_Position,        WPOS_CENTERSCREEN,
    WINDOW_UniqueID,        APPNAME,
    WINDOW_Layout, IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_DeferLayout, TRUE,
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,

            // START - Left Sidebar
            LAYOUT_AddChild, gadgets[GID_LEFT_SIDEBAR] = IIntuition->NewObject(ListBrowserClass, NULL,
              GA_ID,                      GID_LEFT_SIDEBAR,
              GA_RelVerify,               TRUE,
              GA_TabCycle,                TRUE,
              LISTBROWSER_AutoFit,        TRUE,
              LISTBROWSER_ColumnTitles,   FALSE,
              LISTBROWSER_Hierarchical,   TRUE,
              LISTBROWSER_Separators,     FALSE,
              LISTBROWSER_ShowSelected,   TRUE,
              ICA_TARGET,                 radioPages,
              ICA_MAP,                    listToPage,
              TAG_DONE),
              CHILD_WeightedWidth, 20,
              CHILD_MinWidth, 150,
              LAYOUT_WeightBar, TRUE,
            // END - Left Sidebar

            LAYOUT_AddChild, gadgets[GID_PAGES] = radioPages,
            LAYOUT_WeightBar, TRUE,

            // START - Right Sidebar
            LAYOUT_AddChild, gadgets[GID_RIGHT_SIDEBAR_PAGES] = objects[OID_RIGHT_SIDEBAR_PAGES],
            CHILD_WeightedWidth, 30,
            CHILD_MinWidth, 200,
            // END - Right Sidebar

         TAG_DONE),
      TAG_DONE),
    TAG_DONE);
}

Object *buildMainMenu(struct Screen *screen)
{
  return IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ROOT,
    MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_MENU,
      MA_Label, "Project",
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_CHECKUPDATES,
        MA_Label,   "Check for Updates",
        MA_Image,   MenuImage("mailget", screen),
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_Label, ML_SEPARATOR,
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_ABOUT,
        MA_Label,   "About...",
        MA_Key,     "?",
        MA_Image,   MenuImage("info", screen),
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_ICONIFY,
        MA_Label,   "Iconify",
        MA_Key,     "I",
        MA_Image,   MenuImage("iconify", screen),
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_Label, ML_SEPARATOR,
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_QUIT,
        MA_Label,   "Quit",
        MA_Key,     "Q",
        MA_Image,   MenuImage("quit", screen),
        TAG_END),
      TAG_END),
    TAG_END);
}

CONST_STRPTR getChooserText(int gadgetId, uint16 code)
{
  switch (gadgetId)
  {
    case GID_CHOOSER_GENRES:
      return genres[code];
      break;
    case GID_CHOOSER_COUNTRIES:
      return countries[code];
      break;
    case GID_CHOOSER_LANGUAGES:
      return languages[code];
      break;
    case GID_PODCAST_TRENDING_CHOOSER_GENRES:
      return podcastCategories[code];
      break;
    case GID_PODCAST_TRENDING_CHOOSER_LANGUAGES:
      return languagesISO6391[code];
      break;
  }
  return NULL;
}

static Object *buildRadioSearchPage(void)
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

static Object *buildRadioFavouritePage(void)
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

static Object *buildRadioPopularPage(void)
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

static Object *buildRadioTrendPage(void)
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

void getLeftSidebarContent(void)
{
  IExec->NewList(&leftSidebarList);

  struct Node *node;

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    1,
    LBNA_Flags,         LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Radio",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Favourite",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Popular",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Trending",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    1,
    LBNA_Flags,         LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Podcast",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Favourite",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Listen Later",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }
  
  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Trending",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }
}

static Object *buildPodcastSearchPage(void)
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

static Object *buildPodcastFavouritePage(void)
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

static Object *buildPodcastListenLaterPage(void)
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

static Object *buildPodcastTrendingPage(void)
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

static Object *buildRadioRightSidebar(struct Screen *screen, struct RenderHook *renderhook)
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

static Object *buildPodcastRightSidebar(struct Screen *screen, struct RenderHook *renderhook)
{
  objects[OID_PODCAST_AVATAR_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        GA_ID,              OID_PODCAST_AVATAR_IMAGE,
        IA_Scalable,        FALSE,
        BITMAP_Screen,      screen,
        BITMAP_SourceFile,  LOGO_IMAGE,
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

