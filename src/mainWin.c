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

#include <proto/listbrowser.h>
#include <gadgets/space.h>

#include "globals.h"
#include "gui.h"
#include "mainWin.h"
#include "guifuncs.h"

CONST_STRPTR genres[] =
{
  "All Genres",
  (STRPTR) ~0L,
  "Alternative",
  "Blues",
  "Classical",
  "Country",
  "Dance",
  "Disco",
  "Drum and bass",
  "Electronic",
  "Folk",
  "Jazz",
  "Latin",
  "Love",
  "Metal",
  "New Age",
  "News",
  "Oldies",
  "Pop",
  "Rap",
  "Rock",
  "Reggae",
  "Religion",
  "RnB",
  "Punk",
  "Soul",
  "Sports",
  "Talk",
  "Techno",
  "Trance",
  NULL
};

CONST_STRPTR languages[] =
{
  "All Languages",
  (STRPTR) ~0L,
  "Arabic",
  "Bulgarian",
  "Chinese",
  "Croatian",
  "Czech",
  "Danish",
  "Dutch",
  "English",
  "Finnish",
  "French",
  "German",
  "Greek",
  "Hindi",
  "Hungarian",
  "Irish",
  "Italian",
  "Japanese",
  "Korean",
  "Norwegian",
  "Polish",
  "Portuguese",
  "Romanian",
  "Russian",
  "Serbian",
  "Slovak",
  "Slovenian",
  "Spanish",
  "Swedish",
  "Thai",
  "Turkish",
  "Ukrainian",
  NULL
};

CONST_STRPTR countries[] =
{
  "All Countries",
  (STRPTR) ~0L,
  "Argentina",
  "Australia",
  "Austria",
  "Belgium",
  "Brazil",
  "Canada",
  "Chile",
  "China",
  "Colombia",
  "Croatia",
  "Czechia",
  "France",
  "Germany",
  "Greece",
  "Hungary",
  "India",
  "Ireland",
  "Italy",
  "Mexico",
  "Peru",
  "Poland",
  "Portugal",
  "Romania",
  "Russia",
  "Spain",
  "Switzerland",
  "The Netherlands",
  "The Russian Federation",
  "The United Kingdom",
  "The United States Of America",
  "Turkey",
  "Ukraine",
  NULL
};

extern struct List leftSidebarList;

static Object *buildRadioSearchPage(void);
static Object *buildRadioPopularPage(void);
static Object *buildRadioTrendPage(void);

static const ULONG listToPage[] = {
    LISTBROWSER_Selected,    PAGE_Current,
    TAG_END
};

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ChooserClass;
extern Class *LabelClass;
extern Class *LayoutClass;
extern Class *ListBrowserClass;
extern Class *StringClass;
extern Class *TextEditorClass;
extern Class *WindowClass;


Object *buildMainWindow(struct MsgPort *appPort, Object *winMenu, struct Screen *screen)
{                                           
  //struct DrawInfo *drInfo = IIntuition->GetScreenDrawInfo(screen);

  Object *radioPages = IIntuition->NewObject(NULL, "page.gadget",
        LAYOUT_DeferLayout, TRUE,
        PAGE_Add, gadgets[GID_PAGE_1] = buildRadioSearchPage(),
        PAGE_Add, gadgets[GID_PAGE_2] = buildRadioPopularPage(),
        PAGE_Add, gadgets[GID_PAGE_3] = buildRadioTrendPage(),
        TAG_DONE);

  objects[OID_AVATAR_IMAGE] = IIntuition->NewObject(BitMapClass, NULL,
        GA_ID,              OID_AVATAR_IMAGE,
        IA_Scalable,        FALSE,
        BITMAP_Screen,      screen,
        BITMAP_SourceFile,  LOGO_IMAGE,
        BITMAP_Masking,     TRUE,
        TAG_END);

  struct RenderHook *renderhook = (struct RenderHook *) IExec->AllocSysObjectTags (ASOT_HOOK,
        ASOHOOK_Size,  sizeof(struct RenderHook),
        ASOHOOK_Entry, (HOOKFUNC)renderfunct,
        TAG_END);

  if (renderhook && objects[OID_AVATAR_IMAGE])
  {
    renderhook->img  = objects[OID_AVATAR_IMAGE];
    renderhook->w    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Width;
    renderhook->h    = ((struct Image *)objects[OID_AVATAR_IMAGE])->Height;
    renderhook->fill = FALSE;
  }

  return IIntuition->NewObject(WindowClass, NULL,
    WA_ScreenTitle,         VSTRING,
    WA_Title,               APPNAME,
    WA_Activate,            TRUE,
    WA_CloseGadget,         TRUE,
    WA_DepthGadget,         TRUE,
    WA_DragBar,             TRUE,
    WA_SizeGadget,          TRUE,
    //// TODO: Make it configurable at preferences
    //WA_Opaqueness,          255,    /* Initial opaqueness on opening (0..255) */
    //WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    //WA_FadeTime,            250000, /* Duration of transition in microseconds */
    WA_NewLookMenus,        TRUE,
    WINDOW_AppPort,         appPort,
    WINDOW_Iconifiable,     TRUE,
    WINDOW_IconifyGadget,   TRUE,
    WINDOW_Icon,            IIcon->GetDiskObject("PROGDIR:MediaVault"),
    WINDOW_JumpScreensMenu, TRUE,
    WINDOW_MenuStrip,       winMenu,
    WINDOW_PopupGadget,     TRUE,
    WINDOW_Position,        WPOS_CENTERSCREEN,
    WINDOW_UniqueID,        APPNAME,
    WINDOW_Layout, IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_DeferLayout, TRUE,
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,

            // START - Left Sidebar
            LAYOUT_AddChild, gadgets[GID_LEFT_SIDEBAR] = IIntuition->NewObject(ListBrowserClass, NULL,
              GA_ID,                      GID_LEFT_SIDEBAR,
              GA_RelVerify,               TRUE,
              GA_TabCycle,                TRUE,
              LISTBROWSER_AutoFit,        TRUE,
              LISTBROWSER_ColumnTitles,   FALSE,
              LISTBROWSER_Hierarchical,   TRUE,
              LISTBROWSER_Separators,     FALSE,
              LISTBROWSER_ShowSelected,   TRUE,
              ICA_TARGET,                 radioPages,
              ICA_MAP,                    listToPage,
              TAG_DONE),
              CHILD_WeightedWidth, 20,
              CHILD_MinWidth, 150,
              LAYOUT_WeightBar, TRUE,
            // END - Left Sidebar

            LAYOUT_AddChild, gadgets[GID_PAGES] = radioPages,
            LAYOUT_WeightBar, TRUE,

            // START - Right Sidebar
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              /*
              LAYOUT_AddImage, gadgets[GID_INFO_AVATAR] = IIntuition->NewObject(BitMapClass, NULL,
                GA_ID,              GID_INFO_AVATAR,
                IA_Scalable,        FALSE,
                BITMAP_SourceFile,  "PROGDIR:images/logo_128.png",
                BITMAP_Screen,      screen,
                //BITMAP_Precision, PRECISION_EXACT,
                BITMAP_Masking,     TRUE,
                //BITMAP_Width, 50,
                //BITMAP_Height, 50,
                TAG_END),
                CHILD_WeightedWidth, 30,     
                CHILD_MaxHeight, 128,
              */

              LAYOUT_AddChild, gadgets[GID_INFO_AVATAR] = IIntuition->NewObject(NULL, "space.gadget", // SpaceObject,
                GA_ID,                      GID_INFO_AVATAR,
                SPACE_MinWidth,             128,
                SPACE_MinHeight,            128,
                SPACE_RenderHook,           renderhook,
                GA_Image,                   objects[OID_AVATAR_IMAGE],
                TAG_DONE),
              /*
              LAYOUT_AddChild, gadgets[GID_INFO_AVATAR] = IIntuition->NewObject(ButtonClass, NULL,
                GA_ID,                      GID_INFO_AVATAR,
                GA_Image,                   objects[OID_AVATAR_IMAGE],
                TAG_DONE),
              */
              LAYOUT_AddChild, gadgets[GID_INFO_RADIO_DATA] = IIntuition->NewObject(TextEditorClass, NULL,
                GA_ID,                      GID_INFO_RADIO_DATA,
                GA_RelVerify,               TRUE,
                GA_TEXTEDITOR_BevelStyle,   BVS_NONE,
                GA_TEXTEDITOR_CursorX,      0,
                GA_TEXTEDITOR_CursorY,      0,
                GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Center,
                GA_TEXTEDITOR_ReadOnly,     TRUE,
                GA_TEXTEDITOR_Transparent,  TRUE,
                TAG_DONE),

              LAYOUT_AddChild, IIntuition->NewObject(NULL, "space.gadget",
                TAG_DONE),
              TAG_DONE),
              CHILD_WeightedWidth, 30,
              CHILD_MinWidth, 200,
            // END - Right Sidebar

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
        MA_ID,      MID_ABOUT,
        MA_Label,   "About...",
        MA_Key,     "?",
        MA_Image,   MenuImage("info", screen),
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_ICONIFY,
        MA_Label,   "Iconify",
        MA_Key,     "I",
        MA_Image,   MenuImage("iconify", screen),
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_Label, ML_SEPARATOR,
        TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL,"menuclass",MA_Type,T_ITEM,
        MA_ID,      MID_QUIT,
        MA_Label,   "Quit",
        MA_Key,     "Q",
        MA_Image,   MenuImage("quit", screen),
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

static Object *buildRadioSearchPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
        LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,

        // START - Top Filter Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_BevelStyle,      BVS_GROUP,
          LAYOUT_Label,           "Filter stations by",

          // Top filter section with the Name Text Box with Label
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
            LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
              //LABEL_DrawInfo, drInfo,
              LABEL_Text, "_Name",
              TAG_END),
            LAYOUT_AddChild, gadgets[GID_FILTERS_NAME] = IIntuition->NewObject(StringClass, NULL,
              GA_ID,                GID_FILTERS_NAME,
              GA_RelVerify,         TRUE,
              GA_TabCycle,          TRUE,
              GA_ActivateKey,       "n",
              STRINGA_MinVisible,   10,
              STRINGA_MaxChars,     40,
              TAG_DONE),
            TAG_DONE),

          // Top filter section with the select boxes
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            // Genres Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Genre",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_GENRES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "g",
                CHOOSER_LabelArray,   genres,
                CHOOSER_MaxLabels,    40,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),

            // Countries Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Country",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_COUNTRIES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "c",
                CHOOSER_LabelArray,   countries,
                CHOOSER_MaxLabels,    40,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),

            // Languages Select Box with Label
            LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
              LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
              LAYOUT_AddImage, IIntuition->NewObject(LabelClass, NULL,
                //LABEL_DrawInfo, drInfo,
                LABEL_Text, "_Language",
                TAG_END),
              LAYOUT_AddChild, IIntuition->NewObject(ChooserClass, NULL,
                GA_ID,                GID_CHOOSER_LANGUAGES,
                GA_RelVerify,         TRUE,
                GA_TabCycle,          TRUE,
                GA_ActivateKey,       "l",
                CHOOSER_LabelArray,   languages,
                CHOOSER_MaxLabels,    33,
                CHOOSER_Selected,     0,
                TAG_DONE),
              TAG_DONE),
            TAG_DONE),

            // Filters Button
            LAYOUT_AddChild, gadgets[GID_FILTER_BUTTON] = IIntuition->NewObject(ButtonClass, NULL,
              GA_ID,              GID_FILTER_BUTTON,
              GA_Text,            "_Discover",
              GA_TabCycle,        TRUE,
              GA_RelVerify,       TRUE,
              GA_ActivateKey,     "d",
              TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,
        // END - Top Filter Section

        // START - Bottom List Section
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation,     LAYOUT_ORIENT_VERT,
          LAYOUT_AddChild, gadgets[GID_RADIO_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                      GID_RADIO_LISTBROWSER,
            GA_RelVerify,               TRUE,
            GA_TabCycle,                TRUE,
            LISTBROWSER_ColumnTitles,   TRUE,
            LISTBROWSER_HorizontalProp, TRUE,
            LISTBROWSER_Separators,     TRUE,
            LISTBROWSER_ShowSelected,   TRUE,
            LISTBROWSER_Striping,       LBS_ROWS,
            LISTBROWSER_SortColumn,     0,
            LISTBROWSER_TitleClickable, TRUE,
            TAG_DONE),
          TAG_DONE),
          CHILD_MinHeight,  300,
          // END - Bottom List Section
        TAG_DONE);
}

static Object *buildRadioPopularPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Most listened to over 24 hours",

        LAYOUT_AddChild, gadgets[GID_RADIO_POPULAR_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_RADIO_POPULAR_LISTBROWSER,
          GA_RelVerify,               TRUE,
          GA_TabCycle,                TRUE,
          LISTBROWSER_ColumnTitles,   TRUE,
          LISTBROWSER_HorizontalProp, TRUE,
          LISTBROWSER_Separators,     TRUE,
          LISTBROWSER_ShowSelected,   TRUE,
          LISTBROWSER_Striping,       LBS_ROWS,
          LISTBROWSER_SortColumn,     0,
          LISTBROWSER_TitleClickable, TRUE,
          TAG_DONE),

        TAG_DONE);
}

static Object *buildRadioTrendPage(void)
{
  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_BevelStyle,      BVS_GROUP,
      LAYOUT_Label,           "Trending in the last 24 hours",
      
        LAYOUT_AddChild, gadgets[GID_RADIO_TREND_LISTBROWSER] = IIntuition->NewObject(ListBrowserClass, NULL,
          GA_ID,                      GID_RADIO_TREND_LISTBROWSER,
          GA_RelVerify,               TRUE,
          GA_TabCycle,                TRUE,
          LISTBROWSER_ColumnTitles,   TRUE,
          LISTBROWSER_HorizontalProp, TRUE,
          LISTBROWSER_Separators,     TRUE,
          LISTBROWSER_ShowSelected,   TRUE,
          LISTBROWSER_Striping,       LBS_ROWS,
          LISTBROWSER_SortColumn,     0,
          LISTBROWSER_TitleClickable, TRUE,
          TAG_DONE),

        TAG_DONE);
}

void getLeftSidebarContent(void)
{

  IExec->NewList(&leftSidebarList);

  struct Node *node;

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    1,
    LBNA_Flags,         LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Radio",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Popular",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }

  node = IListBrowser->AllocListBrowserNode(1,
    LBNA_Generation,    2,
    LBNA_Column,        0,
      LBNCA_CopyText,   TRUE,
      LBNCA_Text,       "Trending",
    TAG_DONE);
  if (node)
  {
    IExec->AddTail( &leftSidebarList, node );
  }
}
