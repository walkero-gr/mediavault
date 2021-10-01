/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: guifuncs.c
**
** Date: 10-04-2021 23:28:17
**
************************************************************

*/


#include <intuition/gadgetclass.h>
#include <classes/window.h>
#include <gadgets/space.h>

#include <proto/application.h>
#include <proto/listbrowser.h>

#include "globals.h"
#include "gui.h"
#include "guifuncs.h"
#include "radiofuncs.h"
#include "httpfuncs.h"
#include "stringfuncs.h"
#include "fsfuncs.h"

extern Class *BitMapClass;

struct releaseInfo {
  char tag_name[10];
  char content_type[24];
  char browser_download_url[255];
  char changes[1024];
};
static struct releaseInfo *release = NULL;

uint32 cacheFilenameSize = 128;

/**
 * The following code is from MenuClass.c as found at the SDK 53.30 examples
 *
 * Copyright of Hyperion Entertainment CVBA
 */
struct Image *MenuImage(CONST_STRPTR name, struct Screen *screen) {
   struct Image *i = NULL;
   APTR prev_win;
   BPTR dir, prev_dir;
   STRPTR name_s, name_g;
   uint32 len;

   len = IUtility->Strlen(name);

   name_s = IExec->AllocVecTags(len + 3 + len + 3,TAG_END);

   if (name_s)
   {
      name_g = name_s + len + 3;

      strcpy(name_s,name);
      strcat(name_s,"_s");

      strcpy(name_g,name);
      strcat(name_g,"_g");

      prev_win = IDOS->SetProcWindow((APTR)-1);  /* Disable requesters */

      dir = IDOS->Lock("TBIMAGES:",SHARED_LOCK);

      IDOS->SetProcWindow(prev_win);             /* Re-enable requesters */

      if (dir != ZERO)
      {
         prev_dir = IDOS->SetCurrentDir(dir);

         i = (struct Image *)IIntuition->NewObject(NULL,"bitmap.image",BITMAP_SourceFile, name,
                                                           BITMAP_SelectSourceFile, name_s,
                                                           BITMAP_DisabledSourceFile, name_g,
                                                           BITMAP_Screen, screen,
                                                           BITMAP_Masking, TRUE,
                                                           TAG_END);

         if (i)
            IIntuition->SetAttrs((Object *)i,IA_Height,i->Height + 2,TAG_END);

         IDOS->SetCurrentDir(prev_dir);

         IDOS->UnLock(dir);
      }

      IExec->FreeVec(name_s);
   }

   return (i);
}

void windowBlocking(Object *objId, BOOL disable)
{
  IIntuition->SetAttrs(objId, WA_BusyPointer, disable, TAG_DONE);
}

LONG showMsgReq(
  Object *reqGadget,
  CONST_STRPTR title,
  CONST_STRPTR message,
  ULONG type,
  CONST_STRPTR gadgetText,
  ULONG image )
{
  if(reqGadget)
  {
    IIntuition->SetAttrs(reqGadget,
      REQ_Type,       type ? type : REQTYPE_INFO,
      REQ_TitleText,  title,
      REQ_BodyText,   message,
      REQ_GadgetText, gadgetText ? gadgetText : "_Ok",
      REQ_Image,      image ? image : REQIMAGE_DEFAULT,
      REQ_StayOnTop,  FALSE,
      REQ_WrapBorder, 32,
      TAG_END);
    return IIntuition->IDoMethod(reqGadget, RM_OPENREQ, NULL, NULL, NULL);
  }
  return -1;
}

void gadgetBlocking(struct Window *winId, Object *gadgetObj, BOOL disable)
{
  if(gadgetObj)
  {
    IIntuition->SetGadgetAttrs((struct Gadget*)gadgetObj, winId, NULL,
        GA_Disabled,  disable,
        TAG_DONE);
  }
}

int listCount(struct List *list)
{
  struct Node *node;
  int cnt = 0;

  if (list->lh_Head)
  {
    for(node = list->lh_Head ;
          node->ln_Succ != NULL ;
          node = node->ln_Succ)
    {
      cnt++;
    }
  }

  return cnt;
}

BOOL appHide(uint32 appID, Object *winObj, uint32 methodID)
{
  BOOL retVal = FALSE;

  if ( IIntuition->IDoMethod(winObj, methodID, NULL) == TRUE )
  {
    IIntuition->SetAttrs(winObj,
                WA_PubScreen, NULL,
                WINDOW_Window, NULL,
                TAG_DONE);
    IApplication->SetApplicationAttrs(appID,
                  APPATTR_Hidden, TRUE,
                  TAG_DONE);
    retVal = TRUE;
  }

  return retVal;
}

struct Window *appUnhide(uint32 appID, Object *winObj)
{
  struct Screen *pubScr = NULL;
  struct Window *window = NULL;
  BOOL hidden;

  IApplication->GetApplicationAttrs(appID,
                APPATTR_Hidden, &hidden,
                TAG_DONE);

  if ( hidden == TRUE )
  {
    pubScr = IIntuition->LockPubScreen(NULL);
    if ( !pubScr ) return NULL;

    IIntuition->SetAttrs(winObj, WA_PubScreen, pubScr, TAG_DONE);

    if ( (window = (struct Window *) IIntuition->IDoMethod(winObj, WM_OPEN, NULL)) )
    {
      IIntuition->ScreenToFront(pubScr);
      IApplication->SetApplicationAttrs(appID, APPATTR_Hidden, FALSE, TAG_DONE);
    }

    IIntuition->UnlockPubScreen(NULL, pubScr);
  }
  else {
    // The GUI is not hidden = just bring it to front.
    IIntuition->GetAttr(WINDOW_Window, winObj, (uint32 *) &window);
    if (window)
    {
      IIntuition->WindowToFront(window);
      IIntuition->ScreenToFront(window->WScreen);
      IIntuition->ActivateWindow(window);
    }
  }

  return window;
}

void freeList(struct List *listBrowser, int structID)
{
  struct Node *node;
  while((node = IExec->RemHead( listBrowser )))
  {
    switch(structID)
    {
      case 0:
        {
          struct stationInfo *structData = NULL;
          IListBrowser->GetListBrowserNodeAttrs( node,
                LBNA_UserData, &structData,
                TAG_DONE);
          IExec->FreeVec(structData);
        }
        break;
      case 1:
        {
          struct podcastInfo *structData = NULL;
          IListBrowser->GetListBrowserNodeAttrs( node,
                LBNA_UserData, &structData,
                TAG_DONE);
          IExec->FreeVec(structData);
        }
        break;
      case 2:
        {
          struct podcastEpisodeInfo *structData = NULL;
          IListBrowser->GetListBrowserNodeAttrs( node,
                LBNA_UserData, &structData,
                TAG_DONE);
          IExec->FreeVec(structData);
        }
        break;
    }

    IListBrowser->FreeListBrowserNode(node);
  }
  //IExec->FreeSysObject(ASOT_LIST, listBrowser);     // TODO: Find the reason why this freeze the machine
}

static STRPTR getCachedImageIfExists(STRPTR uuid)
{
  char buf[cacheFilenameSize];
  char avatarBase[cacheFilenameSize];
  IUtility->Strlcpy(avatarBase, CACHE_DIR, sizeof(avatarBase));
  IUtility->Strlcat(avatarBase, uuid, sizeof(avatarBase));

  BPTR imgLock;
  const char ext[][6] = {".jpg", ".png", ".webp", ".ico", ".gif"};

  size_t i;
  for (i = 0; i < sizeof(ext)/sizeof(ext[0]); i++)
  {
    IUtility->Strlcpy(buf, avatarBase, sizeof(buf));
    IUtility->Strlcat(buf, ext[i], sizeof(buf));

    imgLock = IDOS->Lock(buf, SHARED_LOCK);
    if (imgLock)
    {
      STRPTR avatar = IExec->AllocVecTags(sizeof(char) * cacheFilenameSize,
          AVT_Type,            MEMF_SHARED,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);
      IDOS->UnLock(imgLock);

      strcpy(avatar, buf);
      return avatar;
    }
  }
  return NULL;
}

void showAvatarImage(STRPTR uuid, STRPTR url, Object *avatarWrapperGadget, Object *avatarImageObject, struct RenderHook *renderhook)
{
  struct Screen *screen = NULL;

  STRPTR avatarImage = getCachedImageIfExists(uuid);

  if (!avatarImage)
  {
    downloadFile(url, uuid, (STRPTR)CACHE_DIR);
    avatarImage = getCachedImageIfExists(uuid);
  }

  if (!avatarImage)
  {
    avatarImage = (STRPTR)LOGO_IMAGE_BIG;
  }

  if (avatarImage)
  {
    if((screen = IIntuition->LockPubScreen(NULL)))
    {
      // Clean previous image shown
      IIntuition->SetGadgetAttrs((struct Gadget*)avatarWrapperGadget, windows[WID_MAIN], NULL,
          GA_Image, NULL,
          TAG_END );
      IIntuition->DisposeObject(avatarImageObject);

      // Create a new object for the image
      avatarImageObject = IIntuition->NewObject(BitMapClass, NULL,
          IA_Scalable,        TRUE,
          BITMAP_Screen,      screen,
          BITMAP_SourceFile,  avatarImage,
          BITMAP_Masking,     TRUE,
          TAG_END);

      if (renderhook && avatarImageObject)
      {
        renderhook->img  = avatarImageObject;
        renderhook->w    = ((struct Image *)avatarImageObject)->Width;
        renderhook->h    = ((struct Image *)avatarImageObject)->Height;
        renderhook->fill = FALSE;

        IIntuition->SetGadgetAttrs((struct Gadget*)avatarWrapperGadget, windows[WID_MAIN], NULL,
            SPACE_RenderHook,   renderhook,
            GA_Image,           avatarImageObject,
            TAG_END );

        IIntuition->IDoMethod( objects[OID_MAIN], WM_RETHINK );
        IIntuition->RefreshWindowFrame( windows[WID_MAIN] );
      }

      IIntuition->UnlockPubScreen(NULL, screen);
      screen = NULL;
    }
  }
}

struct Region *set_clip_region (struct RastPort *rp,struct Rectangle *rect, BOOL *in_refresh)
{
  struct Region *old_region = NULL;
  struct Layer *layer = rp->Layer;

  if (layer)
  {
    struct Region *new_region = IGraphics->NewRegion();

    if (new_region)
      IGraphics->OrRectRegion (new_region,rect);

    if (layer->Flags & LAYERUPDATING)
    {
      ILayers->EndUpdate (layer,FALSE);
      *in_refresh = TRUE;
    }
    else
      *in_refresh = FALSE;

    old_region = ILayers->InstallClipRegion (layer,new_region);
  }

  return (old_region);
}

void remove_clip_region (struct RastPort *rp,struct Region *old_region, BOOL in_refresh)
{
  struct Layer *layer = rp->Layer;

  if (layer)
  {
    struct Region *new_region = ILayers->InstallClipRegion (layer,old_region);

    if (new_region)
      IGraphics->DisposeRegion (new_region);

    if (in_refresh)
      ILayers->BeginUpdate (layer);
  }
}

ULONG renderfunct(struct RenderHook *hook, Object *obj, struct gpRender *msg)
{
  Object *img = hook->img;
  struct IBox box;
  struct impDraw drawmsg;
  long w, h;
  struct Rectangle clip_rect;
  struct Region *old_region;
  BOOL in_refresh;

  if (!img) return (0);

  IIntuition->GetAttr(SPACE_RenderBox, obj, (ULONG *)&box);

  w = box.Width;
  h = hook->h * box.Width / hook->w;

  if (h > box.Height)
  {
    h = box.Height;
    w = hook->w * box.Height / hook->h;
  }

  clip_rect.MinX = box.Left;
  clip_rect.MinY = box.Top;
  clip_rect.MaxX = box.Left + box.Width - 1;
  clip_rect.MaxY = box.Top + box.Height - 1;
  old_region = set_clip_region (msg->gpr_RPort,&clip_rect,&in_refresh);

  drawmsg.MethodID              = IM_DRAWFRAME;
  drawmsg.imp_RPort             = msg->gpr_RPort;

  drawmsg.imp_Offset.X          = box.Left + (box.Width  - w) / 2;
  drawmsg.imp_Offset.Y          = box.Top  + (box.Height - h) / 2;

  drawmsg.imp_State             = IDS_NORMAL;
  drawmsg.imp_DrInfo            = msg->gpr_GInfo->gi_DrInfo;
  drawmsg.imp_Dimensions.Width  = w;
  drawmsg.imp_Dimensions.Height = h;
  IIntuition->IDoMethodA (img, (Msg)&drawmsg);

  remove_clip_region (msg->gpr_RPort,old_region,in_refresh);

  return (0);
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
    if (getGithubLatestData())
    {
      if (checkForUpdates())
      {
        IUtility->Strlcpy(infoMsg, "There is a new version available!\nWould you like to download and install the new version automatically?\n", infoMsgSize);
        IUtility->Strlcat(infoMsg, "\nChanges:\n", infoMsgSize);
      
        // TODO: Clear \r from the changes string
        IUtility->Strlcat(infoMsg, release->changes, infoMsgSize);
        requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, REQTYPE_INFO, "_Sure|_Nah", REQIMAGE_QUESTION);
        
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
            requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, REQIMAGE_ERROR);
          }

          windowBlocking(objects[OID_MAIN], FALSE);
        }
      }
      else
      {
        IUtility->Strlcpy(infoMsg, "Congratulations!\nYou have the latest version installed on your machine.", infoMsgSize);
        requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, 0);
      }
    }
    else
    {
      IUtility->Strlcpy(infoMsg, "There was an error on update process.\nPlease, check your internet connection.\n", infoMsgSize);
      requesterResult = showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (STRPTR)infoMsg, 0, NULL, REQIMAGE_ERROR);
    }
  }
  IExec->FreeVec(infoMsg);
}

 /**
  *
  * void switchRightSidebar(ULONG)
  *
  * Summary:
  *
  *     This function switches the pages at the right sidebar
  *     depending the selected section from the left sidebar
  *
  * Parameters   : page: the page number
  *
  * Return Value :
  *
  * Description:
  *
  *     This function checks the current visible page at the
  *     right sidebar, and if it needs switches to the one
  *     that reflects the choice at the left sidebar
  *
  */
void switchRightSidebar(ULONG page)
{
  ULONG currentRightSidebarPage;
  IIntuition->GetAttr(PAGE_Current, objects[OID_RIGHT_SIDEBAR_PAGES], &currentRightSidebarPage);

  if (currentRightSidebarPage != page)
  {
    IIntuition->SetAttrs(objects[OID_RIGHT_SIDEBAR_PAGES],
        PAGE_Current, page,
        TAG_END);

    IIntuition->IDoMethod( objects[OID_MAIN], WM_RETHINK );
  }
}
