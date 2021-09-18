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
#include "stringfuncs.h"
#include "guifuncs.h"
#include "secrets.h"

static CONST_STRPTR podcastAPIUrl = "https://api.podcastindex.org/api/1.0";

extern struct memory response;

BOOL getPodcasts(struct filters lastFilters, int offset)
{
  BOOL success = FALSE;
  char url[255];
  STRPTR buf = IExec->AllocVecTags(sizeof(char) * 90,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  IUtility->Strlcpy(url, podcastAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/search/byterm?pretty=", sizeof(url));

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

  char authString[71];
  char nowString[11];
  ULONG nowSecondsUTC = nowUnix(now()) + getOffsetUTC();

  IUtility->SNPrintf(nowString, sizeof(nowString), "%lu", nowSecondsUTC);
  
  IUtility->Strlcpy(authString, PODCASTINDEX_KEY, sizeof(authString));
  IUtility->Strlcat(authString, PODCASTINDEX_SECRET, sizeof(authString));
  IUtility->Strlcat(authString, nowString, sizeof(authString));

  IUtility->Strlcpy(buf, "X-Auth-Key: " PODCASTINDEX_KEY, sizeof(char) * 35);
  addRequestHeader(buf);
  
  IUtility->Strlcpy(buf, "X-Auth-Date: ", sizeof(char) * 35);
  IUtility->Strlcat(buf, nowString, sizeof(char) * 35);
  addRequestHeader(buf);
  
  IUtility->Strlcpy(buf, "Authorization: ", sizeof(char) * 90);
  IUtility->Strlcat(buf, SHA1Encrypt(authString), sizeof(char) * 90);
  addRequestHeader(buf);

  doHTTPRequest(url);
  if (getResponseCode() == 200)
    success = TRUE;

  //IDOS->Printf("DBG: url: %s\n", url);
  //IDOS->Printf("\n\n%s\n", getResponseBody());

  IExec->FreeVec(buf);
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
      IUtility->Strlcpy(itemData->language, charConv(IJansson->json_string_value(buf)), sizeof(itemData->language));

      buf = IJansson->json_object_get(data, "url");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->url, IJansson->json_string_value(buf), sizeof(itemData->url));

      buf = IJansson->json_object_get(data, "originalUrl");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->originalUrl, IJansson->json_string_value(buf), sizeof(itemData->originalUrl));

      buf = IJansson->json_object_get(data, "image");
      if(!json_is_string(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      IUtility->Strlcpy(itemData->image, IJansson->json_string_value(buf), sizeof(itemData->image));

      /*
      buf = IJansson->json_object_get(data, "votes");
      if(!json_is_integer(buf))
      {
        IJansson->json_decref(jsonRoot);
        return ~0UL;
      }
      itemData->votes = (ULONG)IJansson->json_integer_value(buf);
      */
      itemNode = IListBrowser->AllocListBrowserNode( 3,
          LBNA_UserData,          itemData,
          LBNA_Column,            0,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->title,
          LBNA_Column,            1,
            LBNCA_CopyText,       TRUE,
            LBNCA_Text,           itemData->language,
          //LBNA_Column,            2,
          //  LBNCA_CopyInteger,    TRUE,
          //  LBNCA_Integer,        &itemData->votes,
          //  LBNCA_Justification,  LCJ_RIGHT,
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

void freePodcastInfo(struct podcastInfo *itemData)
{
  if(itemData)
  {
    IExec->FreeVec(itemData);
  }
}
