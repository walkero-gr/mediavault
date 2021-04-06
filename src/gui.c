/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: gui.c
**
** Date: 06-04-2021 16:38:55
**
************************************************************

*/

#include "gui.h"

static Object *rootWindow(void);

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort) {
	screen = IIntuition->LockPubScreen(NULL);
		if(screen) {
		  objMain = rootWindow();
		  if (objMain) {
		  	windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objMain, WM_OPEN, NULL);
				IIntuition->UnlockPubScreen(NULL, screen);
			
				if (windows[WID_MAIN]) {
				  uint32 signal = 0;
		      IIntuition->GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);
		
				  BOOL done = FALSE;
				  while (!done) {
				    uint32 wait = IExec->Wait(signal | SIGBREAKF_CTRL_C);
				    if ( wait & SIGBREAKF_CTRL_C ) {
							done = TRUE;
		          break;
		        }
				    
				    if ( wait & signal ) {
		        	uint32 result = WMHI_LASTMSG;
		          int16 code = 0;
		
		          while ((result = IIntuition->IDoMethod(objects[OID_MAIN], WM_HANDLEINPUT, &code)) != WMHI_LASTMSG) {
		          	switch (result & WMHI_CLASSMASK) {
		           		case WMHI_CLOSEWINDOW:
		              	windows[WID_MAIN] = NULL;
		                done = TRUE;
		                break;
		              case WMHI_ICONIFY:
		                IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY);
		                windows[WID_MAIN] = NULL;
		                break;
									case WMHI_UNICONIFY:
										windows[WID_MAIN] = (struct Window *)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN);
										break;
								}
							}
				    }
				  }
				}
			}
		}
	  IIntuition->DisposeObject(objects[OID_MAIN]);
  }
  IExec->FreeSysObject(ASOT_PORT, appPort);
}


static Object *rootWindow(void) {
	objects[OID_MAIN] = IIntuition->NewObject(NULL, "window.class",
		WA_ScreenTitle, "MediaVault",
		WA_Title, "MediaVault",
		WA_Activate, TRUE,
		WA_DepthGadget, TRUE,
		WA_DragBar, TRUE,
		WA_CloseGadget, TRUE,
		WA_SizeGadget, TRUE,

		// If on the Workbench screen, this tells window.class that
		// we can handle the iconify/uniconify when changing screens.
		WINDOW_Iconifiable, TRUE,

		WINDOW_AppPort, appPort,
		WINDOW_Position, WPOS_CENTERMOUSE,
		WINDOW_Layout, IIntuition->NewObject(NULL, "layout.gadget",
			LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_DeferLayout, TRUE,
			LAYOUT_AddChild, IIntuition->NewObject(NULL, "button.gadget",
				GA_Text, "Click Me",
				TAG_DONE),
			TAG_DONE),
		TAG_DONE);
		
		//if (objects[OID_LAYOUT_ROOT]) {
		  return objects[OID_MAIN];
		//}
}  
  