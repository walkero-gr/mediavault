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


#include <proto/application.h>
#include <proto/listbrowser.h>

#include "globals.h"
#include "guifuncs.h"
#include "mainWin.h"
#include "aboutWin.h"
#include "dlnafuncs.h"
#include "radiofuncs.h"
#include "podcastfuncs.h"
#include "httpfuncs.h"
#include "updates.h"
#include "gui.h"

struct ColumnInfo *columnInfo, *radioFavouritesColInfo, *leftSidebarCI,
            *podcastColInfo, *podcastEpisodeColInfo, *mediaServerColInfo;
struct List radioList,
            radioFavouriteList,
            radioPopularList,
            radioTrendList,
            leftSidebarList,
            podcastList,
            podcastFavouriteList,
            podcastListenLaterList,
            trendingPodcastList,
            podcastEpisodeList,
            mediaServerList;

struct filters  lastFilters = {0},
                prevFilters = {0};

struct RenderHook *renderhook;
struct RenderHook *podcastImageRenderHook;

static void fillLeftSidebar(void);
static BOOL checkFiltersChanged(void);
static void setColumnInfo(void);

extern struct memory response;

void showGUI(void)
{
  struct Screen   *screen = NULL;
  struct MsgPort  *appPort = NULL;
  struct MsgPort  *notifyPort = NULL;
  uint32 appID = 0L;

  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort)
  {
    if((screen = IIntuition->LockPubScreen(NULL)))
    {
      //drInfo = IIntuition->GetScreenDrawInfo(screen);
      menus[MID_PROJECT]  = buildMainMenu(screen);
      objects[OID_MAIN]   = buildMainWindow(appPort, menus[MID_PROJECT], screen);
      objects[OID_ABOUT]  = buildAboutWindow(appPort, screen);

      gadgets[GID_MSG_REQ] = IIntuition->NewObject(NULL, "requester.class",
          REQ_Type, REQTYPE_INFO,
          REQ_StayOnTop, TRUE,
          REQ_GadgetText, "_OK",
          TAG_DONE);

      if (objects[OID_MAIN])
      {
        windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);

        if (windows[WID_MAIN])
        {
          struct ApplicationMsg *notifyMsg = NULL;
          uint32  wait = 0L,
                  winSignal = 0L,
                  appSignal = 0L,
                  selectedMenu = MID_LAST;
          uint16 code = 0;
          BOOL done = FALSE;
          struct filters lastPodcastFilters = {0};

          fillLeftSidebar();
          startUpdateProcess(TRUE);
          setColumnInfo();

          //## Register MediaVault as an application
          if ( (appID = IApplication->RegisterApplication(APPNAME,
                  REGAPP_CanCreateNewDocs,  FALSE,
                  REGAPP_CanPrintDocs,      FALSE,
                  REGAPP_Description,       "Online radio station search tool",
                  REGAPP_HasPrefsWindow,    FALSE,
                  REGAPP_HasIconifyFeature, TRUE,
                  REGAPP_UniqueApplication, FALSE,
                  REGAPP_URLIdentifier,     "walkero.gr",
                  TAG_DONE)) )
          {
            IApplication->GetApplicationAttrs(appID, APPATTR_Port, &notifyPort, TAG_DONE);
            if (notifyPort)
            {
              appSignal = (1L << notifyPort->mp_SigBit);
            }
          }


          IIntuition->GetAttr(WINDOW_SigMask, objects[OID_MAIN], &winSignal);

          while (!done)
          {
            wait = IExec->Wait(winSignal | appSignal | SIGBREAKF_CTRL_C);

            if ( wait & SIGBREAKF_CTRL_C )
            {
              done = TRUE;
            }

            if ( wait & appSignal )
            {
              while ( (notifyMsg = (struct ApplicationMsg *) IExec->GetMsg(notifyPort)) )
              {
                switch (notifyMsg->type)
                {
                  case APPLIBMT_Quit:
                  case APPLIBMT_ForceQuit:
                    done = TRUE;
                    break;

                  case APPLIBMT_Hide:
                    if ( appHide(appID, objects[OID_MAIN], WM_ICONIFY) == TRUE )
                    {
                      windows[WID_MAIN] = NULL;
                    }
                    break;

                  case APPLIBMT_ToFront:
                  case APPLIBMT_Unhide:
                    windows[WID_MAIN] = appUnhide(appID, objects[OID_MAIN]);
                    if (windows[WID_MAIN] == NULL) done = TRUE; // error re-opening the window
                    break;
                }
                IExec->ReplyMsg((struct Message *) notifyMsg);
              }
            }

            if ( wait & winSignal )
            {
              uint32  result = WMHI_LASTMSG,
                      res_value,
                      radioNode,
                      podcastNode,
                      podcastEpisodeNode,
                      lsbNodeIdx;

              //## Main Window events
              while ((result = IIntuition->IDoMethod(objects[OID_MAIN], WM_HANDLEINPUT, &code, TAG_DONE)))
              {
                switch (result & WMHI_CLASSMASK)
                {
                  case WMHI_CLOSEWINDOW:
                    done = TRUE;
                    break;

                  case WMHI_ICONIFY:
                    if (appHide(appID, objects[OID_MAIN], WM_ICONIFY) == TRUE)
                    {
                      windows[WID_MAIN] = NULL;
                    }
                    break;

                  case WMHI_UNICONIFY:
                    windows[WID_MAIN] = appUnhide(appID, objects[OID_MAIN]);
                    if (windows[WID_MAIN] == NULL) done = TRUE; // error re-opening the window
                    break;

                  case WMHI_MENUPICK:
                    selectedMenu = NO_MENU_ID;

                    while ((selectedMenu = IIntuition->IDoMethod(menus[MID_PROJECT], MM_NEXTSELECT, 0, selectedMenu, TAG_DONE)) != NO_MENU_ID)
                    {
                      switch (selectedMenu)
                      {
                        case MID_CHECKUPDATES:
                          workOnUpdate();
                          break;
                        case MID_ICONIFY:
                          if (appHide(appID, objects[OID_MAIN], WM_ICONIFY) == TRUE)
                          {
                            windows[WID_MAIN] = NULL;
                          }
                          break;
                        case MID_ABOUT:
                          windows[WID_ABOUT] = (struct Window*)IIntuition->IDoMethod(objects[OID_ABOUT], WM_OPEN, TAG_DONE);
                          windowBlocking(objects[OID_MAIN], TRUE);
                          break;
                        case MID_QUIT:
                          done = TRUE;
                          break;
                      }
                    }

                    break;

                  case WMHI_JUMPSCREEN:
                    {
                      struct Screen *newScr = NULL;

                      IIntuition->GetAttr(WA_PubScreen, objects[OID_MAIN], (uint32 *) &newScr);

                      if (newScr)
                      {
                        if ( appHide(appID, objects[OID_MAIN], WM_CLOSE) == TRUE )
                        {
                          windows[WID_MAIN] = NULL;
                        }

                        // Confirm the screen pointer and re-open the window.
                        IIntuition->SetAttrs(objects[OID_MAIN], WA_PubScreen, newScr, TAG_DONE);

                        if ( (windows[WID_MAIN] = (struct Window *) IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, NULL)) )
                        {
                          IApplication->SetApplicationAttrs(appID,
                                        APPATTR_Hidden, FALSE,
                                        TAG_DONE);
                          IIntuition->ScreenToFront(newScr);
                        }

                        if ( windows[WID_MAIN] == NULL ) done = TRUE; // opening on the new screen failed
                      }
                    }
                    break;

                  case WMHI_GADGETUP:
                    switch (result & WMHI_GADGETMASK)
                    {
                      case GID_FILTER_BUTTON:
                        {
                          BOOL filtersChanged = checkFiltersChanged();
                          IUtility->Strlcpy(lastFilters.name, ((struct StringInfo *)(((struct Gadget *)gadgets[GID_FILTERS_NAME])->SpecialInfo))->Buffer, sizeof(lastFilters.name));

                          windowBlocking(objects[OID_MAIN], TRUE);

                          changeDiscoverButton(!filtersChanged);
                          fillRadioList(lastFilters, filtersChanged);
                          
                          windowBlocking(objects[OID_MAIN], FALSE);
                        }
                        break;

                      case GID_FILTERS_NAME:
                        changeDiscoverButton(FALSE);
                        break;

                      case GID_CHOOSER_GENRES:
                        changeDiscoverButton(FALSE);
                        if (code > 0)
                        {
                          IUtility->Strlcpy(lastFilters.genre, getChooserText(GID_CHOOSER_GENRES, code), sizeof(lastFilters.genre));
                        }
                        else IUtility->Strlcpy(lastFilters.genre, "", sizeof(lastFilters.genre));
                        break;

                      case GID_CHOOSER_COUNTRIES:
                        changeDiscoverButton(FALSE);
                        if (code > 0)
                        {
                          IUtility->Strlcpy(lastFilters.country, getChooserText(GID_CHOOSER_COUNTRIES, code), sizeof(lastFilters.country));
                        }
                        else IUtility->Strlcpy(lastFilters.country, "", sizeof(lastFilters.country));
                        break;

                      case GID_CHOOSER_LANGUAGES:
                        changeDiscoverButton(FALSE);
                        if (code > 0)
                        {
                          IUtility->Strlcpy(lastFilters.language, getChooserText(GID_CHOOSER_LANGUAGES, code), sizeof(lastFilters.language));
                        }
                        else IUtility->Strlcpy(lastFilters.language, "", sizeof(lastFilters.language));
                        break;

                      case GID_RADIO_LISTBROWSER:
                      case GID_RADIO_POPULAR_LISTBROWSER:
                      case GID_RADIO_TREND_LISTBROWSER:
                      case GID_RADIO_FAVOURITE_LISTBROWSER:
                        {
                          Object *lb = NULL;

                          if ((result & WMHI_GADGETMASK) == GID_RADIO_LISTBROWSER) lb = gadgets[GID_RADIO_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_RADIO_POPULAR_LISTBROWSER)   lb = gadgets[GID_RADIO_POPULAR_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_RADIO_TREND_LISTBROWSER)     lb = gadgets[GID_RADIO_TREND_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_RADIO_FAVOURITE_LISTBROWSER) lb = gadgets[GID_RADIO_FAVOURITE_LISTBROWSER];

                          IIntuition->GetAttr(LISTBROWSER_RelEvent, lb, &res_value);
                          if (res_value == LBRE_NORMAL)
                          {
                            IIntuition->GetAttr(LISTBROWSER_SelectedNode, lb, (uint32 *)&radioNode);
                            windowBlocking(objects[OID_MAIN], TRUE);
                            showRadioInfo((struct Node *)radioNode);
                            windowBlocking(objects[OID_MAIN], FALSE);
                          }
                        }
                        break;

                      case GID_INFO_PLAY_BUTTON:
                        if (res_value == LBRE_NORMAL)
                        {
                          if (radioNode)
                          {
                            playRadio((struct Node *)radioNode);
                          }
                        }
                        break;

                      case GID_RADIO_FAVOURITE_BUTTON:
                        if (res_value == LBRE_NORMAL)
                        {
                          if (radioNode)
                          {
                            addFavouriteRadio((struct Node *)radioNode);
                          }
                        }
                        break;

                      case GID_LEFT_SIDEBAR:
                        IIntuition->GetAttr(LISTBROWSER_Selected, gadgets[GID_LEFT_SIDEBAR], &lsbNodeIdx);
                        switch (lsbNodeIdx)
                        {
                          case PAGE_MAIN_RADIO:
                            switchPage(PAGE_RADIO_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            break;
                          case PAGE_MAIN_RADIO_FAVOURITE:
                            switchPage(PAGE_RADIO_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            fillRadioFavouriteList();
                            break;
                          case PAGE_MAIN_RADIO_POPULAR:
                            switchPage(PAGE_RADIO_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            if(listCount(&radioPopularList) == 0)
                            {
                              windowBlocking(objects[OID_MAIN], TRUE);
                              fillRadioPopularList();
                              windowBlocking(objects[OID_MAIN], FALSE);
                            }
                            break;
                          case PAGE_MAIN_RADIO_TRENDING:
                            switchPage(PAGE_RADIO_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            if(listCount(&radioTrendList) == 0)
                            {
                              windowBlocking(objects[OID_MAIN], TRUE);
                              fillRadioTrendList();
                              windowBlocking(objects[OID_MAIN], FALSE);
                            }
                            break;
                          case PAGE_MAIN_PODCAST:
                          case PAGE_MAIN_PODCAST_TRENDING:
                            switchPage(PAGE_PODCAST_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            break;
                          case PAGE_MAIN_PODCAST_FAVOURITE:
                            switchPage(PAGE_PODCAST_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            fillPodcastFavouriteList();
                            break;
                          case PAGE_MAIN_PODCAST_LISTEN_LATER:
                            switchPage(PAGE_PODCAST_INFO, objects[OID_RIGHT_SIDEBAR_PAGES]);
                            fillPodcastListenLaterList();
                            break;
                          case PAGE_MAIN_MEDIA_SERVERS:
                            //discoverUPnPServers();
                            fillMediaServersList();
                            break;
                        }
                        switchPage(lsbNodeIdx, objects[OID_MAIN_PAGES]);
                        break;

                      case GID_PODCAST_TRENDING_CHOOSER_GENRES:
                        //changeDiscoverButton(FALSE);
                        if (code > 0)
                        {
                          IUtility->Strlcpy(lastPodcastFilters.genre, getChooserText(GID_PODCAST_TRENDING_CHOOSER_GENRES, code), sizeof(lastPodcastFilters.genre));
                        }
                        else IUtility->Strlcpy(lastPodcastFilters.genre, "", sizeof(lastPodcastFilters.genre));
                        break;

                      case GID_PODCAST_TRENDING_CHOOSER_LANGUAGES:
                        //changeDiscoverButton(FALSE);
                        if (code > 0)
                        {
                          IUtility->Strlcpy(lastPodcastFilters.language, getChooserText(GID_PODCAST_TRENDING_CHOOSER_LANGUAGES, code), sizeof(lastPodcastFilters.language));
                        }
                        else IUtility->Strlcpy(lastPodcastFilters.language, "", sizeof(lastPodcastFilters.language));
                        break;

                      case GID_PODCAST_FILTER_BUTTON:
                        IUtility->Strlcpy(lastPodcastFilters.name, ((struct StringInfo *)(((struct Gadget *)gadgets[GID_PODCAST_FILTERS_NAME])->SpecialInfo))->Buffer, sizeof(lastPodcastFilters.name));
                        if (IUtility->Stricmp(lastPodcastFilters.name, ""))
                        {
                          windowBlocking(objects[OID_MAIN], TRUE);
                          fillPodcastList(lastPodcastFilters);
                          windowBlocking(objects[OID_MAIN], FALSE);
                        }
                        else
                        {
                          char notFoundMsg[] = "Please, add the podcast title you want to search!";
                          showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (char *)notFoundMsg, 0, NULL, 0);
                        }
                        break;

                      case GID_PODCAST_TRENDING_FILTER_BUTTON:
                        windowBlocking(objects[OID_MAIN], TRUE);
                        fillPodcastTrendingList(lastPodcastFilters);
                        windowBlocking(objects[OID_MAIN], FALSE);
                        break;

                      case GID_PODCAST_LISTBROWSER:
                      case GID_PODCAST_FAVOURITE_LISTBROWSER:
                      case GID_PODCAST_TRENDING_LISTBROWSER:
                        {
                          Object *lb = NULL;

                          if ((result & WMHI_GADGETMASK) == GID_PODCAST_LISTBROWSER)                  lb = gadgets[GID_PODCAST_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_PODCAST_FAVOURITE_LISTBROWSER) lb = gadgets[GID_PODCAST_FAVOURITE_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_PODCAST_TRENDING_LISTBROWSER)  lb = gadgets[GID_PODCAST_TRENDING_LISTBROWSER];

                          IIntuition->GetAttr(LISTBROWSER_RelEvent, lb, &res_value);
                          if (res_value == LBRE_NORMAL)
                          {
                            IIntuition->GetAttr(LISTBROWSER_SelectedNode, lb, (uint32 *)&podcastNode);

                            windowBlocking(objects[OID_MAIN], TRUE);
                            showPodcastInfo((struct Node *)podcastNode);
                            windowBlocking(objects[OID_MAIN], FALSE);
                          }
                        }
                        break;

                      case GID_PODCAST_EPISODES_LISTBROWSER:
                      case GID_PODCAST_LISTEN_LATER_LISTBROWSER:
                        {
                          Object *lb = NULL;

                          if ((result & WMHI_GADGETMASK) == GID_PODCAST_EPISODES_LISTBROWSER)             lb = gadgets[GID_PODCAST_EPISODES_LISTBROWSER];
                            else if ((result & WMHI_GADGETMASK) == GID_PODCAST_LISTEN_LATER_LISTBROWSER)  lb = gadgets[GID_PODCAST_LISTEN_LATER_LISTBROWSER];

                            IIntuition->GetAttr(LISTBROWSER_RelEvent, lb, &res_value);
                            if (res_value == LBRE_NORMAL)
                            {
                              IIntuition->GetAttr(LISTBROWSER_SelectedNode, lb, (uint32 *)&podcastEpisodeNode);

                              windowBlocking(objects[OID_MAIN], TRUE);
                              showPodcastEpisodeInfo((struct Node *)podcastEpisodeNode);
                              windowBlocking(objects[OID_MAIN], FALSE);
                            }
                        }
                        break;

                      case GID_PODCAST_PLAY_BUTTON:
                        if (res_value == LBRE_NORMAL)
                        {
                          if (podcastEpisodeNode)
                          {
                            playPodcast((struct Node *)podcastEpisodeNode);
                          }
                        }
                        break;  

                      case GID_PODCAST_FAVOURITE_BUTTON:
                        if (res_value == LBRE_NORMAL)
                        {
                          if (podcastNode)
                          {
                            addFavouritePodcast((struct Node *)podcastNode);
                          }
                        }
                        break;

                      case GID_PODCAST_LISTEN_LATER_BUTTON:
                        if (res_value == LBRE_NORMAL)
                        {
                          if (podcastEpisodeNode)
                          {
                            addListenLaterPodcast((struct Node *)podcastEpisodeNode);
                          }
                        }
                        break;
                    }
                    break;
                }
              }

              //## About Window events
              while ((result = IIntuition->IDoMethod(objects[OID_ABOUT], WM_HANDLEINPUT, &code, TAG_DONE)))
              {
                switch(result & WMHI_CLASSMASK)
                {
                  case WMHI_CLOSEWINDOW:
                    IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
                    windowBlocking(objects[OID_MAIN], FALSE);
                    break;
                  case WMHI_GADGETUP:
                    switch(result & WMHI_GADGETMASK)
                    {
                      case GID_ABOUT_BUTTON_OK:
                        IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
                        windowBlocking(objects[OID_MAIN], FALSE);
                        break;
                    }
                    break;
                }
              }
            }
          }

          // TODO: Move clearance to a separate method
          if (windows[WID_MAIN])
          {
            IIntuition->IDoMethod(objects[OID_MAIN], WM_CLOSE, TAG_DONE);
            windows[WID_MAIN] = NULL;
          }

          IListBrowser->FreeLBColumnInfo(columnInfo);
          if(listCount(&radioList))
          {
            freeList(&radioList, STRUCT_STATION_INFO);
          }

          IListBrowser->FreeLBColumnInfo(radioFavouritesColInfo);
          if(listCount(&radioFavouriteList))
          {
            freeList(&radioFavouriteList, STRUCT_STATION_INFO);
          }

          if(listCount(&radioPopularList))
          {
            freeList(&radioPopularList, STRUCT_STATION_INFO);
          }

          if(listCount(&radioTrendList))
          {
            freeList(&radioTrendList, STRUCT_STATION_INFO);
          }

          IListBrowser->FreeLBColumnInfo(leftSidebarCI);
          IListBrowser->FreeListBrowserList(&leftSidebarList);

          IListBrowser->FreeLBColumnInfo(podcastColInfo);
          if(listCount(&podcastList))
          {
            freeList(&podcastList, STRUCT_PODCAST_INFO);
          }
          if(listCount(&podcastFavouriteList))
          {
            freeList(&podcastFavouriteList, STRUCT_PODCAST_INFO);
          }
          if(listCount(&podcastListenLaterList))
          {
            freeList(&podcastListenLaterList, STRUCT_PODCAST_INFO);
          }
          if(listCount(&trendingPodcastList))
          {
            freeList(&trendingPodcastList, STRUCT_PODCAST_INFO);
          }
          IListBrowser->FreeLBColumnInfo(podcastEpisodeColInfo);
          if(listCount(&podcastEpisodeList))
          {
            freeList(&podcastEpisodeList, STRUCT_PODCAST_EPISODE_INFO);
          }

          IListBrowser->FreeLBColumnInfo(mediaServerColInfo);
          if(listCount(&mediaServerList))
          {
            freeList(&mediaServerList, STRUCT_MEDIA_SERVER_INFO);
          }

          IIntuition->DisposeObject(objects[OID_ABOUT]);
          IIntuition->DisposeObject(objects[OID_MAIN]);

          IExec->FreeSysObject(ASOT_HOOK, renderhook);
          IExec->FreeSysObject(ASOT_HOOK, podcastImageRenderHook);
          IIntuition->DisposeObject(objects[OID_AVATAR_IMAGE]);
          IIntuition->DisposeObject(objects[OID_PODCAST_AVATAR_IMAGE]);

          IIntuition->DisposeObject(objects[OID_PLAY_IMAGE]);
          IIntuition->DisposeObject(objects[OID_FAVOURITES_ADD_IMAGE]);
          IIntuition->DisposeObject(objects[OID_FAVOURITES_REMOVE_IMAGE]);
          IIntuition->DisposeObject(objects[OID_BOOKMARK_ADD_IMAGE]);
          IIntuition->DisposeObject(objects[OID_BOOKMARK_REMOVE_IMAGE]);

          IIntuition->DisposeObject(menus[MID_PROJECT]);
        }
      }
      IIntuition->UnlockPubScreen(NULL, screen);
      screen = NULL;
      //IIntuition->FreeScreenDrawInfo(screen, drInfo);
    }
  }

  if (appID) IApplication->UnregisterApplicationA(appID, NULL);
  IExec->FreeSysObject(ASOT_PORT, appPort);
}

static BOOL checkFiltersChanged(void)
{
  BOOL changed = FALSE;
  static BOOL firstCheck = TRUE;

  if (IUtility->Stricmp(lastFilters.name, prevFilters.name))
  {
    changed = TRUE;
  }
  if (IUtility->Stricmp(lastFilters.genre, prevFilters.genre))
  {
    changed = TRUE;
  }
  if (IUtility->Stricmp(lastFilters.country, prevFilters.country))
  {
    changed = TRUE;
  }
  if (IUtility->Stricmp(lastFilters.language, prevFilters.language))
  {
    changed = TRUE;
  }

  if (firstCheck)
  {
    firstCheck = FALSE;
    changed = TRUE;
  }

  if (changed)  prevFilters = lastFilters;

  return changed;
}

void changeDiscoverButton(BOOL isMore)
{
  char buttonText[16] = "_Discover";
  if (isMore)
  {
    IUtility->Strlcpy(buttonText, "_Discover more", sizeof(buttonText));
  }

  IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_FILTER_BUTTON], windows[WID_MAIN], NULL,
      GA_Text,          buttonText,
      GA_ActivateKey,   "d",
      TAG_DONE);
}

static void fillLeftSidebar(void)
{
  getLeftSidebarContent();

  leftSidebarCI = IListBrowser->AllocLBColumnInfo( 1,
      LBCIA_Column,   0,
      TAG_DONE);

  IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_LEFT_SIDEBAR], windows[WID_MAIN], NULL,
      LISTBROWSER_Labels,         &leftSidebarList,
      LISTBROWSER_Selected,       0,
      LISTBROWSER_ColumnInfo,     leftSidebarCI,
      TAG_DONE);
}

static void setColumnInfo(void)
{
  columnInfo = IListBrowser->AllocLBColumnInfo( 4,
      LBCIA_Column,                 0,
        LBCIA_Title,                " Station",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               40,
      LBCIA_Column,                 1,
        LBCIA_Title,                " Country",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               30,
      LBCIA_Column,                 2,
        LBCIA_Title,                " Bitrate",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               20,
      LBCIA_Column,                 3,
        LBCIA_Title,                " Votes",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               10,
      TAG_DONE);

  radioFavouritesColInfo = IListBrowser->AllocLBColumnInfo( 3,
      LBCIA_Column,                 0,
        LBCIA_Title,                " Title",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               50,
      LBCIA_Column,                 1,
        LBCIA_Title,                " Country",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               30,
      LBCIA_Column,                 2,
        LBCIA_Title,                " Bitrate",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               20,
      TAG_DONE);

  podcastColInfo = IListBrowser->AllocLBColumnInfo( 2,
      LBCIA_Column,                 0,
        LBCIA_Title,                " Title",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               80,
      LBCIA_Column,                 1,
        LBCIA_Title,                " Language",
        LBCIA_AutoSort,             TRUE,
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Sortable,             TRUE,
        LBCIA_SortArrow,            TRUE,
        LBCIA_Weight,               20,
      TAG_DONE);

  podcastEpisodeColInfo = IListBrowser->AllocLBColumnInfo( 2,
      LBCIA_Column,                 0,
        LBCIA_Title,                " Title",
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Weight,               80,
      LBCIA_Column,                 1,
        LBCIA_Title,                " Released",
        LBCIA_DraggableSeparator,   TRUE,
        LBCIA_Weight,               20,
      TAG_DONE);

  mediaServerColInfo = IListBrowser->AllocLBColumnInfo( 1,
      LBCIA_Column,                 0,
        LBCIA_Title,                " Title",
        //LBCIA_DraggableSeparator,   TRUE,
        //LBCIA_Weight,               80,
      TAG_DONE);
}
