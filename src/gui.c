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
#include "guifuncs.h"
#include "mainWin.h"
#include "aboutWin.h"
#include "oofuncs.h"


struct Screen 			*screen;
struct MsgPort 			*appPort;
struct Window 			*windows[WID_LAST];

Object *gadgets[GID_LAST];
Object *objects[OID_LAST];
Object *menus[MID_LAST];

void showGUI(void)
{
  appPort = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, NULL);
  if (appPort)
  {
		if((screen = IIntuition->LockPubScreen(NULL)))
		{
		  //drInfo = IIntuition->GetScreenDrawInfo(screen);
		  menus[MID_PROJECT] 	= buildMainMenu(screen);
		  objects[OID_MAIN] 	= buildMainWindow(appPort, menus[MID_PROJECT]);
		  objects[OID_ABOUT] 	= buildAboutWindow(appPort, screen);

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
                    while ((selectedMenu = IIntuition->IDoMethod(menus[MID_PROJECT], MM_NEXTSELECT, 0, selectedMenu, TAG_DONE)) != NO_MENU_ID) 
                    {
                      switch (selectedMenu) 
                      {
                        case MID_ICONIFY:
                       		IIntuition->IDoMethod(objects[OID_MAIN], WM_ICONIFY, TAG_DONE);
		                			windows[WID_MAIN] = NULL;
                        	break;
                        case MID_ABOUT:
													windows[WID_ABOUT] = (struct Window*)IIntuition->IDoMethod(objects[OID_ABOUT], WM_OPEN, TAG_DONE);
													windowBlocking(windows[WID_MAIN], objects[OID_MAIN], TRUE);
                        	break;
                        case MID_QUIT:
                        	done = TRUE;
                        	break;
                      }  
                    }

									  break;
									case WMHI_GADGETUP:
										switch(result & WMHI_GADGETMASK) 
										{
											case GID_FILTER_BUTTON:
											  IDOS->Printf("Search radio stations\n");
											  getRadioStations();
											  break;
										}
										break;
								}
							}

							//// About Window events
							while ((result = IIntuition->IDoMethod(objects[OID_ABOUT], WM_HANDLEINPUT, &code, TAG_DONE))) 
							{
								switch(result & WMHI_CLASSMASK) 
								{
									case WMHI_CLOSEWINDOW:
										IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
										windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
										break;
									case WMHI_GADGETUP:
										switch(result & WMHI_GADGETMASK) 
										{
											case GID_ABOUT_BUTTON_OK:
												IIntuition->IDoMethod(objects[OID_ABOUT], WM_CLOSE, TAG_DONE);
												windowBlocking(windows[WID_MAIN], objects[OID_MAIN], FALSE);
												break;		
										}
										break;
								}
							} 
				    }
				  }

					IIntuition->DisposeObject(objects[OID_ABOUT]);
	  			IIntuition->DisposeObject(objects[OID_MAIN]);
	  			IIntuition->DisposeObject(menus[MID_PROJECT]);
				}
			}
			//IIntuition->FreeScreenDrawInfo(screen, drInfo);
		}
  }
  IExec->FreeSysObject(ASOT_PORT, appPort);
}
