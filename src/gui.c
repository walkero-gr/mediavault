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

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort) {
		screen = IIntuition->LockPubScreen(NULL);
		if(screen) {
		  buildMainWindow();
		  if (objects[OID_MAIN]) {
		  	windows[WID_MAIN] = (struct Window*)IIntuition->IDoMethod(objects[OID_MAIN], WM_OPEN, NULL);
				IIntuition->UnlockPubScreen(NULL, screen);
			
				if (windows[WID_MAIN]) {
				  struct Menu *menuStrip;
					struct MenuItem *item;

				  uint32 signal = 0;
				  uint16 code = 0,
                 selection;
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
									case WMHI_MENUPICK:
										menuStrip = windows[WID_MAIN]->MenuStrip;
										selection = code;
										uint16 menuNum, itemNum;
										
										while ((selection != MENUNULL) && (done == FALSE)) {

                      item = IIntuition->ItemAddress(menuStrip, selection);
                        
                      menuNum = MENUNUM(selection);
											itemNum = ITEMNUM(selection); 

									    switch (menuNum)
									    {
									      case PROJECT_MENU:
							            switch (itemNum)
							            {
							                case 0: 
							                	printf("About\n");
							                  break;
							                case 1:
							                  printf("Iconify\n");
							                  break;
							                case 3:
							                  done = TRUE;
							                  break;
							            }
							            break;
									    }
                      selection = item->NextSelect;
                    }
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
		WINDOW_NewMenu,					mainMenu,  
		WINDOW_AppPort, 				appPort,
		WINDOW_Position, 				WPOS_CENTERSCREEN,
		WINDOW_Layout, IIntuition->NewObject(NULL, "layout.gadget",
			LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_DeferLayout, TRUE,
				LAYOUT_AddChild,   objects[GID_FILTERS_LAYOUT] = IIntuition->NewObject(NULL, "layout.gadget",
					LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,      BVS_GROUP,
					LAYOUT_Label,           "Filter stations by",
					
					LAYOUT_AddChild,   objects[GID_FILTERS_LAYOUT_LINE1] = IIntuition->NewObject(NULL, "layout.gadget",
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
						
						LAYOUT_AddChild,   objects[GID_FILTERS_LAYOUT_LINE2] = IIntuition->NewObject(NULL, "layout.gadget",
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
  