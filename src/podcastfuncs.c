/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: podcastfuncs.c
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
#include "sqldb.h"
#include "stringfuncs.h"
#include "guifuncs.h"
#include "secrets.h"
#include "fsfuncs.h"

static CONST_STRPTR podcastAPIUrl = "https://api.podcastindex.org/api/1.0";
static uint8 maxResults = 100;
static uint8 maxEpisodesResults = 100;

extern struct memory response;
extern struct RenderHook *podcastImageRenderHook;
extern struct ColumnInfo *podcastColInfo, *podcastEpisodeColInfo;
extern struct List  podcastList, podcastFavouriteList,
                    trendingPodcastList, podcastEpisodeList;

static void fillEpisodesList(ULONG);
static void toggleFavouriteButton(BOOL);

static void addRequestHeaders(void)
{
  STRPTR buf = IExec->AllocVecTags(sizeof(char) * 90,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  char authString[71];   
  ULONG nowSecondsUTC = nowUnix(now()) + getOffsetUTC();

  IUtility->SNPrintf(authString, sizeof(authString), "%s%s%lu", PODCASTINDEX_KEY, PODCASTINDEX_SECRET, nowSecondsUTC);

  IUtility->Strlcpy(buf, "X-Auth-Key: " PODCASTINDEX_KEY, sizeof(char) * 90);
  addRequestHeader(buf);

  IUtility->SNPrintf(buf, sizeof(char) * 90, "X-Auth-Date: %lu", nowSecondsUTC);
  addRequestHeader(buf);

  IUtility->Strlcpy(buf, "Authorization: ", sizeof(char) * 90);
  IUtility->Strlcat(buf, SHA1Encrypt(authString), sizeof(char) * 90);
  addRequestHeader(buf);

  IExec->FreeVec(buf);
}

static BOOL getPodcasts(struct filters lastFilters, int offset)
{
  BOOL success = FALSE;
  char url[255];

  IUtility->SNPrintf(url, sizeof(url), "%s/search/byterm?max=%ld", podcastAPIUrl, maxResults);

  // TODO: Check if there is offset in podcast API and use it here
  if (offset > 0)
    offset = 0;
  /*
  if (offset > 0)
  {
    char offsetStr[5];
    IUtility->Strlcat(url, "&offset=", sizeof(url));
    IUtility->SNPrintf(offsetStr, sizeof(offsetStr), "%ld", offset * maxRadioResults);
    IUtility->Strlcat(url, offsetStr, sizeof(url));
  }
  */

  if (IUtility->Stricmp(lastFilters.name, ""))
  {
    STRPTR encSelName = urlEncode(lastFilters.name);
    IUtility->Strlcat(url, "&q=", sizeof(url));
    IUtility->Strlcat(url, encSelName, sizeof(url));
    IExec->FreeVec(encSelName);
  }

  addRequestHeaders();
  doHTTPRequest(url);
  if (getResponseCode() == 200)
    success = TRUE;

  return success;
}

static BOOL getTrendingPodcasts(struct filters lastFilters, int offset)
{
  BOOL success = FALSE;
  char url[255];

  IUtility->SNPrintf(url, sizeof(url), "%s/podcasts/trending?max=%ld&pretty=", podcastAPIUrl, maxResults);

  // TODO: Check if there is offset in podcast API and use it here
  if (offset > 0)
    offset = 0;

  if (IUtility->Stricmp(lastFilters.genre, ""))
  {
    IUtility->SNPrintf(url, sizeof(url), "%s&cat=%s", url, urlEncode(lastFilters.genre));
  }

  if (IUtility->Stricmp(lastFilters.language, NULL))
  {
    IUtility->SNPrintf(url, sizeof(url), "%s&lang=%s", url, urlEncode(lastFilters.language));
  }

  addRequestHeaders();
  doHTTPRequest(url);
  if (getResponseCode() == 200)
    success = TRUE;

  return success;
}

size_t getPodcastList(struct List *itemsList, int offset)
{
  LONG  responseCode = getResponseCode();
  if (responseCode != 200)
    return ~0UL;

  STRPTR responseBody = getResponseBody();
  if (responseBody == NULL)
    return ~0UL;

  json_t *jsonRoot;
  json_error_t jsonError;
  size_t cnt = 0;

  jsonRoot = IJansson->json_loads(responseBody, 0, &jsonError);
  cleanupHTTPRequest();

  if(!jsonRoot)
  {
    IDOS->Printf("json error: on line %ld: %s\n", jsonError.line, jsonError.text);
    return ~0UL;
  }

  if (!json_is_object(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    IDOS->Printf("JSON error: jsonRoot is not an object\n");
    return ~0UL;
  }

  json_t *feeds = IJansson->json_object_get(jsonRoot, "feeds");
  if (json_is_array(feeds))
  {
    if (offset == 0)
    {
      IExec->NewList(itemsList);
    }

    for(cnt = 0; cnt < IJansson->json_array_size(feeds); cnt++)
    {
      struct Node *itemNode;
      struct podcastInfo *itemData = NULL;
      json_t *data, *buf;

      itemData = (struct podcastInfo *)IExec->AllocVecTags(sizeof(struct podcastInfo),
            AVT_Type,            MEMF_PRIVATE,
            AVT_ClearWithValue,  "\0",
            TAG_DONE);

      data = IJansson->json_array_get(feeds, cnt);
      if(!json_is_object(data))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }

      buf = IJansson->json_object_get(data, "id");
      if(!json_is_integer(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      itemData->id = (ULONG)IJansson->json_integer_value(buf);

      buf = IJansson->json_object_get(data, "title");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->title, charConv(IJansson->json_string_value(buf)), sizeof(itemData->title));

      buf = IJansson->json_object_get(data, "author");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->author, charConv(IJansson->json_string_value(buf)), sizeof(itemData->author));

      buf = IJansson->json_object_get(data, "language");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->language, IJansson->json_string_value(buf), sizeof(itemData->language));

      buf = IJansson->json_object_get(data, "url");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->url, IJansson->json_string_value(buf), sizeof(itemData->url));

      buf = IJansson->json_object_get(data, "originalUrl");
      if(json_is_string(buf))
      {
        //IJansson->json_decref(jsonRoot);
        //return ~0UL;
        IUtility->Strlcpy(itemData->originalUrl, IJansson->json_string_value(buf), sizeof(itemData->originalUrl));
      }
      //IUtility->Strlcpy(itemData->originalUrl, IJansson->json_string_value(buf), sizeof(itemData->originalUrl));

      buf = IJansson->json_object_get(data, "image");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->image, IJansson->json_string_value(buf), sizeof(itemData->image));

      buf = IJansson->json_object_get(data, "description");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->description, charConv(IJansson->json_string_value(buf)), sizeof(itemData->description));

      itemNode = IListBrowser->AllocListBrowserNode( 3,
          LBNA_UserData,          itemData,
          LBNA_Column,            0,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->title,
          LBNA_Column,            1,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->language,
          TAG_DONE);

      if(itemNode)
      {
        IExec->AddTail(itemsList, itemNode);
      }
    }
  }

  IJansson->json_decref(jsonRoot);
  return cnt;
}

size_t getPodcastEpisodeList(struct List *itemsList, int offset)
{
  LONG  responseCode = getResponseCode();
  if (responseCode != 200)
    return ~0UL;

  STRPTR responseBody = getResponseBody();
  if (responseBody == NULL)
    return ~0UL;

  json_t *jsonRoot;
  json_error_t jsonError;
  size_t cnt = 0;

  jsonRoot = IJansson->json_loads(responseBody, 0, &jsonError);
  cleanupHTTPRequest();

  if(!jsonRoot)
  {
    IDOS->Printf("json error: on line %ld: %s\n", jsonError.line, jsonError.text);
    return ~0UL;
  }

  if (!json_is_object(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    IDOS->Printf("JSON error: jsonRoot is not an object\n");
    return ~0UL;
  }

  json_t *feeds = IJansson->json_object_get(jsonRoot, "items");
  if (json_is_array(feeds))
  {
    if (offset == 0)
    {
      IExec->NewList(itemsList);
    }

    for(cnt = 0; cnt < IJansson->json_array_size(feeds); cnt++)
    {
      struct Node *itemNode;
      struct podcastEpisodeInfo *itemData = NULL;
      json_t *data, *buf;

      itemData = (struct podcastEpisodeInfo *)IExec->AllocVecTags(sizeof(struct podcastEpisodeInfo),
            AVT_Type,            MEMF_PRIVATE,
            AVT_ClearWithValue,  "\0",
            TAG_DONE);

      data = IJansson->json_array_get(feeds, cnt);
      if(!json_is_object(data))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }

      buf = IJansson->json_object_get(data, "id");
      if(!json_is_integer(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      itemData->id = (ULONG)IJansson->json_integer_value(buf);

      buf = IJansson->json_object_get(data, "title");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->title, charConv(IJansson->json_string_value(buf)), sizeof(itemData->title));

      buf = IJansson->json_object_get(data, "datePublishedPretty");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->datePublishedPretty, IJansson->json_string_value(buf), sizeof(itemData->datePublishedPretty));

      buf = IJansson->json_object_get(data, "enclosureUrl");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->enclosureUrl, IJansson->json_string_value(buf), sizeof(itemData->enclosureUrl));
      
      buf = IJansson->json_object_get(data, "description");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->description, charConv(IJansson->json_string_value(buf)), sizeof(itemData->description));

      buf = IJansson->json_object_get(data, "feedImage");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->image, IJansson->json_string_value(buf), sizeof(itemData->image));

      buf = IJansson->json_object_get(data, "feedId");
      if(!json_is_integer(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      itemData->feedId = (ULONG)IJansson->json_integer_value(buf);

      buf = IJansson->json_object_get(data, "season");
      if(!json_is_integer(buf))
      {
        itemData->season = 0;
      } else itemData->season = (ULONG)IJansson->json_integer_value(buf);


      buf = IJansson->json_object_get(data, "episode");
      if(!json_is_integer(buf))
      {
        itemData->episode = 0;
      } else itemData->episode = (ULONG)IJansson->json_integer_value(buf);

      itemNode = IListBrowser->AllocListBrowserNode( 3,
          LBNA_UserData,          itemData,
          LBNA_Column,            0,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->title,
          LBNA_Column,            1,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->datePublishedPretty,
          TAG_DONE);

      if(itemNode)
      {
        IExec->AddTail(itemsList, itemNode);
      }
    }
  }

  IJansson->json_decref(jsonRoot);
  return cnt;
}

int getPodcastFavourite(void *unused, int cntCols, char **fields, char **colNames)
{
  char itemUID[32];
  struct Node *itemNode;
  struct podcastInfo *itemData = {0};

  itemData = (struct podcastInfo *)IExec->AllocVecTags(sizeof(struct podcastInfo),
      AVT_Type,            MEMF_PRIVATE,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);
  
  IUtility->Strlcpy(itemUID, strcpy(itemUID, fields[0] + 4), sizeof(itemUID));
  itemData->id = atoi(itemUID);
  IUtility->Strlcpy(itemData->title,        fields[4], sizeof(itemData->title));
  IUtility->Strlcpy(itemData->language,     fields[6], sizeof(itemData->language));
  IUtility->Strlcpy(itemData->image,        fields[10], sizeof(itemData->image));
  IUtility->Strlcpy(itemData->description,  fields[11], sizeof(itemData->description));

  itemNode = IListBrowser->AllocListBrowserNode( 3,
      LBNA_UserData,          itemData,
      LBNA_Column,            0,
        LBNCA_CopyText,       TRUE,
        LBNCA_Text,           fields[4],
      LBNA_Column,            1,
        LBNCA_CopyText,       TRUE,
        LBNCA_Text,           fields[6],
      TAG_DONE);

  if(itemNode)
  {
    IExec->AddTail(&podcastFavouriteList, itemNode);
  }

  // Dummy variable usage for gcc happiness
  if (unused) unused = 0;
  if (cntCols) cntCols = 0;
  if (colNames) colNames = NULL;

  return 0;
}

static ULONG getLatestEpisodePublishDate(struct List *list)
{
  if (list->lh_Head)
  {
    struct Node *node = list->lh_Head;

    struct podcastEpisodeInfo *episodeData = {0};
    episodeData = (struct podcastEpisodeInfo *)IExec->AllocVecTags(sizeof(struct podcastEpisodeInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)node,
          LBNA_UserData, &episodeData,
          TAG_DONE);

    return episodeData->datePublished;
  }

  return 0;
}

void showPodcastInfo(struct Node *res_node)
{
  if (res_node)
  {
    char infoText[641], itemUID[32];

    struct podcastInfo *podcastData = NULL;
    podcastData = (struct podcastInfo *)IExec->AllocVecTags(sizeof(struct podcastInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
          LBNA_UserData, &podcastData,
          TAG_DONE);

    IUtility->SNPrintf(infoText, sizeof(infoText), "%s\n\n%s\n", podcastData->title, podcastData->description);

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_INFO_DATA], windows[WID_MAIN], NULL,
          GA_TEXTEDITOR_Contents,   infoText,
          TAG_DONE);
    
    fillEpisodesList(podcastData->id);

    IUtility->SNPrintf(itemUID, sizeof(itemUID), "pod_%lu", podcastData->id);
    showAvatarImage(
      itemUID, podcastData->image,
      gadgets[GID_PODCAST_INFO_DATA], objects[OID_PODCAST_AVATAR_IMAGE],
      podcastImageRenderHook
    );

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_PLAY_BUTTON], windows[WID_MAIN], NULL,
        GA_Disabled,   TRUE,
        TAG_DONE);

    toggleFavouriteButton(sqlCheckExist(itemUID, "podcast"));
  }
}

void showPodcastEpisodeInfo(struct Node *res_node)
{
  if (res_node)
  {
    char infoText[641], itemUID[32];

    struct podcastEpisodeInfo *episodeData = NULL;
    episodeData = (struct podcastEpisodeInfo *)IExec->AllocVecTags(sizeof(struct podcastEpisodeInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
          LBNA_UserData, &episodeData,
          TAG_DONE);

    IUtility->SNPrintf(infoText, sizeof(infoText), "%s\nSeason: %ld, Episode: %ld\n\n%s\n",
      episodeData->title,
      episodeData->season,
      episodeData->episode,
      episodeData->description);
    IUtility->SNPrintf(itemUID, sizeof(itemUID), "pod_%lu", episodeData->feedId);

    if (IUtility->Stricmp(itemUID, ""))
    {
      showAvatarImage(
        itemUID, episodeData->image,
        gadgets[GID_PODCAST_INFO_DATA], objects[OID_PODCAST_AVATAR_IMAGE],
        podcastImageRenderHook
      );
    }

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_INFO_DATA], windows[WID_MAIN], NULL,
          GA_TEXTEDITOR_Contents,   infoText,
          TAG_DONE);

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_PLAY_BUTTON], windows[WID_MAIN], NULL,
          GA_Disabled,   FALSE,
          TAG_DONE);
  }
}

static BOOL getEpisodesByID(ULONG feedID)
{
  BOOL success = FALSE;
  if (feedID > 0)
  {
    char url[255];

    IUtility->SNPrintf(url, sizeof(url), "%s/episodes/byfeedid?id=%lu&max=%ld", podcastAPIUrl, feedID, maxEpisodesResults);
    
    addRequestHeaders();
    doHTTPRequest(url);
    if (getResponseCode() == 200)
      success = TRUE;
  }

  return success;
}

static void addListItems(struct Gadget *listbrowser, struct List *list, struct ColumnInfo *colInfo)
{
  // Detach list before modify it
  IIntuition->SetAttrs(listbrowser,
      LISTBROWSER_Labels, NULL,
      TAG_DONE);

  IIntuition->SetGadgetAttrs(listbrowser, windows[WID_MAIN], NULL,
      LISTBROWSER_Labels,         list,
      LISTBROWSER_SortColumn,     0,
      LISTBROWSER_Selected,       -1,
      LISTBROWSER_ColumnInfo,     colInfo,
      TAG_DONE);
}

void fillPodcastList(struct filters lastFilters)
{
  char jsonErrorMsg[] = "There was an error with the returned data.\n" \
      "Please try again. If the problem remains,\n" \
      "please, check your network and that\n"
      "your system time is synced.";
  
  if (getPodcasts(lastFilters, 0))
  {
    size_t itemsCnt = getPodcastList(&podcastList, 0);

    if (itemsCnt == ~0UL)
    {
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
    }
    else if (itemsCnt == 0)
    {
      char notFoundMsg[] = "No podcasts found based on your search criteria!";
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (char *)notFoundMsg, 0, NULL, 0);
    }

    if ((itemsCnt != ~0UL) && (itemsCnt > 0))
    {
      addListItems((struct Gadget*)gadgets[GID_PODCAST_LISTBROWSER], &podcastList, podcastColInfo);
    }
  }
  else showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
}

void fillPodcastFavouriteList(void)
{
  IExec->NewList(&podcastFavouriteList);
  sqlGetFavourites("podcast", getPodcastFavourite);

  if (listCount(&podcastFavouriteList) == 0)
  {
    // TODO: Add a message to the user
  }

  if (listCount(&podcastFavouriteList) > 0)
  {
    IIntuition->SetAttrs((struct Gadget*)gadgets[GID_PODCAST_FAVOURITE_LISTBROWSER],
      LISTBROWSER_Labels, NULL,
      TAG_DONE);

    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_FAVOURITE_LISTBROWSER], windows[WID_MAIN], NULL,
        LISTBROWSER_Labels,         &podcastFavouriteList,
        LISTBROWSER_SortColumn,     0,
        LISTBROWSER_Selected,       -1,
        LISTBROWSER_ColumnInfo,     podcastColInfo,
        TAG_DONE);
  }
  //char notFoundMsg[] = "No Favourite Radio Stations found!";
  //getRadioFavouriteStations();
  //listStations((struct Gadget*)gadgets[GID_RADIO_FAVOURITE_LISTBROWSER], &radioFavouriteList, 0, (char *)notFoundMsg, NULL);
}

void fillPodcastTrendingList(struct filters lastFilters)
{
  char jsonErrorMsg[] = "There was an error with the returned data.\n" \
      "Please try again. If the problem remains,\n" \
      "please, check your network and that\n"
      "your system time is synced.";

  if (getTrendingPodcasts(lastFilters, 0))
  {
    size_t itemsCnt = getPodcastList(&trendingPodcastList, 0);

    if (itemsCnt == ~0UL)
    {
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
    }
    else if (itemsCnt == 0)
    {
      char notFoundMsg[] = "No podcasts found based on your search criteria!";
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (char *)notFoundMsg, 0, NULL, 0);
    }

    if ((itemsCnt != ~0UL) && (itemsCnt > 0))
    {
      addListItems((struct Gadget*)gadgets[GID_PODCAST_TRENDING_LISTBROWSER], &trendingPodcastList, podcastColInfo);
    }
  }
  else showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
}

static void fillEpisodesList(ULONG feedID)
{
  char jsonErrorMsg[] = "There was an error with the returned data.\n" \
      "Please try again. If the problem remains,\n" \
      "please, check your network and that\n"
      "your system time is synced.";

  if (getEpisodesByID(feedID))
  {
    size_t itemsCnt = getPodcastEpisodeList(&podcastEpisodeList, 0);

    if (itemsCnt == ~0UL)
    {
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
    }
    else if (itemsCnt == 0)
    {
      char notFoundMsg[] = "The selected podcast has no episodes!";
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (char *)notFoundMsg, 0, NULL, 0);
    }

    if ((itemsCnt != ~0UL) && (itemsCnt > 0))
    {
      addListItems((struct Gadget*)gadgets[GID_PODCAST_EPISODES_LISTBROWSER], &podcastEpisodeList, podcastEpisodeColInfo);
    }
  }
  else showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
}

void playPodcast(struct Node *res_node)
{
  if (res_node)
  {
    char startPlayerPath[128];
    STRPTR cmd = NULL;
    struct podcastEpisodeInfo *itemData = NULL;
    itemData = (struct podcastEpisodeInfo *)IExec->AllocVecTags(sizeof(struct podcastEpisodeInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

    IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
          LBNA_UserData, &itemData,
          TAG_DONE);


    IUtility->SNPrintf(startPlayerPath, sizeof(startPlayerPath), "%s/scripts/start_player", getFilePath((STRPTR)"PROGDIR:MediaVault"));
    if (fileExists(startPlayerPath))
    {
      cmd = IUtility->ASPrintf("execute \"%s\" \"%s\" ", startPlayerPath, itemData->enclosureUrl);
    }
    else
    {
      cmd = IUtility->ASPrintf("Run <>NIL: APPDIR:AmigaAmp3 \"%s\" ", itemData->enclosureUrl);
    }
    
    IDOS->SystemTags( cmd,
        SYS_Input,    ZERO,
        SYS_Output,   NULL,
        SYS_Error,    ZERO,
        SYS_Asynch,   TRUE,
        TAG_DONE);

    // TODO: Free itemData
  }
}

void addFavouritePodcast(struct Node *res_node)
{
  char itemUID[32];
  char latestEpisodePublishDate[11];
  struct podcastInfo *itemData = {0};
  itemData = (struct podcastInfo *)IExec->AllocVecTags(sizeof(struct podcastInfo),
        AVT_Type,            MEMF_PRIVATE,
        AVT_ClearWithValue,  "\0",
        TAG_DONE);

  IListBrowser->GetListBrowserNodeAttrs((struct Node *)res_node,
        LBNA_UserData, &itemData,
        TAG_DONE);

  if(listCount(&podcastEpisodeList))
  {
    IUtility->SNPrintf(
        latestEpisodePublishDate,
        sizeof(latestEpisodePublishDate),
        "%lu",
        getLatestEpisodePublishDate(&podcastEpisodeList)
    );
  }

  IUtility->SNPrintf(itemUID, sizeof(itemUID), "pod_%lu", itemData->id);
  if(sqlCheckExist(itemUID, "podcast"))
  {
    sqlRemoveFavourite(itemUID, "podcast");
    toggleFavouriteButton(FALSE);
  }
  else
  {
    sqlAddFavouritePodcast(
      itemUID,
      itemData->title,
      itemData->language,
      itemData->image,
      itemData->description,
      latestEpisodePublishDate
    );
    toggleFavouriteButton(TRUE);
  }

  // TODO: Free itemData
}

static void toggleFavouriteButton(BOOL status)
{
  if (status)
  {
    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_FAVOURITE_BUTTON], windows[WID_MAIN], NULL,
          GA_Disabled,          FALSE,
          GA_HintInfo,          "Click to remove the selected\npodcast from the favourites list",
          BUTTON_RenderImage,   objects[OID_FAVOURITES_REMOVE_IMAGE],
          TAG_DONE);
  }
  else
  {
    IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_PODCAST_FAVOURITE_BUTTON], windows[WID_MAIN], NULL,
          GA_Disabled,          FALSE,
          GA_HintInfo,          "Click to add the selected\npodcast to the favourites list",
          BUTTON_RenderImage,   objects[OID_FAVOURITES_ADD_IMAGE],
          TAG_DONE);
  }
}
