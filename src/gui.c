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
#include "radiofuncs.h"

static struct ColumnInfo *columnInfo;
struct List radioList;
int32 radioListItemsCnt = 0;

// TODO: Set below to a struct
static char 	selName[32] = "",
          		selGenre[32] = "",
          		selCountry[32] = "",
          		selLanguage[32] = "";

static void fillRadioList(void);

extern NETWORKOBJ *net;

//struct Screen 			*screen;
//struct MsgPort 			*appPort;
//struct Window 			*windows[WID_LAST];

//Object *gadgets[GID_LAST];
//Object *objects[OID_LAST];
//Object *menus[MID_LAST];

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
          
          columnInfo = IListBrowser->AllocLBColumnInfo(2,
        			LBCIA_Column, 			0,
	            	LBCIA_Title, 			" Station",
	            	LBCIA_AutoSort, 	TRUE,
	            	LBCIA_SortArrow, 	TRUE,
	            	LBCIA_Width,			0,
        			LBCIA_Column, 			1,
	            	LBCIA_Title, 			" Country",
	            	LBCIA_AutoSort, 	TRUE,
	            	LBCIA_SortArrow, 	TRUE,
	            	LBCIA_Width,			0,
	        		TAG_DONE);

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
										switch (result & WMHI_GADGETMASK)
										{
											case GID_FILTER_BUTTON:
											  IUtility->Strlcpy(selName, ((struct StringInfo *)(((struct Gadget *)gadgets[GID_FILTERS_NAME])->SpecialInfo))->Buffer, sizeof(selGenre));											  
											  fillRadioList();
											  break;
											case GID_CHOOSER_GENRES:
											  if (code > 0) 
											  {
											  	IUtility->Strlcpy(selGenre, getChooserText(GID_CHOOSER_GENRES, code), sizeof(selGenre));
											  }
											  else IUtility->Strlcpy(selGenre, "", sizeof(selGenre));
											  break;
											case GID_CHOOSER_COUNTRIES:
											  if (code > 0) 
											  {
											  	IUtility->Strlcpy(selCountry, getChooserText(GID_CHOOSER_COUNTRIES, code), sizeof(selCountry));
											  }
											  else IUtility->Strlcpy(selCountry, "", sizeof(selCountry));
											  break;
											case GID_CHOOSER_LANGUAGES:
											  if (code > 0) 
											  {
											  	IUtility->Strlcpy(selLanguage, getChooserText(GID_CHOOSER_LANGUAGES, code), sizeof(selLanguage));
											  }
											  else IUtility->Strlcpy(selLanguage, "", sizeof(selLanguage));
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
				  
				  IListBrowser->FreeLBColumnInfo(columnInfo);
				  if(radioListItemsCnt)
				  {
				  	IListBrowser->FreeListBrowserList(&radioList);
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

static void fillRadioList(void)
{
	STRPTR responseJSON = getRadioStations(selName, selGenre, selLanguage, selCountry);
	if (responseJSON)
	{
	  IDOS->Printf("Response JSON is valid\n");
		getRadioList(responseJSON);
		
		// Dispose net here, after the creation of the listbrowser content,
		// because it trashes the response data, so to free the signals
		// TODO: communicate with the oo.library dev to find a more elegant way to do it
		if (net)
		{
		  net->DisposeConnection();
		  IOO->DisposeNetworkObject(net);
		}
		
	} else IDOS->Printf("Response JSON is NOT valid\n");
	
	if (radioListItemsCnt)
	{
		IIntuition->SetGadgetAttrs((struct Gadget*)gadgets[GID_RADIO_LISTBROWSER], windows[WID_MAIN], NULL,
				LISTBROWSER_Labels, 				(ULONG)&radioList,
				LISTBROWSER_SortColumn,			0,
				LISTBROWSER_Selected,				-1,
				LISTBROWSER_ColumnInfo,     columnInfo,
				TAG_DONE);
	}
}  
