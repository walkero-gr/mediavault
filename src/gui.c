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


#include "globals.h"
#include "gui.h"
#include "mainWin.h"


struct Screen 			*screen;
struct MsgPort 			*appPort;
struct Window 			*windows[WID_LAST];

Object *objects[OID_LAST];
Object *menus[MID_LAST];

//#include "guifuncs.h"
//
//extern void buildMainWindow(void);
//extern void buildMainMenu(void);
//
//static void buildAboutWindow(void);
//
//struct Image *MenuImage(CONST_STRPTR, struct Screen *);
//void windowBlocking(struct Window *, Object *, BOOL disable);
//
//extern void getRadioStations(void);

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort)
  {
		if((screen = IIntuition->LockPubScreen(NULL)))
		{
		  //drInfo = IIntuition->GetScreenDrawInfo(screen);
		  menus[MID_PROJECT] = buildMainMenu(screen);
		  objects[OID_MAIN] = buildMainWindow(appPort, menus[MID_PROJECT]);
		  //buildAboutWindow();

		  if (objects[OID_MAIN])
		  {
		  	windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);
				IIntuition->UnlockPubScreen(NULL, screen);

				if (windows[WID_MAIN])
				{
				  uint32 	signal = 0,
				  				selectedMenu = MID_LAST;
				  uint16 code = 0;
          BOOL done = FALSE;

		      IIntuition->GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);

				  while (!done)
				  {
				    uint32 wait = IExec->Wait(signal | SIGBREAKF_CTRL_C);
				    if ( wait & SIGBREAKF_CTRL_C ) 
				    {
							done = TRUE;
		          break;
		        }

				    if ( wait & signal ) 
				    {
		        	uint32 result = WMHI_LASTMSG;

							// Main Window events
		          while ((result = IIntuition->IDoMethod(objects[OID_MAIN], WM_HANDLEINPUT, &code, TAG_DONE)))
		          {
		          	switch (result & WMHI_CLASSMASK)
		          	{
		           		case WMHI_CLOSEWINDOW:
		              	windows[WID_MAIN] = NULL;
		                done = TRUE;
		                break;
		              case WMHI_ICONIFY:
		                IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY, TAG_DONE);
		                windows[WID_MAIN] = NULL;
		                break;
									case WMHI_UNICONIFY:
										windows[WID_MAIN] = (struct Window *)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);
										break;
									case WMHI_MENUPICK:
										selectedMenu = NO_MENU_ID;
                    while ((selectedMenu = IIntuition->IDoMethod(menus[MID_PROJECT], MM_NEXTSELECT, 0, selectedMenu, TAG_DONE)) != NO_MENU_ID) {
                      switch (selectedMenu) {
                        case MID_ICONIFY:
                       		IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY, TAG_DONE);
		                			windows[WID_MAIN] = NULL;
                        	break;
                        case MID_ABOUT:
													//windows[WID_ABOUT] = (struct Window*)IIntuition->IDoMethod(objects[OID_ABOUT], WM_OPEN, TAG_DONE);
													//windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                        	break;
                        case MID_QUIT:
                        	done = TRUE;
                        	break;
                      }  
                    }

									  break;
									//case WMHI_GADGETUP:
										//switch(result & WMHI_GADGETMASK) {
											//case GID_FILTER_BUTTON:
											  //IDOS->Printf("Search radio stations\n");
											  //getRadioStations();
											  //break;
										//}
										//break;
								}
							}

							//// About Window events
							//while ((result = IIntuition->IDoMethod(objects[OID_ABOUT], WM_HANDLEINPUT, &code, TAG_DONE))) {
								//switch(result & WMHI_CLASSMASK) {
									//case WMHI_CLOSEWINDOW:
										//IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
										//windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
										//break;
									//case WMHI_GADGETUP:
										//switch(result & WMHI_GADGETMASK) {
											//case GID_ABOUT_BUTTON_OK:
												//IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
												//windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
												//break;		
										//}
										//break;
								//}
							//} 
				    }
				  }

					//IIntuition->DisposeObject(objects[OID_ABOUT]);
	  			IIntuition->DisposeObject(objects[OID_MAIN]);
	  			IIntuition->DisposeObject(menus[MID_PROJECT]);
				}
			}
			//IIntuition->FreeScreenDrawInfo(screen, drInfo);
		}
  }
  IExec->FreeSysObject(ASOT_PORT, appPort);
}

//static void buildAboutWindow(void) {
  /*
  CONST_STRPTR aboutText = VSTRING "\n" \
  	"Copyright (c) 2021 George Sokianos\n\n"
  	"MediaVault is a media frontend for different sources.\n\n" \
		"My plan for MediaVault is to create an app where people can discover and listen online " \
		"radio stations, podcasts and even to support UPnP servers to listen music or watch movies.\n\n" \
		"Created by George Sokianos\n" \
		"Contact email: walkero@gmail.com\n" \
		"Report bugs at https://github.com/walkero-gr/mediavault/issues/\n\n"
		"Dedicated to my beloved family, who support me all these years\nAris, Nefeli and Marily!\n\n" \
		"Distributed without warranty under the terms of the GNU General Public License.";
  */
	//objects[OID_ABOUT] = IIntuition->NewObject(NULL, "window.class",
		//WA_ScreenTitle, 				VSTRING,
		//WA_Title, 							"About " APPNAME,
		//WA_Activate, 						TRUE,
		//WA_DepthGadget, 				TRUE,
		//WA_DragBar, 						TRUE,
		//WA_CloseGadget, 				TRUE,
		//WA_SizeGadget, 					FALSE,
		//WA_Opaqueness,         	255,    /* Initial opaqueness on opening (0..255) */
    //WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    //WA_FadeTime,            250000, /* Duration of transition in microseconds */
		//WINDOW_Iconifiable, 		TRUE,
		//WINDOW_AppPort,					appPort,
		//WINDOW_SharedPort, 			appPort,
		//WINDOW_Position, 				WPOS_CENTERSCREEN,
		//WINDOW_Layout, IIntuition->NewObject(NULL, "layout.gadget",
			//GA_ID, 								GID_ABOUT_LAYOUT_ROOT,
			//LAYOUT_Orientation, 	LAYOUT_ORIENT_VERT,
			//LAYOUT_SpaceOuter, 		TRUE,
			//LAYOUT_DeferLayout, 	TRUE,
               //
			//LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
				//GA_ID, 									GID_ABOUT_LAYOUT_TEXT,
				//LAYOUT_Orientation, 		LAYOUT_ORIENT_HORIZ,
				//LAYOUT_VertAlignment, 	LALIGN_CENTER,
				//LAYOUT_HorizAlignment,	LALIGN_CENTER,
				//LAYOUT_BevelStyle, 			BVS_NONE,
				//LAYOUT_BevelState, 			IDS_SELECTED,
				//LAYOUT_BackFill, 				LAYERS_NOBACKFILL,
				//
				//LAYOUT_AddImage, gadgets[GID_ABOUT_LOGO] = IIntuition->NewObject(NULL, "bitmap.image",
					//GA_ID,							GID_ABOUT_LOGO,
					//IA_Scalable, FALSE,
					//BITMAP_SourceFile, "PROGDIR:images/logo_128.png",
					//BITMAP_Screen, screen,
					////BITMAP_Precision, PRECISION_EXACT,
					//BITMAP_Masking, TRUE,
					////BITMAP_Width, 50,
					////BITMAP_Height, 50,
				//TAG_END),
				//CHILD_WeightedWidth, 30,     
				//CHILD_MaxHeight, 128,	
//
				//
				//LAYOUT_AddChild, gadgets[GID_ABOUT_TEXT] = IIntuition->NewObject(NULL, "texteditor.gadget",
					//GA_ID,										GID_ABOUT_TEXT,
					//GA_RelVerify, 						TRUE,
					//GA_TEXTEDITOR_Contents, 	aboutText,
					//GA_TEXTEDITOR_CursorX, 		0,
					//GA_TEXTEDITOR_CursorY, 		0,
					//GA_TEXTEDITOR_Flow, 			0,
				//TAG_DONE),
				//LAYOUT_AddChild, gadgets[GID_ABOUT_TEXT_SCROLLER] = IIntuition->NewObject(NULL, "scroller.gadget",
					//GA_ID, 									GID_ABOUT_TEXT_SCROLLER,
					//SCROLLER_Orientation, 	SORIENT_VERT,
					//SCROLLER_Arrows, 				FALSE,
					//ICA_TARGET, 						objects[GID_ABOUT_TEXT],
					//ICA_MAP, 								scrollerToText,
				//TAG_DONE),
			//TAG_DONE),
			//CHILD_MinWidth, 	520,	  
			//CHILD_MinHeight,  200,	 
			//
			//LAYOUT_AddChild, IIntuition->NewObject(NULL, "button.gadget",
				//GA_ID, 							GID_ABOUT_BUTTON_OK,
				//GA_RelVerify, 			TRUE,
				//GA_Text, 						"_Okay",
				//BUTTON_AutoButton, 	0,
			//TAG_DONE),                
			//CHILD_MaxHeight, 40,
	  //TAG_DONE), 
	//TAG_DONE);
	//
	//IIntuition->SetAttrs(gadgets[GID_ABOUT_TEXT], 
		//ICA_TARGET, 	gadgets[GID_ABOUT_TEXT_SCROLLER],
		//ICA_MAP, 			textToScroller,
		//TAG_END); 
//
//}

/**
 * The following code is from MenuClass.c as found at the SDK 53.30 examples
 *
 * Copyright of Hyperion Entertainment CVBA
 */
//struct Image *MenuImage(CONST_STRPTR name, struct Screen *screen) {
   //struct Image *i = NULL;
   //APTR prev_win;
   //BPTR dir, prev_dir;
   //STRPTR name_s, name_g;
   //uint32 len;
//
   //len = strlen(name);
//
   //name_s = IExec->AllocVecTags(len + 3 + len + 3,TAG_END);
//
   //if (name_s)
   //{
      //name_g = name_s + len + 3;
//
      //strcpy(name_s,name);
      //strcat(name_s,"_s");
//
      //strcpy(name_g,name);
      //strcat(name_g,"_g");
//
      //prev_win = IDOS->SetProcWindow((APTR)-1);  /* Disable requesters */
//
      //dir = IDOS->Lock("TBIMAGES:",SHARED_LOCK);
//
      //IDOS->SetProcWindow(prev_win);             /* Re-enable requesters */
//
      //if (dir != ZERO)
      //{
         //prev_dir = IDOS->SetCurrentDir(dir);
//
         //i = (struct Image *)IIntuition->NewObject(NULL,"bitmap.image",BITMAP_SourceFile, name,
                                                           //BITMAP_SelectSourceFile, name_s,
                                                           //BITMAP_DisabledSourceFile, name_g,
                                                           //BITMAP_Screen, screen,
                                                           //BITMAP_Masking, TRUE,
                                                           //TAG_END);
//
         //if (i)
            //IIntuition->SetAttrs((Object *)i,IA_Height,i->Height + 2,TAG_END);
//
         //IDOS->SetCurrentDir(prev_dir);
//
         //IDOS->UnLock(dir);
      //}
//
      //IExec->FreeVec(name_s);
   //}
//
   //return (i);
//}
/*
void windowBlocking(struct Window *winId, Object *objId, BOOL disable) {
  IIntuition->SetWindowPointer(winId, WA_BusyPointer, disable, TAG_DONE);
	IIntuition->SetAttrs(objId, WA_BusyPointer, disable, TAG_DONE);
}
*/


