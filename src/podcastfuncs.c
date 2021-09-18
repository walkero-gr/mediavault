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

  IDOS->Printf("DBG: url: %s\n", url);
  IDOS->Printf("\n\n%s\n", getResponseBody());

  IExec->FreeVec(buf);
  return success;
}

