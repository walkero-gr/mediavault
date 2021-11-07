/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: updates.c
**
** Date: 17-04-2021 10:56:59
**
************************************************************

*/

#include "globals.h"
#include "gui.h"
#include "httpfuncs.h"
#include "stringfuncs.h"
#include "fsfuncs.h"
#include "guifuncs.h"
#include "updates.h"

struct releaseInfo {
  char tag_name[10];
  char content_type[24];
  char browser_download_url[255];
  char changes[2048];
};
static struct releaseInfo *release = {0};

static BOOL checkForUpdates(void)
{
  ULONG currentVersion = 0;
  currentVersion = currentVersion + VERSION * 100;
  currentVersion = currentVersion + REVISION * 10;
  currentVersion = currentVersion + PATCH;

  if (release->tag_name)
  {
    ULONG latestRelease = convertVersionToInt(release->tag_name);

    if (currentVersion < latestRelease)
    {
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL getGithubLatestData(void)
{
  BOOL success = FALSE;
  release = (struct releaseInfo *)IExec->AllocVecTags(sizeof(struct releaseInfo),
          AVT_Type,            MEMF_PRIVATE,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);

  char url[] = "https://api.github.com/repos/walkero-gr/mediavault/releases/latest";

  doHTTPRequest(url);

  if (getResponseCode() != 200)
    return FALSE;

  STRPTR responseBody = getResponseBody();
  if (responseBody == NULL)
    return FALSE;

  if (responseBody)
  {
    json_t *jsonRoot;
    json_error_t jsonError;

    jsonRoot = IJansson->json_loads(responseBody, 0, &jsonError);
    cleanupHTTPRequest();

    if (jsonRoot)
    {
      if (json_is_object(jsonRoot))
      {
        json_t *buf, *assets, *assetItem;
        size_t cnt;

        buf = IJansson->json_object_get(jsonRoot, "tag_name");
        if (json_is_string(buf))
        {
          IUtility->Strlcpy(release->tag_name, IJansson->json_string_value(buf), sizeof(release->tag_name));
        }

        buf = IJansson->json_object_get(jsonRoot, "body");
        if (json_is_string(buf))
        {
          IUtility->Strlcpy(release->changes, IJansson->json_string_value(buf), sizeof(release->changes));
          strReplace("\r", "", release->changes);
        }

        assets = IJansson->json_object_get(jsonRoot, "assets");
        if (json_is_array(assets))
        {
          for(cnt = 0; cnt < IJansson->json_array_size(assets); cnt++)
          {
            assetItem = IJansson->json_array_get(assets, cnt);
            if(json_is_object(assetItem))
            {
              buf = IJansson->json_object_get(assetItem, "browser_download_url");
              if (json_is_string(buf))
              {
                IUtility->Strlcpy(release->browser_download_url, IJansson->json_string_value(buf), sizeof(release->browser_download_url));
              }
            }
          }
        }

        success = TRUE;
      }
      IJansson->json_decref(jsonRoot);
    }
  }

  return success;
}

void startUpdateProcess(BOOL muted)
{
  uint32 infoMsgSize = sizeof(char) * 2048;
  STRPTR infoMsg = IExec->AllocVecTags(infoMsgSize,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  if (getGithubLatestData())
  {
    if (checkForUpdates())
    {
      IUtility->Strlcpy(infoMsg, "There is a new version available!\nWould you like to download and install the new version automatically?\n", infoMsgSize);
      IUtility->Strlcat(infoMsg, "\nChanges:\n", infoMsgSize);

      IUtility->Strlcat(infoMsg, release->changes, infoMsgSize);
      LONG requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, REQTYPE_INFO, "_Sure|_Nah", REQIMAGE_QUESTION);

      if (requesterResult)
      {
        windowBlocking(objects[OID_MAIN], TRUE);

        // download the update archive
        if (downloadLatestUpdate((STRPTR)release->browser_download_url))
        {
          int32 unarcResult = unarcFile((STRPTR)"RAM:mediavault_update.lha", getParentPath((STRPTR)"PROGDIR:MediaVault"));
          if (unarcResult >= 0)
          {
            IUtility->Strlcpy(infoMsg, "MediaVault was updated to the latest version.\nPlease, close this window and restart it.\n", infoMsgSize);
            requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, 0);
          }
          else
          {
            IUtility->Strlcpy(infoMsg, "There was an error with the update.\nYou will find in RAM the update.lha file,\nwhich you can use to update MediaVault manually.\n", infoMsgSize);
            requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, REQIMAGE_ERROR);
          }
        }
        else
        {
          IUtility->Strlcpy(infoMsg, "There was an error with the download of the update.\nPlease, check your internet connection.\n", infoMsgSize);
          showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, REQIMAGE_ERROR);
        }

        windowBlocking(objects[OID_MAIN], FALSE);
      }
    }
    else
    {
      if (!muted)
      {
        IUtility->Strlcpy(infoMsg, "\33bCongratulations!\n\33nYou have the latest version installed on your machine.", infoMsgSize);
        showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, 0);
      }
    }
  }
  else
  {
    if (!muted)
    {
      IUtility->Strlcpy(infoMsg, "There was an error on update process.\nPlease, check your internet connection.\n", infoMsgSize);
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, REQIMAGE_ERROR);
    }
  }
  IExec->FreeVec(infoMsg);
}

void workOnUpdate(void)
{
  uint32 infoMsgSize = sizeof(char) * 2048;
  STRPTR infoMsg = IExec->AllocVecTags(infoMsgSize,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  IUtility->Strlcpy(infoMsg, "Would you like to check if there is a\nnewer version of MediaVault available?", infoMsgSize);
  LONG requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, REQTYPE_INFO, "_Yes|_No", REQIMAGE_QUESTION);

  if (requesterResult)
  {
    startUpdateProcess(FALSE);
  }
  IExec->FreeVec(infoMsg);
}
