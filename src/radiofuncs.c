/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: radiofuncs.c
**
** Date: 24-04-2021 10:01:11
**
************************************************************

*/

#include <proto/listbrowser.h>

#include "globals.h"
#include "gui.h"
#include "libshandler.h"
#include "httpfuncs.h"
#include "stringfuncs.h"
#include "guifuncs.h"
#include "fsfuncs.h"

static CONST_STRPTR radioAPIUrl = "https://de1.api.radio-browser.info/json";
static char url[255];
static uint8 maxRadioResults = 100;

extern struct memory response;
extern struct RenderHook *renderhook;
extern struct ColumnInfo *columnInfo;
extern struct List radioList,
                radioPopularList,
                radioTrendList;

static void setBaseSearchUrl(void)
{
  char maxRadioResultsStr[5];

  IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/stations/search?hidebroken=true&limit=", sizeof(url));
  IUtility->SNPrintf(maxRadioResultsStr, sizeof(maxRadioResultsStr), "%ld", maxRadioResults);
  IUtility->Strlcat(url, maxRadioResultsStr, sizeof(url));
}

BOOL getRadioStations(struct filters lastFilters, int offset)
{
  BOOL success = FALSE;
  setBaseSearchUrl();

  if (offset > 0)
  {
    char offsetStr[5];
    IUtility->Strlcat(url, "&offset=", sizeof(url));
    IUtility->SNPrintf(offsetStr, sizeof(offsetStr), "%ld", offset * maxRadioResults);
    IUtility->Strlcat(url, offsetStr, sizeof(url));
  }

  if (IUtility->Stricmp(lastFilters.name, ""))
  {
    STRPTR encSelName = urlEncode(lastFilters.name);
    IUtility->Strlcat(url, "&name=", sizeof(url));
    IUtility->Strlcat(url, encSelName, sizeof(url));
    IExec->FreeVec(encSelName);
  }
  if (IUtility->Stricmp(lastFilters.genre, ""))
  {
    STRPTR encSelGenre = urlEncode(lastFilters.genre);
    IUtility->Strlcat(url, "&tag=", sizeof(url));
    IUtility->Strlcat(url, encSelGenre, sizeof(url));
    IExec->FreeVec(encSelGenre);
  }
  if (IUtility->Stricmp(lastFilters.language, ""))
  {
    STRPTR encSelLanguage = urlEncode(lastFilters.language);
    IUtility->Strlcat(url, "&language=", sizeof(url));
    IUtility->Strlcat(url, encSelLanguage, sizeof(url));
    IExec->FreeVec(encSelLanguage);
  }
  if (IUtility->Stricmp(lastFilters.country, ""))
  {
    STRPTR encSelCountry = urlEncode(lastFilters.country);
    IUtility->Strlcat(url, "&country=", sizeof(url));
    IUtility->Strlcat(url, encSelCountry, sizeof(url));
    IExec->FreeVec(encSelCountry);
  }

  doHTTPRequest(url);
  if (getResponseCode() == 200)
    success = TRUE;

  return success;
}

void getRadioTrendStations(void)
{
  setBaseSearchUrl();
  IUtility->Strlcat(url, "&order=clicktrend", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));

  doHTTPRequest(url);
}

void getRadioPopularStations(void)
{
  setBaseSearchUrl();
  IUtility->Strlcat(url, "&order=clickcount", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));

  doHTTPRequest(url);
}

size_t getRadioList(struct List *stationList, int offset)
{
  LONG  responseCode = getResponseCode();
  if (responseCode != 200)
    return ~0UL;

  STRPTR responseBody = getResponseBody();
  if (responseBody == NULL)
    return ~0UL;

  json_t *jsonRoot;
  json_error_t jsonError;
  size_t cnt;

  jsonRoot = IJansson->json_loads(responseBody, 0, &jsonError);
  cleanupHTTPRequest();

  if(!jsonRoot)
  {
    //IDOS->Printf("json error: on line %ld: %s\n", jsonError.line, jsonError.text);
    return ~0UL;
  }

  if (!json_is_array(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    //IDOS->Printf("JSON error: jsonRoot is not an array");
    return ~0UL;
  }

  if (offset == 0)
  {
    IExec->NewList(stationList);
  }

  for(cnt = 0; cnt < IJansson->json_array_size(jsonRoot); cnt++)
  {
    struct Node *stationNode;
    struct stationInfo *stationData = NULL;
    json_t *data, *buf;

    stationData = (struct stationInfo *)IExec->AllocVecTags(sizeof(struct stationInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    data = IJansson->json_array_get(jsonRoot, cnt);
    if(!json_is_object(data))
    {
      //IDOS->Printf("error: commit data %d is not an object\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }

    buf = IJansson->json_object_get(data, "stationuuid");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->uuid, IJansson->json_string_value(buf), sizeof(stationData->uuid));

    buf = IJansson->json_object_get(data, "name");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->name, charConv(IJansson->json_string_value(buf)), sizeof(stationData->name));

    buf = IJansson->json_object_get(data, "country");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->country, charConv(IJansson->json_string_value(buf)), sizeof(stationData->country));

    buf = IJansson->json_object_get(data, "language");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->language, charConv(IJansson->json_string_value(buf)), sizeof(stationData->language));

    buf = IJansson->json_object_get(data, "url_resolved");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->url_resolved, IJansson->json_string_value(buf), sizeof(stationData->url_resolved));

    buf = IJansson->json_object_get(data, "homepage");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->homepage, IJansson->json_string_value(buf), sizeof(stationData->homepage));

    buf = IJansson->json_object_get(data, "favicon");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->favicon, IJansson->json_string_value(buf), sizeof(stationData->favicon));

    buf = IJansson->json_object_get(data, "codec");
    if(!json_is_string(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    IUtility->Strlcpy(stationData->codec, IJansson->json_string_value(buf), sizeof(stationData->codec));

    buf = IJansson->json_object_get(data, "bitrate");
    if(json_is_integer(buf))
    {
      stationData->bitrate = (uint8)IJansson->json_integer_value(buf);
    }
    else stationData->bitrate = 0;

    buf = IJansson->json_object_get(data, "votes");
    if(!json_is_integer(buf))
    {
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    stationData->votes = (ULONG)IJansson->json_integer_value(buf);

    char codecBitrate[15];
    IUtility->SNPrintf(codecBitrate, sizeof(codecBitrate), "%ld kbps %s", stationData->bitrate, stationData->codec);

    stationNode = IListBrowser->AllocListBrowserNode( 5,
        LBNA_UserData,          stationData,
        LBNA_Column,            0,
          LBNCA_CopyText,       TRUE,
          LBNCA_Text,           stationData->name,
        LBNA_Column,            1,
          LBNCA_CopyText,       TRUE,
          LBNCA_Text,           stationData->country,
        LBNA_Column,            2,
          LBNCA_CopyText,       TRUE,
          LBNCA_Text,           codecBitrate,
        LBNA_Column,            3,
          LBNCA_CopyInteger,    TRUE,
          LBNCA_Integer,        &stationData->votes,
          LBNCA_Justification,  LCJ_RIGHT,
        TAG_DONE);

    if(stationNode)
    {
      IExec->AddTail(stationList, stationNode);
    }
  }
  IJansson->json_decref(jsonRoot);
  return cnt;
}

void playRadio(struct Node *res_node)
{
  if (res_node)
  {
    char startPlayerPath[128];
    STRPTR cmd = NULL;
    struct stationInfo *itemData = NULL;
    itemData = (struct stationInfo *)IExec->AllocVecTags(sizeof(struct stationInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
          LBNA_UserData, &itemData,
          TAG_DONE);

    IUtility->SNPrintf(startPlayerPath, sizeof(startPlayerPath), "%s/scripts/start_player", getFilePath((STRPTR)"PROGDIR:MediaVault"));
    if (fileExists(startPlayerPath))
    {
      cmd = IUtility->ASPrintf("execute %s \"%s\" ", startPlayerPath, itemData->url_resolved);
    }
    else
    {
      cmd = IUtility->ASPrintf("Run <>NIL: APPDIR:AmigaAmp3 \"%s\" ", itemData->url_resolved);
    }

    IDOS->SystemTags( cmd,
        SYS_Input,    ZERO,
        SYS_Output,   NULL,
        SYS_Error,    ZERO,
        SYS_Asynch,   TRUE,
        TAG_DONE);
  }

  // TODO: Free itemData
}

void showRadioInfo(struct Node *res_node)
{
  if (res_node)
  {
    char radioInfo[512];
    struct stationInfo *itemData = NULL;
    itemData = (struct stationInfo *)IExec->AllocVecTags(sizeof(struct stationInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
          LBNA_UserData, &itemData,
          TAG_DONE);

    showAvatarImage(itemData->uuid, itemData->favicon, gadgets[GID_INFO_RADIO_DATA], objects[OID_AVATAR_IMAGE], renderhook);

    IUtility->SNPrintf(radioInfo, sizeof(radioInfo), "%s\n%s\n%ld kbps %s\n",
          itemData->name, itemData->country, itemData->bitrate, itemData->codec);

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_RADIO_DATA], windows[WID_MAIN], NULL,
          GA_TEXTEDITOR_Contents,   radioInfo,
          TAG_DONE);
    
    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_PLAY_BUTTON], windows[WID_MAIN], NULL,
          GA_Disabled,   FALSE,
          TAG_DONE);

    // TODO: Free itemData
  }
}

// TODO: Change like the one I did on podcasts funcs
static BOOL listStations(
  struct Gadget *listbrowser,
  struct List *list,
  int offset,
  char *notFoundMsg,
  void (*maxResultCallback)(BOOL)
) {
  size_t stationsCnt = 0;
  BOOL success = FALSE;

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

  if ((stationsCnt != ~0UL) && (stationsCnt > 0))
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

    success = TRUE;
  }

  return success;
}

void fillRadioList(struct filters lastFilters, BOOL newSearch)
{
  static int offset;
  char notFoundMsg[128];
  IUtility->Strlcpy(notFoundMsg, "No more Radio Stations found!", sizeof(notFoundMsg));

  if (newSearch)
  {
    offset = 0;
    IUtility->Strlcpy(notFoundMsg, "No Radio Stations found with these criteria!\nChange them and try again!", sizeof(notFoundMsg));
  }

  if (getRadioStations(lastFilters, offset))
  {
    if (listStations((struct Gadget*)gadgets[GID_RADIO_LISTBROWSER], &radioList, offset, (char *)notFoundMsg, changeDiscoverButton))
    {
      offset++;
    }
  }
}

void fillRadioPopularList(void)
{
  char notFoundMsg[] = "No Popular Radio Stations found!";
  getRadioPopularStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_POPULAR_LISTBROWSER], &radioPopularList, 0, (char *)notFoundMsg, NULL);
}

void fillRadioTrendList(void)
{
  char notFoundMsg[] = "No Trending Radio Stations found!";
  getRadioTrendStations();
  listStations((struct Gadget*)gadgets[GID_RADIO_TREND_LISTBROWSER], &radioTrendList, 0, (char *)notFoundMsg, NULL);
}
