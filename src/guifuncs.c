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

#include <classes/window.h>
#include <images/bitmap.h>

#include <proto/application.h>

#include "globals.h"
#include "guifuncs.h"

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