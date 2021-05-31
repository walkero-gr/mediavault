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

//#include <iconv.h>
#include <proto/listbrowser.h>

#include "globals.h"
#include "gui.h"
#include "libshandler.h"
#include "httpfuncs.h"
#include "stringfuncs.h"

uint8 maxRadioResults = 20;

static CONST_STRPTR radioAPIUrl = "https://de1.api.radio-browser.info/json";  
static char url[255];

static void setBaseSearchUrl(void)
{                   
  char maxRadioResultsStr[4];

  IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/stations/search?hidebroken=true&limit=", sizeof(url));
  IUtility->SNPrintf(maxRadioResultsStr, sizeof(maxRadioResultsStr), "%ld", maxRadioResults);
  IUtility->Strlcat(url, maxRadioResultsStr, sizeof(url));
}

STRPTR getRadioStations(struct filters lastFilters, int offset)
{
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

  return getResponseBody(url, NET_PORT_HTTPS);
}

STRPTR getRadioTrendStations(void)
{
  setBaseSearchUrl();
  IUtility->Strlcat(url, "&order=clicktrend", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));   

  return getResponseBody(url, NET_PORT_HTTPS);
}

STRPTR getRadioPopularStations(void)
{
  setBaseSearchUrl();
  IUtility->Strlcat(url, "&order=clickcount", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));  

  return getResponseBody(url, NET_PORT_HTTPS);
}

size_t getRadioList(struct List *stationList, STRPTR jsonData, int offset)
{                          
  json_t *jsonRoot;
  json_error_t jsonError;
  size_t cnt;
  //iconv_t cd = (iconv_t) -1;
  //char charsetName[128];

  jsonRoot = IJansson->json_loads(jsonData, 0, &jsonError);
  if(!jsonRoot)
  {
    //IDOS->Printf("json error: on line %d: %s\n", jsonError.line, jsonError.text);
    return ~0UL;
  }

  if (!json_is_array(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    //IDOS->Printf("JSON error: jsonRoot is not an array");
    return ~0UL;
  }                                                                         

  ULONG votesNum;
  //IDOS->GetVar("Charset", charsetName, sizeof(charsetName), GVF_GLOBAL_ONLY);
  //cd = iconv_open(charsetName, "UTF-8");

  if (offset == 0)
  {
    IExec->NewList(stationList);
  }

  for(cnt = 0; cnt < IJansson->json_array_size(jsonRoot); cnt++)
  {
    struct Node *stationNode;
    //struct stationInfo stationData;
    json_t *data, *stationuuid, *name, *country, *tags, *url_resolved, *votes;

    data = IJansson->json_array_get(jsonRoot, cnt);
    if(!json_is_object(data))
    {
      IDOS->Printf("error: commit data %d is not an object\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }

    stationuuid = IJansson->json_object_get(data, "stationuuid");
    if(!json_is_string(stationuuid))
    {
      IDOS->Printf("error: commit %d: stationuuid is not a string\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("stationuuid: %s\n", IJansson->json_string_value(stationuuid));

    name = IJansson->json_object_get(data, "name");
    if(!json_is_string(name))
    {
      IDOS->Printf("error: commit %d: name is not a string\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("Station name: %s\n", IJansson->json_string_value(name));
    //IUtility->Strlcpy(stationData.name, IJansson->json_string_value(name), sizeof(stationData.name));
    //IDOS->Printf("Station name: %s\n", stationData.name);

    country = IJansson->json_object_get(data, "country");
    if(!json_is_string(country))
    {
      IDOS->Printf("error: commit %d: country is not a string\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("Station country: %s\n", IJansson->json_string_value(country));

    tags = IJansson->json_object_get(data, "tags");
    if(!json_is_string(tags))
    {
      IDOS->Printf("error: commit %d: tags is not a string\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("Station tags: %s\n", IJansson->json_string_value(tags));

    url_resolved = IJansson->json_object_get(data, "url_resolved");
    if(!json_is_string(url_resolved))
    {
      IDOS->Printf("error: commit %d: url_resolved is not a string\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("Station url_resolved: %s\n", IJansson->json_string_value(url_resolved));

    votes = IJansson->json_object_get(data, "votes");
    if(!json_is_integer(votes))
    {
      IDOS->Printf("error: commit %d: votes is not an integer\n", (int)(cnt + 1));
      IJansson->json_decref(jsonRoot);
      return ~0UL;
    }
    //IDOS->Printf("votes: %ld\n", (ULONG)IJansson->json_integer_value(votes));
    votesNum = (ULONG)IJansson->json_integer_value(votes);

    stationNode = IListBrowser->AllocListBrowserNode( 4,
        //LBNA_UserData, stationData,
        LBNA_Column, 0,
          LBNCA_CopyText, TRUE,
          //LBNCA_Text, IJansson->json_string_value(name),
          //LBNCA_Text, useIconv(cd, IJansson->json_string_value(name)),
          LBNCA_Text,     charConv(IJansson->json_string_value(name)),
        LBNA_Column, 1,
          LBNCA_CopyText, TRUE,
          LBNCA_Text, IJansson->json_string_value(country),
        LBNA_Column, 2,
          LBNCA_CopyInteger, TRUE,
          LBNCA_Integer, &votesNum,
          LBNCA_Justification, LCJ_RIGHT,
        LBNA_Column, 3,
          LBNCA_CopyText, TRUE,
          LBNCA_Text, IJansson->json_string_value(url_resolved),
        TAG_DONE);

    if(stationNode)
    {
      IExec->AddTail(stationList, stationNode);
    }
  }

  //if (cd != (iconv_t)-1) iconv_close(cd);

  IJansson->json_decref(jsonRoot);
  return cnt;
}

void playRadio(struct Node *res_node)
{
  STRPTR selListValue;
  
  IListBrowser->GetListBrowserNodeAttrs(res_node,
    LBNA_Column,  3,
    LBNCA_Text,   &selListValue,
    TAG_DONE);

  STRPTR cmd = IUtility->ASPrintf("Run <>NIL: APPDIR:AmigaAmp3 \"%s\" ", selListValue);
  IDOS->SystemTags( cmd,
      SYS_Input,    ZERO,
      SYS_Output,   NULL,
      SYS_Error,    ZERO,
      SYS_Asynch,   TRUE,
      TAG_DONE);
}

void showRadioInfo(struct Node *res_node)
{
  STRPTR  selListValue;
  
  IListBrowser->GetListBrowserNodeAttrs(res_node,
      LBNA_Column,  0,
      LBNCA_Text,   &selListValue,
      TAG_DONE);
  IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_LBL_INFO_NAME], windows[WID_MAIN], NULL,
    GA_Text,   selListValue,
    TAG_DONE);
}
