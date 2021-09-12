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

/*
https://api.podcastindex.org/api/1.0/search/byterm
  ?q=batman+university&pretty
User-Agent: mediavault/1.3
X-Auth-Date: 1631373575
X-Auth-Key: CWN8YHEYKJDRPRE6PQSH
Authorization: caa664769943563462b64ef7f0c0184e3722c400
*/

static CONST_STRPTR podcastAPIUrl = "https://api.podcastindex.org/api/1.0";

extern struct memory response;

BOOL getPodcasts(struct filters lastFilters, int offset)
{
  BOOL success = FALSE;
  char url[255];

  IUtility->Strlcpy(url, podcastAPIUrl, sizeof(url));
  IUtility->Strlcat(url, "/search/byterm?", sizeof(url));

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
    IUtility->Strlcat(url, "q=", sizeof(url));
    IUtility->Strlcat(url, encSelName, sizeof(url));
    IExec->FreeVec(encSelName);
  }

  char authString[71];
  IUtility->Strlcpy(authString, PODCASTINDEX_KEY, sizeof(authString));
  IUtility->Strlcat(authString, PODCASTINDEX_SECRET, sizeof(authString));
  IUtility->Strlcat(authString, now(), sizeof(authString));

  

  IDOS->Printf("DBG: url:%s\nX-Auth-Date: %s\nX-Auth-Key: %s\nAuthorization: %s\n",
    url, now(), PODCASTINDEX_KEY, SHA1Encrypt(authString));

  //doHTTPRequest(url);
  //if (getResponseCode() == 200)
  //  success = TRUE;

  return success;
}

