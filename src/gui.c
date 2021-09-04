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
#include "gui.h"
#include "guifuncs.h"
#include "mainWin.h"
#include "aboutWin.h"
#include "radiofuncs.h"
#include "httpfuncs.h"

static struct ColumnInfo *columnInfo, *leftSidebarCI;
struct List radioList,
            radioPopularList,
            radioTrendList,
            leftSidebarList;

struct filters lastFilters, prevFilters;
struct RenderHook *renderhook;

static void fillLeftSidebar(void);
static void fillRadioList(BOOL);
static void fillRadioPopularList(void);
static void fillRadioTrendList(void);
static BOOL checkFiltersChanged(void);
static void changeDiscoverButton(BOOL);

extern uint8 maxRadioResults;
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

          fillLeftSidebar();

          columnInfo = IListBrowser->AllocLBColumnInfo( 3,
              LBCIA_Column,                 0,
                LBCIA_Title,                " Station",
                LBCIA_AutoSort,             TRUE,
                LBCIA_DraggableSeparator,   TRUE,
                LBCIA_Sortable,             TRUE,
                LBCIA_SortArrow,            TRUE,
                LBCIA_Weight,               60,
              LBCIA_Column,                 1,
                LBCIA_Title,                " Country",
                LBCIA_AutoSort,             TRUE,
                LBCIA_DraggableSeparator,   TRUE,
                LBCIA_Sortable,             TRUE,
                LBCIA_SortArrow,            TRUE,
                LBCIA_Weight,               30,
              LBCIA_Column,                 2,
                LBCIA_Title,                " Votes",
                LBCIA_AutoSort,             TRUE,
                LBCIA_DraggableSeparator,   TRUE,
                LBCIA_Sortable,             TRUE,
                LBCIA_SortArrow,            TRUE,
                LBCIA_Weight,               10,
              TAG_DONE);

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
                      res_node,
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
                        IUtility->Strlcpy(lastFilters.name, ((struct StringInfo *)(((struct Gadget *)gadgets[GID_FILTERS_NAME])->SpecialInfo))->Buffer, sizeof(lastFilters.name));

                        windowBlocking(objects[OID_MAIN], TRUE);
                        if(checkFiltersChanged())
                        {
                          changeDiscoverButton(FALSE);
                          fillRadioList(TRUE);
                        }
                        else {
                          changeDiscoverButton(TRUE);
                          fillRadioList(FALSE);
                        }
                        windowBlocking(objects[OID_MAIN], FALSE);
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
                        {
                          Object *lb = NULL;

                          if ((result & WMHI_GADGETMASK) == GID_RADIO_LISTBROWSER) lb = gadgets[GID_RADIO_LISTBROWSER];
                          else if ((result & WMHI_GADGETMASK) == GID_RADIO_POPULAR_LISTBROWSER) lb = gadgets[GID_RADIO_POPULAR_LISTBROWSER];
                          else lb = gadgets[GID_RADIO_TREND_LISTBROWSER];


                          IIntuition->GetAttr(LISTBROWSER_RelEvent, lb, &res_value);

                          // TODO: Remove this when the play button is added at the right sidebar
                          if (res_value == LBRE_DOUBLECLICK)
                          {
                            IIntuition->GetAttr(LISTBROWSER_SelectedNode, lb, (uint32 *)&res_node);
                            playRadio((struct Node *)res_node);
                          }
                          else if (res_value == LBRE_NORMAL)
                          {
                            IIntuition->GetAttr(LISTBROWSER_SelectedNode, lb, (uint32 *)&res_node);
                            windowBlocking(objects[OID_MAIN], TRUE);
                            showRadioInfo((struct Node *)res_node);
                            windowBlocking(objects[OID_MAIN], FALSE);
                          }
                        }
                        break;

                      case GID_LEFT_SIDEBAR:
                        IIntuition->GetAttr(LISTBROWSER_Selected, gadgets[GID_LEFT_SIDEBAR], &lsbNodeIdx);
                        switch (lsbNodeIdx)
                        {
                          case 1:
                            if(listCount(&radioPopularList) == 0)
                            {
                              windowBlocking(objects[OID_MAIN], TRUE);
                              fillRadioPopularList();
                              windowBlocking(objects[OID_MAIN], FALSE);
                            }
                            break;
                          case 2:
                            if(listCount(&radioTrendList) == 0)
                            {
                              windowBlocking(objects[OID_MAIN], TRUE);
                              fillRadioTrendList();
                              windowBlocking(objects[OID_MAIN], FALSE);
                            }
                            break;
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

          if (windows[WID_MAIN])
          {
            IIntuition->IDoMethod(objects[OID_MAIN], WM_CLOSE, TAG_DONE);
            windows[WID_MAIN] = NULL;
          }

          IListBrowser->FreeLBColumnInfo(columnInfo);
          if(listCount(&radioList))
          {
            freeList(&radioList, freeStationInfo);
          }

          if(listCount(&radioPopularList))
          {
            freeList(&radioPopularList, freeStationInfo);
          }

          if(listCount(&radioTrendList))
          {
            freeList(&radioTrendList, freeStationInfo);
          }

          IListBrowser->FreeLBColumnInfo(leftSidebarCI);
          IListBrowser->FreeListBrowserList(&leftSidebarList);

          IIntuition->DisposeObject(objects[OID_ABOUT]);
          IIntuition->DisposeObject(objects[OID_MAIN]);

          IExec->FreeSysObject(ASOT_HOOK, renderhook);
          IIntuition->DisposeObject(objects[OID_AVATAR_IMAGE]);

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

static void listStations(
  struct Gadget *listbrowser,
  struct List *list,
  int offset,
  char *notFoundMsg,
  void (*maxResultCallback)(BOOL)
) {
  size_t stationsCnt = 0;

  stationsCnt = getRadioList(list, offset);

  if (stationsCnt == ~0UL)
  {
    char jsonErrorMsg[] = "There was an error with the returned data.\nPlease try again or check your network.";
    showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
  }
  else if (stationsCnt == 0)
  {
    showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", notFoundMsg, 0, NULL, 0);
  }
  else if (stationsCnt == maxRadioResults)
  {
    if (maxResultCallback)
    {
      maxResultCallback(TRUE);
    }
  }

  if (((stationsCnt != ~0UL) && (stationsCnt > 0)) || (offset == 0))
  {
    // Detach list before modify it
    IIntuition->SetAttrs(listbrowser,
        LISTBROWSER_Labels, NULL,
        TAG_DONE);

    IIntuition->SetGadgetAttrs(listbrowser, windows[WID_MAIN], NULL,
        LISTBROWSER_Labels,         list,
        LISTBROWSER_SortColumn,     0,
        LISTBROWSER_Selected,       -1,
        LISTBROWSER_ColumnInfo,     columnInfo,
        TAG_DONE);
  }
}

static void fillRadioList(BOOL newSearch)
{
  static int offset;
  char notFoundMsg[128];
  IUtility->Strlcpy(notFoundMsg, "No more Radio Stations found!", sizeof(notFoundMsg));

  if (newSearch)
  {
    offset = 0;
    IUtility->Strlcpy(notFoundMsg, "No Radio Stations found with these criteria!\nChange them and try again!", sizeof(notFoundMsg));
  }
  else
    offset++;

  getRadioStations(lastFilters, offset);
  listStations((struct Gadget*)gadgets[GID_RADIO_LISTBROWSER], &radioList, offset, (char *)notFoundMsg, changeDiscoverButton);
}

static void fillRadioPopularList(void)
{
  char notFoundMsg[] = "No Popular Radio Stations found!";
  getRadioPopularStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_POPULAR_LISTBROWSER], &radioPopularList, 0, (char *)notFoundMsg, NULL);
}

static void fillRadioTrendList(void)
{
  char notFoundMsg[] = "No Trending Radio Stations found!";
  getRadioTrendStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_TREND_LISTBROWSER], &radioTrendList, 0, (char *)notFoundMsg, NULL);
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

static void changeDiscoverButton(BOOL isMore)
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

