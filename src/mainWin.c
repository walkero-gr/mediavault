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
#include "guifuncs.h"

CONST_STRPTR genres[] =
{
  "All Genres",
  "Blues",
  "Classic Rock",
  "Heavy Metal",
  "Rock",
  NULL
};

CONST_STRPTR languages[] =
{
  "All Languages",
  "English",
  "Greek",
  "Irish",
  NULL
};

CONST_STRPTR countries[] =
{
  "All Countries",
  "Britain",
  "Germany",
  "Greece",
  "Ireland",
  "Sweden",
  "The United States Of America",
  NULL
};

Object *buildMainWindow(struct MsgPort *appPort, Object *winMenu)
{
  struct ColumnInfo 	*columninfo = NULL;
	//struct DrawInfo *drInfo = IIntuition->GetScreenDrawInfo(screen);

  columninfo = IListBrowser->AllocLBColumnInfo(2,
    LBCIA_Column, 0,
        LBCIA_Title, " GUI Attributes",
        LBCIA_Width, 80,
    LBCIA_Column, 1,
        LBCIA_Title, " BOOL",
        LBCIA_Width, 20,
    TAG_DONE);

	return IIntuition->NewObject(NULL, "window.class",
		WA_ScreenTitle, 				VSTRING,
		WA_Title, 							APPNAME,
		WA_Activate, 						TRUE,
		WA_CloseGadget, 				TRUE,
		WA_DepthGadget, 				TRUE,
		WA_DragBar, 						TRUE,
		WA_SizeGadget, 					TRUE,
		WA_Opaqueness,          255,    /* Initial opaqueness on opening (0..255) */
    WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    WA_FadeTime,            250000, /* Duration of transition in microseconds */
    WA_NewLookMenus,				TRUE,
		WINDOW_Iconifiable, 		TRUE,
		WINDOW_IconifyGadget,  	TRUE,
		WINDOW_Icon, 			      IIcon->GetDiskObject("PROGDIR:MediaVault"),
		WINDOW_MenuStrip, 			winMenu,
		WINDOW_AppPort, 				appPort,
		WINDOW_Position, 				WPOS_CENTERSCREEN,
		WINDOW_Layout, IIntuition->NewObject(NULL, "layout.gadget",
			LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_DeferLayout, TRUE,
				LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
					LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,

					// START - Top Filter Section
					LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
						LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
						LAYOUT_BevelStyle,      BVS_GROUP,
						LAYOUT_Label,           "Filter stations by",

						// Top filter section with the Name Text Box with Label
						LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
							LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
							LAYOUT_AddImage, IIntuition->NewObject(NULL, "label.image",
								//LABEL_DrawInfo, drInfo,
								LABEL_Text, "_Name",
								TAG_END),
							LAYOUT_AddChild, gadgets[GID_FILTERS_NAME] = IIntuition->NewObject(NULL, "string.gadget",
								GA_ID, 								GID_FILTERS_NAME,
								GA_RelVerify, 				TRUE,
								GA_TabCycle, 					TRUE,
								GA_ActivateKey,				"n",
								STRINGA_MinVisible, 	10,
								STRINGA_MaxChars, 		31,
								TAG_DONE),
							TAG_DONE),

						// Top filter section with the select boxes
						LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
							// Genres Select Box with Label
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
								LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
								LAYOUT_AddImage, IIntuition->NewObject(NULL, "label.image",
									//LABEL_DrawInfo, drInfo,
									LABEL_Text, "_Genre",
									TAG_END),
								LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
			          	GA_ID,            		GID_CHOOSER_GENRES,
			          	GA_RelVerify,        	TRUE,
									GA_TabCycle, 					TRUE,
			          	GA_Underscore,		 		0,
									GA_ActivateKey,				"g",
			          	CHOOSER_LabelArray,  	genres,
			          	CHOOSER_Selected,    	0,
									TAG_DONE),
								TAG_DONE),

							// Countries Select Box with Label
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
								LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
								LAYOUT_AddImage, IIntuition->NewObject(NULL, "label.image",
									//LABEL_DrawInfo, drInfo,
									LABEL_Text, "_Country",
									TAG_END),
								LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
			          	GA_ID,            		GID_CHOOSER_COUNTRIES,
			          	GA_RelVerify,        	TRUE,
									GA_TabCycle, 					TRUE,
			          	GA_Underscore,		 		0,
									GA_ActivateKey,				"c",
			          	CHOOSER_LabelArray,  	countries,
			          	CHOOSER_Selected,    	0,
									TAG_DONE),
								TAG_DONE),

							// Languages Select Box with Label
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
								LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
								LAYOUT_AddImage, IIntuition->NewObject(NULL, "label.image",
									//LABEL_DrawInfo, drInfo,
									LABEL_Text, "_Language",
									TAG_END),
								LAYOUT_AddChild, IIntuition->NewObject(NULL, "chooser.gadget",
			          	GA_ID,            		GID_CHOOSER_LANGUAGES,
			          	GA_RelVerify,        	TRUE,
									GA_TabCycle, 					TRUE,
			          	GA_Underscore,		 		0,
									GA_ActivateKey,				"l",
			          	CHOOSER_LabelArray,  	languages,
			          	CHOOSER_Selected,    	0,
									TAG_DONE),
								TAG_DONE),
							TAG_DONE),

							// Filters Button
							LAYOUT_AddChild, IIntuition->NewObject(NULL, "button.gadget",
								GA_ID,							GID_FILTER_BUTTON,
								GA_Text, 						"_Discover",
								GA_TabCycle, 				TRUE,
								GA_RelVerify, 			TRUE,
								GA_Underscore,		 	0,
								GA_ActivateKey,			"d",
								//BUTTON_AutoButton, 	0,
								TAG_DONE),
						TAG_DONE),
						CHILD_WeightedHeight, 0,
					// END - Top Filter Section

					// START - Bottom List Section
					LAYOUT_AddChild, IIntuition->NewObject(NULL, "layout.gadget",
						LAYOUT_Orientation, 		LAYOUT_ORIENT_VERT,
						LAYOUT_AddChild, gadgets[GID_RADIO_LISTBROWSER] = IIntuition->NewObject(NULL, "listbrowser.gadget",
	            GA_ID,                     	GID_RADIO_LISTBROWSER,
							GA_RelVerify,              	TRUE,
	            GA_TabCycle,               	TRUE,
	            LISTBROWSER_AutoFit,       	TRUE,
	            LISTBROWSER_ColumnInfo,    	columninfo,
	            LISTBROWSER_ColumnTitles,  	TRUE,
							LISTBROWSER_HorizontalProp, TRUE,
							LISTBROWSER_Separators, 		TRUE,
	            LISTBROWSER_ShowSelected,  	TRUE,
	            LISTBROWSER_Striping,      	LBS_ROWS,
	            LISTBROWSER_SortColumn,			0,
	            LISTBROWSER_TitleClickable,	TRUE,
	            TAG_DONE),
          	TAG_DONE),
						CHILD_MinHeight, 	300,
				TAG_DONE),
			TAG_DONE),
		TAG_DONE);
}

Object *buildMainMenu(struct Screen *screen)
{
  return IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ROOT,
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

CONST_STRPTR getChooserText(int gadgetId, uint16 code)
{
  switch (gadgetId)
  {
    case GID_CHOOSER_GENRES:
      return genres[code];
      break;
    case GID_CHOOSER_COUNTRIES:
      return countries[code];
      break;
    case GID_CHOOSER_LANGUAGES:
      return languages[code];
      break;
  }
  return NULL;
}
