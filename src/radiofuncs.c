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

#include "globals.h"
#include "libshandler.h"
#include "httpfuncs.h"

static CONST_STRPTR radioAPIUrl = "https://de1.api.radio-browser.info/json";
static int maxResults = 20;

extern struct List radioList, radioTrendList, radioPopularList;
extern int32 radioListItemsCnt, radioTrendListItemsCnt, radioPopularListItemsCnt;

STRPTR getRadioStations(struct filters lastFilters, int offset)
{
  char url[255];
  char maxResultsStr[4];

  IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/stations/search?hidebroken=true&limit=", sizeof(url));
  IUtility->SNPrintf(maxResultsStr, sizeof(maxResultsStr), "%ld", maxResults);
  IUtility->Strlcat(url, maxResultsStr, sizeof(url));

  if (offset > 0)
  {
    char offsetStr[3];
    IUtility->Strlcat(url, "&offset=", sizeof(url));
    IUtility->SNPrintf(offsetStr, sizeof(offsetStr), "%ld", offset * maxResults);
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

  //IDOS->Printf("%s\n", url);

  return getResponseBody(url, NET_PORT_HTTPS);
}

STRPTR getRadioTrendStations(void)
{
  char url[255];
  char maxResultsStr[4];

  IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/stations/search?hidebroken=true&limit=", sizeof(url));
  IUtility->SNPrintf(maxResultsStr, sizeof(maxResultsStr), "%ld", maxResults);
  IUtility->Strlcat(url, maxResultsStr, sizeof(url));
  IUtility->Strlcat(url, "&order=clicktrend", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));

  //IDOS->Printf("%s\n", url);

  return getResponseBody(url, NET_PORT_HTTPS);
}

STRPTR getRadioPopularStations(void)
{
  char url[255];
  char maxResultsStr[4];

  IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/stations/search?hidebroken=true&limit=", sizeof(url));
  IUtility->SNPrintf(maxResultsStr, sizeof(maxResultsStr), "%ld", maxResults);
  IUtility->Strlcat(url, maxResultsStr, sizeof(url));
  IUtility->Strlcat(url, "&order=clickcount", sizeof(url));
  IUtility->Strlcat(url, "&reverse=true", sizeof(url));

  //IDOS->Printf("%s\n", url);

  return getResponseBody(url, NET_PORT_HTTPS);
}

// TODO: Split methods to smaller reusable ones
void getRadioList(STRPTR jsonData, int offset)
{                          
  json_t *jsonRoot;
  json_error_t jsonError;
  size_t i;

  jsonRoot = IJansson->json_loads(jsonData, 0, &jsonError);

  if(!jsonRoot)
  {
    //IDOS->Printf("json error: on line %d: %s\n", jsonError.line, jsonError.text);
    CleanExit("JSON Error");
  }

  if (!json_is_array(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    CleanExit("JSON error: jsonRoot is not an array");
  }                                                                         
  ULONG votesNum;

  if (offset == 0)
  {
    IExec->NewList(&radioList);
  }
  radioListItemsCnt = 0;
  
  for(i = 0; i < IJansson->json_array_size(jsonRoot); i++)
  {
    struct Node *stationNode;
    json_t *data, *stationuuid, *name, *country, *tags, *url_resolved, *votes;

    data = IJansson->json_array_get(jsonRoot, i);
    if(!json_is_object(data))
    {
      IDOS->Printf("error: commit data %d is not an object\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }

    stationuuid = IJansson->json_object_get(data, "stationuuid");
    if(!json_is_string(stationuuid))
    {
      IDOS->Printf("error: commit %d: stationuuid is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("stationuuid: %s\n", IJansson->json_string_value(stationuuid));

    name = IJansson->json_object_get(data, "name");
    if(!json_is_string(name))
    {
      IDOS->Printf("error: commit %d: name is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station name: %s\n", IJansson->json_string_value(name));

    country = IJansson->json_object_get(data, "country");
    if(!json_is_string(country))
    {
      IDOS->Printf("error: commit %d: country is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station country: %s\n", IJansson->json_string_value(country));

    tags = IJansson->json_object_get(data, "tags");
    if(!json_is_string(tags))
    {
      IDOS->Printf("error: commit %d: tags is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station tags: %s\n", IJansson->json_string_value(tags));

    url_resolved = IJansson->json_object_get(data, "url_resolved");
    if(!json_is_string(url_resolved))
    {
      IDOS->Printf("error: commit %d: url_resolved is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station url_resolved: %s\n", IJansson->json_string_value(url_resolved));
    

    votes = IJansson->json_object_get(data, "votes");
    if(!json_is_integer(votes))
    {
      IDOS->Printf("error: commit %d: votes is not an integer\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("votes: %ld\n", (ULONG)IJansson->json_integer_value(votes));
    votesNum = (ULONG)IJansson->json_integer_value(votes);

    stationNode = IListBrowser->AllocListBrowserNode( 4,
        LBNA_Column, 0,
          LBNCA_CopyText, TRUE,
          LBNCA_Text, IJansson->json_string_value(name),
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
      radioListItemsCnt++;
      IExec->AddTail(&radioList, stationNode);
    }
  }

  IJansson->json_decref(jsonRoot);
}

// TODO: Split methods to smaller reusable ones
void getRadioTrendList(STRPTR jsonData, int offset)
{
  json_t *jsonRoot;
  json_error_t jsonError;
  size_t i;

  jsonRoot = IJansson->json_loads(jsonData, 0, &jsonError);

  if(!jsonRoot)
  {
    IDOS->Printf("json error: on line %d: %s\n", jsonError.line, jsonError.text);
    CleanExit("JSON Error");
  }

  if (!json_is_array(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    CleanExit("JSON error: jsonRoot is not an array");
  }
  ULONG votesNum;

  if (offset == 0)
  {
    IExec->NewList(&radioTrendList);
  }
  radioTrendListItemsCnt = 0;

  for(i = 0; i < IJansson->json_array_size(jsonRoot); i++)
  {
    struct Node *stationNode;
    json_t *data, *stationuuid, *name, *country, *tags, *url_resolved, *votes;

    data = IJansson->json_array_get(jsonRoot, i);
    if(!json_is_object(data))
    {
      IDOS->Printf("error: commit data %d is not an object\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }

    stationuuid = IJansson->json_object_get(data, "stationuuid");
    if(!json_is_string(stationuuid))
    {
      IDOS->Printf("error: commit %d: stationuuid is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("stationuuid: %s\n", IJansson->json_string_value(stationuuid));

    name = IJansson->json_object_get(data, "name");
    if(!json_is_string(name))
    {
      IDOS->Printf("error: commit %d: name is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station name: %s\n", IJansson->json_string_value(name));

    country = IJansson->json_object_get(data, "country");
    if(!json_is_string(country))
    {
      IDOS->Printf("error: commit %d: country is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station country: %s\n", IJansson->json_string_value(country));

    tags = IJansson->json_object_get(data, "tags");
    if(!json_is_string(tags))
    {
      IDOS->Printf("error: commit %d: tags is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station tags: %s\n", IJansson->json_string_value(tags));

    url_resolved = IJansson->json_object_get(data, "url_resolved");
    if(!json_is_string(url_resolved))
    {
      IDOS->Printf("error: commit %d: url_resolved is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station url_resolved: %s\n", IJansson->json_string_value(url_resolved));

    votes = IJansson->json_object_get(data, "votes");
    if(!json_is_integer(votes))
    {
      IDOS->Printf("error: commit %d: votes is not an integer\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("votes: %ld\n", (ULONG)IJansson->json_integer_value(votes));
    votesNum = (ULONG)IJansson->json_integer_value(votes);

    stationNode = IListBrowser->AllocListBrowserNode( 4,
        LBNA_Column, 0,
          LBNCA_CopyText, TRUE,
          LBNCA_Text, IJansson->json_string_value(name),
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
      radioTrendListItemsCnt++;
      IExec->AddTail(&radioTrendList, stationNode);
    }
  }

  IJansson->json_decref(jsonRoot);
}

// TODO: Split methods to smaller reusable ones
void getRadioPopularList(STRPTR jsonData, int offset)
{
  json_t *jsonRoot;
  json_error_t jsonError;
  size_t i;

  jsonRoot = IJansson->json_loads(jsonData, 0, &jsonError);

  if(!jsonRoot)
  {
    IDOS->Printf("json error: on line %d: %s\n", jsonError.line, jsonError.text);
    CleanExit("JSON Error");
  }

  if (!json_is_array(jsonRoot))
  {
    IJansson->json_decref(jsonRoot);
    CleanExit("JSON error: jsonRoot is not an array");
  }
  ULONG votesNum;

  if (offset == 0)
  {
    IExec->NewList(&radioPopularList);
  }
  radioPopularListItemsCnt = 0;

  for(i = 0; i < IJansson->json_array_size(jsonRoot); i++)
  {
    struct Node *stationNode;
    json_t *data, *stationuuid, *name, *country, *tags, *url_resolved, *votes;

    data = IJansson->json_array_get(jsonRoot, i);
    if(!json_is_object(data))
    {
      IDOS->Printf("error: commit data %d is not an object\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }

    stationuuid = IJansson->json_object_get(data, "stationuuid");
    if(!json_is_string(stationuuid))
    {
      IDOS->Printf("error: commit %d: stationuuid is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("stationuuid: %s\n", IJansson->json_string_value(stationuuid));

    name = IJansson->json_object_get(data, "name");
    if(!json_is_string(name))
    {
      IDOS->Printf("error: commit %d: name is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station name: %s\n", IJansson->json_string_value(name));

    country = IJansson->json_object_get(data, "country");
    if(!json_is_string(country))
    {
      IDOS->Printf("error: commit %d: country is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station country: %s\n", IJansson->json_string_value(country));

    tags = IJansson->json_object_get(data, "tags");
    if(!json_is_string(tags))
    {
      IDOS->Printf("error: commit %d: tags is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station tags: %s\n", IJansson->json_string_value(tags));

    url_resolved = IJansson->json_object_get(data, "url_resolved");
    if(!json_is_string(url_resolved))
    {
      IDOS->Printf("error: commit %d: url_resolved is not a string\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("Station url_resolved: %s\n", IJansson->json_string_value(url_resolved));

    votes = IJansson->json_object_get(data, "votes");
    if(!json_is_integer(votes))
    {
      IDOS->Printf("error: commit %d: votes is not an integer\n", (int)(i + 1));
      IJansson->json_decref(jsonRoot);
      CleanExit("JSON Error");
    }
    //IDOS->Printf("votes: %ld\n", (ULONG)IJansson->json_integer_value(votes));
    votesNum = (ULONG)IJansson->json_integer_value(votes);

    stationNode = IListBrowser->AllocListBrowserNode( 4,
        LBNA_Column, 0,
          LBNCA_CopyText, TRUE,
          LBNCA_Text, IJansson->json_string_value(name),
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
      radioPopularListItemsCnt++;
      IExec->AddTail(&radioPopularList, stationNode);
    }
  }

  IJansson->json_decref(jsonRoot);
}

