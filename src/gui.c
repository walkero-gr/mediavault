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


#include "globals.h"
#include "gui.h"
#include "guifuncs.h"
#include "mainWin.h"
#include "aboutWin.h"
#include "radiofuncs.h"

static struct ColumnInfo *columnInfo, *leftSidebarCI;
struct List radioList,
            radioPopularList,
            radioTrendList,
            leftSidebarList;

struct filters lastFilters, prevFilters;

static void fillLeftSidebar(void);
static void fillRadioList(BOOL);
static void fillRadioPopularList(void);
static void fillRadioTrendList(void);
static BOOL checkFiltersChanged(void);
static void changeDiscoverButton(BOOL);

extern NETWORKOBJ *net;
extern int8 maxRadioResults;

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort)
  {
    if((screen = IIntuition->LockPubScreen(NULL)))
    {
      //drInfo = IIntuition->GetScreenDrawInfo(screen);
      menus[MID_PROJECT]  = buildMainMenu(screen);
      objects[OID_MAIN]   = buildMainWindow(appPort, menus[MID_PROJECT]);
      objects[OID_ABOUT]  = buildAboutWindow(appPort, screen);

      gadgets[GID_MSG_REQ] = IIntuition->NewObject(NULL, "requester.class",
          REQ_Type, REQTYPE_INFO,
          REQ_StayOnTop, TRUE,
          REQ_GadgetText, "_Ok",
          TAG_DONE);

      if (objects[OID_MAIN])
      {
        windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);
        IIntuition->UnlockPubScreen(NULL, screen);

        if (windows[WID_MAIN])
        {
          uint32  signal = 0,
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

          IIntuition->GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);

          while (!done)
          {
            uint32 wait = IExec->Wait(signal | SIGBREAKF_CTRL_C);
            if ( wait & SIGBREAKF_CTRL_C )
            {
              break;
            }

            if ( wait & signal )
            {
              uint32  result = WMHI_LASTMSG,
                      res_value,
                      res_node,
                      lsbNodeIdx;
              STRPTR  selListValue; 

              // Main Window events
              while ((result = IIntuition->IDoMethod(objects[OID_MAIN], WM_HANDLEINPUT, &code, TAG_DONE)))
              {
                switch (result & WMHI_CLASSMASK)
                {
                  case WMHI_CLOSEWINDOW:
                    windows[WID_MAIN] = NULL;
                    done = TRUE;
                    break;
                  case WMHI_ICONIFY:
                    IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY, TAG_DONE);
                    windows[WID_MAIN] = NULL;
                    break;
                  case WMHI_UNICONIFY:
                    windows[WID_MAIN] = (struct Window *)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);
                    break;
                  case WMHI_MENUPICK:
                    selectedMenu = NO_MENU_ID;
                    while ((selectedMenu = IIntuition->IDoMethod(menus[MID_PROJECT], MM_NEXTSELECT, 0, selectedMenu, TAG_DONE)) != NO_MENU_ID)
                    {
                      switch (selectedMenu)
                      {
                        case MID_ICONIFY:
                          IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY, TAG_DONE);
                          windows[WID_MAIN] = NULL;
                          break;
                        case MID_ABOUT:
                          windows[WID_ABOUT] = (struct Window*)IIntuition->IDoMethod(objects[OID_ABOUT], WM_OPEN, TAG_DONE);
                          windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                          break;
                        case MID_QUIT:
                          done = TRUE;
                          break;
                      }
                    }

                    break;
                  case WMHI_GADGETUP:
                    switch (result & WMHI_GADGETMASK)
                    {
                      case GID_FILTER_BUTTON:
                        IUtility->Strlcpy(lastFilters.name, ((struct StringInfo *)(((struct Gadget *)gadgets[GID_FILTERS_NAME])->SpecialInfo))->Buffer, sizeof(lastFilters.name));

                        windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                        if(checkFiltersChanged())
                        {
                          changeDiscoverButton(FALSE);
                          fillRadioList(TRUE);
                        }                                                           
                        else {
                          changeDiscoverButton(TRUE);
                          fillRadioList(FALSE);
                        }
                        windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);              
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
                        IIntuition->GetAttr(LISTBROWSER_RelEvent, gadgets[GID_RADIO_LISTBROWSER], &res_value);
                        if (res_value == LBRE_DOUBLECLICK)
                        {
                          IIntuition->GetAttr(LISTBROWSER_SelectedNode, gadgets[GID_RADIO_LISTBROWSER], (uint32 *)&res_node);
                          IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
                              LBNA_Column,  3,
                              LBNCA_Text,   &selListValue,
                              TAG_DONE);
                          playRadio(selListValue);
                        }
                        break;
                      case GID_RADIO_POPULAR_LISTBROWSER:
                        IIntuition->GetAttr(LISTBROWSER_RelEvent, gadgets[GID_RADIO_POPULAR_LISTBROWSER], &res_value);
                        if (res_value == LBRE_DOUBLECLICK)
                        {
                          IIntuition->GetAttr(LISTBROWSER_SelectedNode, gadgets[GID_RADIO_POPULAR_LISTBROWSER], (uint32 *)&res_node);
                          IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
                              LBNA_Column,  3,
                              LBNCA_Text,   &selListValue,
                              TAG_DONE);
                          playRadio(selListValue);
                        }
                        break;
                      case GID_RADIO_TREND_LISTBROWSER:
                        IIntuition->GetAttr(LISTBROWSER_RelEvent, gadgets[GID_RADIO_TREND_LISTBROWSER], &res_value);
                        if (res_value == LBRE_DOUBLECLICK)
                        {
                          IIntuition->GetAttr(LISTBROWSER_SelectedNode, gadgets[GID_RADIO_TREND_LISTBROWSER], (uint32 *)&res_node);
                          IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
                              LBNA_Column,  3,
                              LBNCA_Text,   &selListValue,
                              TAG_DONE);
                          playRadio(selListValue);
                        }
                        break;
                      case GID_LEFT_SIDEBAR:
                        IIntuition->GetAttr(LISTBROWSER_Selected, gadgets[GID_LEFT_SIDEBAR], &lsbNodeIdx);
                        switch (lsbNodeIdx)
                        {
                          case 0:
                            //IDOS->Printf("LSB_RADIO \n");
                            break;
                          case 1:
                            //IDOS->Printf("listCount Popular: %ld\n", listCount(&radioPopularList));
                            if(listCount(&radioPopularList) == 0)
                            {
                              windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                              fillRadioPopularList();
                              windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
                            }                            
                            break;
                          case 2:
                            if(listCount(&radioTrendList) == 0)
                            {
                              windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                              fillRadioTrendList();
                              windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
                            }
                            break;
                        }
                        break;
                    }
                    break; 
                }
              }

              //// About Window events
              while ((result = IIntuition->IDoMethod(objects[OID_ABOUT], WM_HANDLEINPUT, &code, TAG_DONE)))
              {
                switch(result & WMHI_CLASSMASK)
                {
                  case WMHI_CLOSEWINDOW:
                    IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
                    windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
                    break;
                  case WMHI_GADGETUP:
                    switch(result & WMHI_GADGETMASK)
                    {
                      case GID_ABOUT_BUTTON_OK:
                        IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
                        windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
                        break;
                    }
                    break;
                }
              }
            }
          }

          IListBrowser->FreeLBColumnInfo(columnInfo);
          if(listCount(&radioList))
          {
            IListBrowser->FreeListBrowserList(&radioList);
          }
          if(listCount(&radioPopularList))
          {
            IListBrowser->FreeListBrowserList(&radioPopularList);
          }
          if(listCount(&radioTrendList))
          {
            IListBrowser->FreeListBrowserList(&radioTrendList);
          }

          IListBrowser->FreeLBColumnInfo(leftSidebarCI);
          IListBrowser->FreeListBrowserList(&leftSidebarList);

          IIntuition->DisposeObject(objects[OID_ABOUT]);
          IIntuition->DisposeObject(objects[OID_MAIN]);
          IIntuition->DisposeObject(menus[MID_PROJECT]);
        }
      }
      //IIntuition->FreeScreenDrawInfo(screen, drInfo);
    }
  }
  IExec->FreeSysObject(ASOT_PORT, appPort);
}

static void listStations(
  struct Gadget *listbrowser,
  STRPTR responseJSON,
  struct List *list,
  int offset,
  char *notFoundMsg,
  void (*maxResultCallback)(BOOL)
) {
  size_t stationsCnt = 0;
  char jsonErrorMsg[] = "There was an error with the returned data.\nPlease, try again or check your network.";

  // Detach list before modify it
  IIntuition->SetAttrs(listbrowser,
      LISTBROWSER_Labels, NULL,
      TAG_DONE);

  if (responseJSON)
  {
    stationsCnt = getRadioList(list, responseJSON, offset);
    
    if (stationsCnt == 0)
    {
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", notFoundMsg);
    }

    if (stationsCnt == maxRadioResults)
    {
      if (maxResultCallback)
      {
        maxResultCallback(TRUE);
      }
    }

  } else showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg);

  // Dispose net here, after the creation of the listbrowser content,
  // because it trashes the response data, so to free the signals
  // TODO: adapt it to oo.library v1.11 changes
  if (net)
  {
    net->DisposeConnection();
    IOO->DisposeNetworkObject(net);
  }

  if (stationsCnt)
  {
    IIntuition->SetGadgetAttrs(listbrowser, windows[WID_MAIN], NULL,
        LISTBROWSER_Labels,         (ULONG)list,
        LISTBROWSER_SortColumn,     0,
        LISTBROWSER_Selected,       -1,
        LISTBROWSER_ColumnInfo,     columnInfo,
        TAG_DONE);

  }
}

static void fillRadioList(BOOL newSearch)
{
  char notFoundMsg[] = "No Radio Stations found with these criteria!\nChange them and try again!";
  static int offset;

  if (newSearch)
  {
    offset = 0;
  }
  else offset++;
  
  STRPTR responseJSON = getRadioStations(lastFilters, offset);
  listStations((struct Gadget*)gadgets[GID_RADIO_LISTBROWSER], responseJSON, &radioList, offset, (char *)notFoundMsg, changeDiscoverButton);
}

static void fillRadioPopularList(void)
{
  char notFoundMsg[] = "No Popular Radio Stations found!";
  STRPTR responseJSON = getRadioPopularStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_POPULAR_LISTBROWSER], responseJSON, &radioPopularList, 0, (char *)notFoundMsg, NULL);
}

static void fillRadioTrendList(void)
{
  char notFoundMsg[] = "No Trending Radio Stations found!";
  STRPTR responseJSON = getRadioTrendStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_TREND_LISTBROWSER], responseJSON, &radioTrendList, 0, (char *)notFoundMsg, NULL);
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
      LISTBROWSER_Labels,         (ULONG)&leftSidebarList,
      LISTBROWSER_Selected,       0,
      LISTBROWSER_ColumnInfo,     leftSidebarCI,
      TAG_DONE);
}
