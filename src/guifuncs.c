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

extern Class *BitMapClass;

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

void showMsgReq(Object *reqGadget, CONST_STRPTR title, CONST_STRPTR message)
{
  if(reqGadget)
  {
    IIntuition->SetAttrs(reqGadget,
      REQ_TitleText,  title,
      REQ_BodyText,   message,
      TAG_END);

    IIntuition->IDoMethod(reqGadget, RM_OPENREQ, NULL, NULL, NULL);
  }
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

void FreeList(
  struct List *listBrowser,
  void (*freeUserDataCallback)(struct stationInfo *)
)
{
  struct Node *node;

  while((node = IExec->RemHead( listBrowser )))
  {
    struct stationInfo *stationData = NULL;

    IListBrowser->GetListBrowserNodeAttrs( node,
          LBNA_UserData, &stationData,
          TAG_DONE);

    freeUserDataCallback(stationData);

    IListBrowser->FreeListBrowserNode(node);
  }

  IExec->FreeSysObject(ASOT_LIST, listBrowser);
}

static STRPTR getCachedImageIfExists(STRPTR uuid)
{
  char buf[128];
  char avatarBase[128];
  IUtility->Strlcpy(avatarBase, CACHE_DIR, sizeof(avatarBase));
  IUtility->Strlcat(avatarBase, uuid, sizeof(avatarBase));

  //char avatar[128];
  IDOS->Printf("getCachedImageIfExists: %s\n",  uuid);
  BPTR imgLock;        
  const char ext[][6] = {".jpg", ".png", ".webp", ".ico", ".gif"};
  
  size_t i;
  for (i = 0; i < sizeof(ext)/sizeof(ext[0]); i++)
  {
    IDOS->Printf("EXT: %s\n",  ext[i]);
    
    IUtility->Strlcpy(buf, avatarBase, sizeof(buf));
    IUtility->Strlcat(buf, ext[i], sizeof(buf));
    IDOS->Printf("Lock: %s\n", buf);
    imgLock = IDOS->Lock(buf, SHARED_LOCK);
    if (imgLock)
    {
      STRPTR avatar = IExec->AllocVecTags(sizeof(char) * 128,
          AVT_Type,            MEMF_SHARED,
          AVT_ClearWithValue,  "\0",
          TAG_DONE);
      IDOS->UnLock(imgLock);
      //IUtility->Strlcpy(avatar, buf, sizeof(avatar));
      strcpy(avatar, buf);
      //return avatar;
      IDOS->Printf("Avatar found: %s\nbuf: %s\n", avatar, buf);
      return avatar;
      //break;
    }
  }
  IDOS->Printf("Cached avatar not found\n");
  return NULL;
}

void showAvatarImage(STRPTR uuid, STRPTR url)
{
  //Object *avatarImageObj = NULL;
  struct Screen *screen = NULL;
  
  //STRPTR avatarImage = NULL;
  IDOS->Printf("showAvatarImage\n");
  //## TODO: First show MediaVault avatar in right sidebar

  //## TODO: Check first if the avatar exists in cache
  //IUtility->Strlcpy(avatarImage, getCachedImageIfExists(uuid), sizeof(avatarImage));
  STRPTR avatarImage = getCachedImageIfExists(uuid);
  IDOS->Printf("Avatar url: %s\n", url);

  if (!avatarImage)
  {
    cacheFileFromUrl(url, getPortByURL(url), uuid);
    avatarImage = getCachedImageIfExists(uuid);
  }

  if (!avatarImage)
  {
    avatarImage = (STRPTR)LOGO_IMAGE;
  }
  IDOS->Printf("Avatar found in cache: %s\n", avatarImage);


  //if (screen) IDOS->Printf("Screen is set\n");

  //## TODO: Show this image at the right sidebar
  /*
  */
  if (avatarImage)
  {
    if((screen = IIntuition->LockPubScreen(NULL)))
    {


      /*
      IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_AVATAR], windows[WID_MAIN], NULL,
          GA_Image, NULL,
          TAG_END );

      if (objects[OID_AVATAR_IMAGE]) IDOS->Printf("OID_AVATAR_IMAGE is set 1\n");
      IIntuition->DisposeObject(objects[OID_AVATAR_IMAGE]);
      */


      objects[OID_AVATAR_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
          IA_Scalable,        FALSE,
          BITMAP_Screen,      screen,
          BITMAP_SourceFile,  avatarImage,
          BITMAP_Masking,     TRUE,
          TAG_END);

      if (objects[OID_AVATAR_IMAGE]) IDOS->Printf("OID_AVATAR_IMAGE is set 2\n");

      struct RenderHook *renderhook = (struct RenderHook *) IExec->AllocSysObjectTags(ASOT_HOOK,
            ASOHOOK_Size,  sizeof(struct RenderHook),
            ASOHOOK_Entry, (HOOKFUNC)renderfunct,
            TAG_END);

      if (renderhook && objects[OID_AVATAR_IMAGE])
      {
        renderhook->img  = objects[OID_AVATAR_IMAGE];
        renderhook->w    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Width;
        renderhook->h    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Height;
        renderhook->fill = FALSE;
        IDOS->Printf("W: %ld, H: %ld \n", renderhook->w, renderhook->h);

        IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_AVATAR], windows[WID_MAIN], NULL,
            SPACE_RenderHook,   renderhook,
            GA_Image,           objects[OID_AVATAR_IMAGE],
            TAG_END );

        IIntuition->IDoMethod( objects[OID_MAIN], WM_RETHINK );
        IIntuition->RefreshWindowFrame( windows[WID_MAIN] );
      }





      /*
      IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_AVATAR], windows[WID_MAIN], NULL,
          BUTTON_RenderImage, NULL,
        TAG_END );
      IIntuition->DisposeObject(objects[OID_AVATAR_IMAGE]);

      avatarImageObj = IIntuition->NewObject(BitMapClass, NULL,
          GA_ID,              OID_AVATAR_IMAGE,
          IA_Scalable,        TRUE,
          BITMAP_Screen,      screen,
          BITMAP_SourceFile,  avatarImage,
          BITMAP_Masking,     TRUE,
          TAG_END);

      if (objects[OID_AVATAR_IMAGE]) IDOS->Printf("OID_AVATAR_IMAGE is set 2\n");


      IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_INFO_AVATAR], windows[WID_MAIN], NULL,
          BUTTON_RenderImage, avatarImageObj,
          TAG_END );
      */
      IIntuition->UnlockPubScreen(NULL, screen);
      screen = NULL;
    }
  }
  IDOS->Printf("================================\n");
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
  long w,h;
  struct Rectangle clip_rect;
  struct Region *old_region;
  BOOL in_refresh;

  if (!img) return (0);

  IIntuition->GetAttr (SPACE_RenderBox, obj, (ULONG *)&box);

  // TODO: Put some more thought on resizing
  w = box.Width;
  h = hook->h * box.Width / hook->w;

  //IDOS->Printf("BOX: %ld x %d\n", box.Width, box.Height);
  //IDOS->Printf("HOOK: %ld x %ld\n", hook->w, hook->h);
  //IDOS->Printf("CALC: %ld x %ld\n", w, h);

  //h = box.Height;
  //w = hook->w * box.Height / hook->h;
  /*
  if (hook->fill)
    {
    if (h < box.Height)
      {
      h = box.Height;
      w = hook->w * box.Height / hook->h;
      }
    }
  else
    {
    if (h > box.Height)
      {
      h = box.Height;
      w = hook->w * box.Height / hook->h;
      }
    }
  */
  clip_rect.MinX = box.Left;
  clip_rect.MinY = box.Top;
  clip_rect.MaxX = box.Left + box.Width - 1;
  clip_rect.MaxY = box.Top + box.Height - 1;
  old_region = set_clip_region (msg->gpr_RPort,&clip_rect,&in_refresh);

  drawmsg.MethodID              = IM_DRAWFRAME;
  drawmsg.imp_RPort             = msg->gpr_RPort;

  drawmsg.imp_Offset.X          = box.Left + (box.Width  - w) / 2;
  drawmsg.imp_Offset.Y          = box.Top  + (box.Height - h) / 2;
  //drawmsg.imp_Offset.X          = box.Left;
  //drawmsg.imp_Offset.Y          = box.Top - topMarginScroll;
  
  drawmsg.imp_State             = IDS_NORMAL;
  drawmsg.imp_DrInfo            = msg->gpr_GInfo->gi_DrInfo;
  drawmsg.imp_Dimensions.Width  = w;
  drawmsg.imp_Dimensions.Height = h;
  IIntuition->IDoMethodA (img, (Msg)&drawmsg);

  remove_clip_region (msg->gpr_RPort,old_region,in_refresh);

  return (0);
}
