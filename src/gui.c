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

static void buildMainWindow(void);
static void buildAboutWindow(void);
static void buildMainMenu(void);
static struct Image *MenuImage(CONST_STRPTR, struct Screen *);

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort) {
		
		if((screen = IIntuition->LockPubScreen(NULL))) {
		  buildMainMenu();
		  buildMainWindow();
		  buildAboutWindow();
		  
		  if (objects[OID_MAIN]) {
		  	windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, TAG_DONE);
				IIntuition->UnlockPubScreen(NULL, screen);
			
				if (windows[WID_MAIN]) {
				  //Object *obj;
				  uint32 	signal = 0,
				  				selectedMenu = MID_LAST;
				  uint16 code = 0;
          BOOL done = FALSE;
          
		      IIntuition->GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);
		 
				  while (!done) {
				    uint32 wait = IExec->Wait(signal | SIGBREAKF_CTRL_C);
				    if ( wait & SIGBREAKF_CTRL_C ) {
							done = TRUE;
		          break;
		        }
				    
				    if ( wait & signal ) {
		        	uint32 result = WMHI_LASTMSG;
		
		          while ((result = IIntuition->IDoMethod(objects[OID_MAIN], WM_HANDLEINPUT, &code, TAG_DONE)) != WMHI_LASTMSG) {
		          	switch (result & WMHI_CLASSMASK) {
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
													windows[WID_ABOUT] = (struct Window*)IIntuition->IDoMethod(objects[OID_ABOUT], WM_OPEN, TAG_DONE);
													IIntuition->SetWindowPointer(windows[WID_MAIN], WA_BusyPointer, TRUE, TAG_DONE);
													IIntuition->SetAttrs(objects[OID_MAIN], WA_BusyPointer, TRUE, TAG_DONE);
                        	break;
                        case MID_QUIT:
                        	done = TRUE;
                        	break;
                      }  
                    }

									  break;
								}
							}
							
							while ((result = IIntuition->IDoMethod(objects[OID_ABOUT], WM_HANDLEINPUT, &code, TAG_DONE))) {
								switch(result & WMHI_CLASSMASK) {
									case WMHI_CLOSEWINDOW:
										IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
										IIntuition->SetAttrs(objects[OID_MAIN], WA_BusyPointer, FALSE, TAG_DONE);
										IIntuition->SetWindowPointer(windows[WID_MAIN], WA_BusyPointer, FALSE, TAG_DONE);
										break;
									case WMHI_GADGETUP:
										switch(result & WMHI_GADGETMASK) {
											case GID_ABOUT_BUTTON_OK:
												IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
												IIntuition->SetAttrs(objects[OID_MAIN], WA_BusyPointer, FALSE, TAG_DONE);
												IIntuition->SetWindowPointer(windows[WID_MAIN], WA_BusyPointer, FALSE, TAG_DONE);
												break;		
										}
										break;
								}
							} 
				    }
				  }
				}
			}
		}
		
		IIntuition->DisposeObject(objects[OID_ABOUT]);
	  IIntuition->DisposeObject(objects[OID_MAIN]);
	  IIntuition->DisposeObject(menus[MID_PROJECT]);
  }
  IExec->FreeSysObject(ASOT_PORT, appPort);
}


static void buildMainWindow(void) {
	objects[OID_MAIN] = IIntuition->NewObject(NULL, "window.class",
		WA_ScreenTitle, 				screenTitle,
		WA_Title, 							windowTitle,
		WA_Activate, 						TRUE,
		WA_CloseGadget, 				TRUE,
		WA_DepthGadget, 				TRUE,
		WA_DragBar, 						TRUE,
		WA_SizeGadget, 					TRUE,
		WA_Opaqueness,          255,    /* Initial opaqueness on opening (0..255) */
    WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    WA_FadeTime,            500000, /* Duration of transition in microseconds */
    WA_NewLookMenus,				TRUE,
		WINDOW_Iconifiable, 		TRUE,
		WINDOW_IconifyGadget,  	TRUE,
		WINDOW_MenuStrip, 			menus[MID_PROJECT],
		WINDOW_AppPort, 				appPort,
		WINDOW_Position, 				WPOS_CENTERSCREEN,
		WINDOW_Layout, IIntuition->NewObject(NULL, "layout.gadget",
			LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_DeferLayout, TRUE,
				LAYOUT_AddChild,   gadgets[GID_FILTERS_LAYOUT] = IIntuition->NewObject(NULL, "layout.gadget",
					LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,      BVS_GROUP,
					LAYOUT_Label,           "Filter stations by",
					
					LAYOUT_AddChild,   gadgets[GID_FILTERS_LAYOUT_LINE1] = IIntuition->NewObject(NULL, "layout.gadget",
						LAYOUT_AddChild, IIntuition->NewObject(NULL, "string.gadget",
								GA_ID, 								GID_FILTERS_NAME,
								GA_RelVerify, 				TRUE,
								GA_TabCycle, 					TRUE,
								STRINGA_MinVisible, 	10,
								STRINGA_MaxChars, 		64,
								TAG_DONE),
								CHILD_Label, IIntuition->NewObject(NULL, "label.image",
									LABEL_Text, 	"_Name",
									LABEL_Key, 		"n",
									TAG_DONE),	
								
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
		          	GA_ID,            		GID_CHOOSER_GENRES,
		          	GA_RelVerify,        	TRUE,
		          	CHOOSER_LabelArray,  	genres,
		          	CHOOSER_Selected,    	0,
		          	GA_Underscore,		 		0,
								TAG_DONE),
		      			CHILD_NominalSize, TRUE,
								CHILD_Label, IIntuition->NewObject(NULL, "label.image",
									LABEL_Text, 	"_Genre",
									LABEL_Key, 		"g",
									TAG_DONE),		
						TAG_DONE),
						
						LAYOUT_AddChild,   gadgets[GID_FILTERS_LAYOUT_LINE2] = IIntuition->NewObject(NULL, "layout.gadget",
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
		          	GA_ID,            		GID_CHOOSER_COUNTRIES,
		          	GA_RelVerify,        	TRUE,
		          	CHOOSER_LabelArray,  	countries,
		          	CHOOSER_Selected,    	0,
		          	GA_Underscore,		 		0,
								TAG_DONE),
		      			CHILD_NominalSize, TRUE,
								CHILD_Label, IIntuition->NewObject(NULL, "label.image",
									LABEL_Text, 	"_Country",
									LABEL_Key, 		"c",
									TAG_DONE),
									
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
		          	GA_ID,            		GID_CHOOSER_LANGUAGES,
		          	GA_RelVerify,        	TRUE,
		          	CHOOSER_LabelArray,  	languages,
		          	CHOOSER_Selected,    	0,
		          	GA_Underscore,		 		0,
								TAG_DONE),
		      			CHILD_NominalSize, TRUE,
								CHILD_Label, IIntuition->NewObject(NULL, "label.image",
									LABEL_Text, 	"_Language",
									LABEL_Key, 		"l",
									TAG_DONE),		
						TAG_DONE),
							
					LAYOUT_AddChild, IIntuition->NewObject(NULL, "button.gadget",
						GA_Text, "Filter Stations",
						TAG_DONE),
				TAG_DONE),
			TAG_DONE),
		TAG_DONE);
}  

static void buildAboutWindow(void) {
  CONST_STRPTR aboutText = VSTRING \
  	"Copyright (c) 2021 George Sokianos\n\n"
  	"MediaVault is a media frontend for different sources.\n\n" \
		"My plan for MediaVault is to create an app where people can find and listen on online " \
		"radio stations, podcasts and even to support UPnP servers to listen music or watch movies.\n\n" \
		"Created by George Sokianos\n" \
		"Contact email: walkero@gmail.com\n" \
		"Report bugs at https://github.com/walkero-gr/mediavault/issues/\n\n"
		"Dedicated to my beloved family, who support me all these years\nAris, Nefeli and Marily!\n\n" \
		"Distributed without warranty under the terms of the GNU General Public License.";
  
	objects[OID_ABOUT] = IIntuition->NewObject(NULL, "window.class",
		WA_ScreenTitle, 		"About",
		WA_Title, 					"About",
		WA_Activate, 				TRUE,
		WA_DepthGadget, 		TRUE,
		WA_DragBar, 				TRUE,
		WA_CloseGadget, 		TRUE,
		WA_SizeGadget, 			FALSE,
		WINDOW_Iconifiable, TRUE,
		WINDOW_AppPort,			appPort,
		WINDOW_SharedPort, 	appPort,
		WINDOW_Position, 		WPOS_CENTERSCREEN,
		WINDOW_Layout, gadgets[GID_ABOUT_LAYOUT_ROOT] = IIntuition->NewObject(NULL, "layout.gadget",
			LAYOUT_Orientation, 	LAYOUT_ORIENT_VERT,
			LAYOUT_SpaceOuter, 		TRUE,
			LAYOUT_DeferLayout, 	TRUE,
               
			LAYOUT_AddChild, gadgets[GID_ABOUT_LAYOUT_TEXT] = IIntuition->NewObject(NULL, "layout.gadget",
				LAYOUT_Orientation, 		LAYOUT_ORIENT_HORIZ,
				LAYOUT_VertAlignment, 	LALIGN_CENTER,
				LAYOUT_HorizAlignment,	LALIGN_CENTER,
				LAYOUT_BevelStyle, 			BVS_NONE,
				LAYOUT_BevelState, 			IDS_SELECTED,
				LAYOUT_BackFill, 				LAYERS_NOBACKFILL,
				LAYOUT_AddChild, gadgets[GID_ABOUT_TEXT] = IIntuition->NewObject(NULL, "texteditor.gadget",
					GA_RelVerify, 						TRUE,
					GA_TEXTEDITOR_Contents, 	aboutText,
					GA_TEXTEDITOR_CursorX, 		0,
					GA_TEXTEDITOR_CursorY, 		0,
					GA_TEXTEDITOR_Flow, 			0,
				TAG_DONE),
				LAYOUT_AddChild, gadgets[GID_ABOUT_TEXT_SCROLLER] = IIntuition->NewObject(NULL, "scroller.gadget",
					GA_ID, 									GID_ABOUT_TEXT_SCROLLER,
					SCROLLER_Orientation, 	SORIENT_VERT,
					SCROLLER_Arrows, 				FALSE,
					ICA_TARGET, 						objects[GID_ABOUT_TEXT],
					ICA_MAP, 								scrollerToText,
				TAG_DONE),
			TAG_DONE),
			CHILD_MinWidth, 	520,	  
			CHILD_MinHeight, 	300,	 
			
			LAYOUT_AddChild, gadgets[GID_ABOUT_BUTTON_OK] = IIntuition->NewObject(NULL, "button.gadget",
				GA_ID, 							GID_ABOUT_BUTTON_OK,
				GA_RelVerify, 			TRUE,
				GA_Text, 						"_Okay",
				BUTTON_AutoButton, 	0,
			TAG_DONE),                
			CHILD_MaxHeight, 40,
	  TAG_DONE), 
	TAG_DONE);
}

static void buildMainMenu(void) {
  menus[MID_PROJECT] = IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ROOT,
		MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_MENU,
			MA_Label, "Project",
			MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
				MA_ID, MID_ABOUT,
				MA_Label, "About...",
				MA_Key,		"?",
				MA_Image, MenuImage("info",screen),
				TAG_END),
			MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
				MA_ID, MID_ICONIFY,
				MA_Label, "Iconify",
				MA_Key,		"I",
				MA_Image, MenuImage("iconify", screen),
				TAG_END),
			MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
				MA_Label, ML_SEPARATOR,
				TAG_END),
			MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
				MA_ID, MID_QUIT,
				MA_Label, "Quit",
				MA_Key,		"Q",
				MA_Image, MenuImage("quit",screen),
				TAG_END),
			TAG_END),
		TAG_END);
}

/**
 * The following code is from MenuClass.c as found at the SDK 53.30 examples
 *
 * Copyright of Hyperion Entertainment CVBA
 */
static struct Image *MenuImage(CONST_STRPTR name, struct Screen *screen) {
   struct Image *i = NULL;
   APTR prev_win;
   BPTR dir, prev_dir;
   STRPTR name_s, name_g;
   uint32 len;

   len = strlen(name);

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
  