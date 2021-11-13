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
#include "guifuncs.h"
#include "radiopages.h"
#include "podcastpages.h"
#include "serverpages.h"
#include "mainWin.h"

extern struct List leftSidebarList;
extern struct RenderHook *renderhook;
extern struct RenderHook *podcastImageRenderHook;

/*
TODO: This needs to be removed
static const ULONG listToPage[] = {
    LISTBROWSER_Selected,    PAGE_Current,
    TAG_END
};
*/

extern Class *LayoutClass;
extern Class *WindowClass;


Object *buildMainWindow(struct MsgPort *appPort, Object *winMenu, struct Screen *screen)
{                                           
  objects[OID_FAVOURITES_ADD_IMAGE]     = buildImage("favouritesadd", screen);
  objects[OID_FAVOURITES_REMOVE_IMAGE]  = buildImage("favouritesremove", screen);
  objects[OID_PLAY_IMAGE]               = buildImage("td_tn_play", screen);
  objects[OID_BOOKMARK_ADD_IMAGE]       = buildImage("bookmarkadd", screen);
  objects[OID_BOOKMARK_REMOVE_IMAGE]    = buildImage("bookmarkremove", screen);

  //struct DrawInfo *drInfo = IIntuition->GetScreenDrawInfo(screen);
  renderhook = (struct RenderHook *) IExec->AllocSysObjectTags (ASOT_HOOK,
        ASOHOOK_Size,  sizeof(struct RenderHook),
        ASOHOOK_Entry, (HOOKFUNC)renderfunct,
        TAG_END);

  podcastImageRenderHook = (struct RenderHook *) IExec->AllocSysObjectTags (ASOT_HOOK,
        ASOHOOK_Size,  sizeof(struct RenderHook),
        ASOHOOK_Entry, (HOOKFUNC)renderfunct,
        TAG_END);

  //Object *radioPages =
  objects[OID_MAIN_PAGES] = IIntuition->NewObject(NULL, "page.gadget",
        LAYOUT_DeferLayout, TRUE,
        PAGE_Add, gadgets[GID_PAGE_RADIO_SEARCH]          = buildRadioSearchPage(genres, countries, languages),
        PAGE_Add, gadgets[GID_PAGE_RADIO_FAVOURITE]       = buildRadioFavouritePage(),
        PAGE_Add, gadgets[GID_PAGE_RADIO_POPULAR]         = buildRadioPopularPage(),
        PAGE_Add, gadgets[GID_PAGE_RADIO_TREND]           = buildRadioTrendPage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_SEARCH]        = buildPodcastSearchPage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_FAVOURITE]     = buildPodcastFavouritePage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_LISTEN_LATER]  = buildPodcastListenLaterPage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_TRENDING]      = buildPodcastTrendingPage(podcastCategories, languages),
        PAGE_Add, gadgets[GID_PAGE_MEDIA_SERVERS]         = buildMediaServersPage(),
        PAGE_Add, gadgets[GID_PAGE_PODCAST_EPISODES]      = buildPodcastEpisodesPage(),
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
              //ICA_TARGET,                 objects[OID_MAIN_PAGES], // TODO: This needs to be removed
              //ICA_MAP,                    listToPage,
              TAG_DONE),
              CHILD_WeightedWidth, 20,
              CHILD_MinWidth, 150,
              LAYOUT_WeightBar, TRUE,
            // END - Left Sidebar

            LAYOUT_AddChild, gadgets[GID_PAGES] = objects[OID_MAIN_PAGES],
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

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    1,
    //LBNA_Flags,         LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Media Servers",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }
}

