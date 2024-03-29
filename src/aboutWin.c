/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: aboutWin.c
**
** Date: 17-04-2021 10:56:59
**
************************************************************

*/

#include "globals.h"
#include "gui.h"
#include "aboutWin.h"

// For the scroller at the About window
const uint32 textToScroller[] = {
    GA_TEXTEDITOR_Prop_Entries,    SCROLLER_Total,
    GA_TEXTEDITOR_Prop_First,      SCROLLER_Top,
    GA_TEXTEDITOR_Prop_Visible,    SCROLLER_Visible,
    TAG_END
};   
const uint32 scrollerToText[] = {
    SCROLLER_Total,     GA_TEXTEDITOR_Prop_Entries,
    SCROLLER_Top,       GA_TEXTEDITOR_Prop_First,
    SCROLLER_Visible,   GA_TEXTEDITOR_Prop_Visible,
    TAG_END
};

static Object *buildAboutPage(void);
static Object *buildSupportersPage(void);
static Object *buildCreditsPage(void);

extern Class *BitMapClass;
extern Class *ButtonClass;
extern Class *ClickTabClass;
extern Class *LayoutClass;
extern Class *ScrollerClass;
extern Class *TextEditorClass;
extern Class *WindowClass;

Object *buildAboutWindow(struct MsgPort *appPort, struct Screen *screen)
{
  CONST_STRPTR tabLabels[] = {
    "_About",
    "_Supporters",
    "_Credits",
    NULL
  };

  return IIntuition->NewObject(WindowClass, NULL,
    WA_ScreenTitle,         VSTRING,
    WA_Title,               "About " APPNAME,
    WA_Activate,            TRUE,
    WA_DepthGadget,         TRUE,
    WA_DragBar,             TRUE,
    WA_CloseGadget,         TRUE,
    WA_SizeGadget,          FALSE,
    //// TODO: Make it configurable at preferences
    //WA_Opaqueness,          255,    /* Initial opaqueness on opening (0..255) */
    //WA_OverrideOpaqueness,  TRUE,   /* Override global settings? (TRUE|FALSE) */
    //WA_FadeTime,            250000, /* Duration of transition in microseconds */
    WINDOW_Iconifiable,     TRUE,
    WINDOW_AppPort,         appPort,
    WINDOW_SharedPort,      appPort,
    WINDOW_Position,        WPOS_CENTERSCREEN,
    WINDOW_Layout, IIntuition->NewObject(LayoutClass, NULL,
      GA_ID,                GID_ABOUT_LAYOUT_ROOT,
      LAYOUT_Orientation,   LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter,    TRUE,
      LAYOUT_DeferLayout,   TRUE,

      LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
        GA_ID,                  GID_ABOUT_LAYOUT_TEXT,
        LAYOUT_Orientation,     LAYOUT_ORIENT_HORIZ,
        LAYOUT_VertAlignment,   LALIGN_CENTER,
        LAYOUT_HorizAlignment,  LALIGN_CENTER,
        LAYOUT_BevelStyle,      BVS_NONE,
        LAYOUT_BevelState,      IDS_SELECTED,

        LAYOUT_AddImage, IIntuition->NewObject(BitMapClass, NULL,
          IA_Scalable,        FALSE,
          BITMAP_SourceFile,  IMAGES_PATH LOGO_IMAGE,
          BITMAP_Screen,      screen,
          BITMAP_Masking,     TRUE,
        TAG_END),
        CHILD_WeightedWidth, 30,
        CHILD_MaxHeight, 128,

        LAYOUT_AddChild,  IIntuition->NewObject(ClickTabClass, NULL,
          GA_ID,          GID_ABOUT_CLICKTAB,
          GA_RelVerify,   TRUE,
          GA_Text,        tabLabels,

          CLICKTAB_PageGroup, IIntuition->NewObject(NULL, "page.gadget",
            LAYOUT_DeferLayout,   TRUE,
            PAGE_Add,             buildAboutPage(),
            PAGE_Add,             buildSupportersPage(),
            PAGE_Add,             buildCreditsPage(),
            TAG_DONE),
          
          TAG_DONE),

      TAG_DONE),
      CHILD_MinWidth,   520,
      CHILD_MinHeight,  200,

      LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL,
        GA_ID,              GID_ABOUT_BUTTON_OK,
        GA_RelVerify,       TRUE,
        GA_Text,            "_Okay",
        BUTTON_AutoButton,  0,
      TAG_DONE),
      CHILD_MaxHeight, 40,
    TAG_DONE),
  TAG_DONE);
}

static Object *buildAboutPage(void)
{
  CONST_STRPTR aboutText = VSTRING "\n" \
    "Copyright (c) 2021 George Sokianos\n\n"
    "MediaVault is an open source desktop application for AmigaOS 4, which can be used to discover online audio and video content.\n\n" \
    "Dedicated to my beloved family, who have supported me all these years\nAris, Nefeli and Marily!\n\n" \
    "Contact me at: walkero@gmail.com\n\n" \
    "Report bugs or request features at:\nhttps://github.com/walkero-gr/mediavault/issues/\n\n" \
    "Distributed without warranty under the terms of the GNU General Public License.";

  Object *textBox = IIntuition->NewObject(TextEditorClass, NULL,
      GA_ID,                      GID_ABOUT_TEXT,
      GA_RelVerify,               TRUE,
      GA_TEXTEDITOR_Contents,     aboutText,
      GA_TEXTEDITOR_CursorX,      0,
      GA_TEXTEDITOR_CursorY,      0,
      GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Left,
      GA_TEXTEDITOR_ReadOnly,     TRUE,
      TAG_DONE);

  Object *textBoxScroller = IIntuition->NewObject(ScrollerClass, NULL,
      GA_ID,                  GID_ABOUT_TEXT_SCROLLER,
      SCROLLER_Orientation,   SORIENT_VERT,
      SCROLLER_Arrows,        FALSE,
      ICA_TARGET,             textBox,
      ICA_MAP,                scrollerToText,
      TAG_DONE);

  IIntuition->SetAttrs(textBox,
      ICA_TARGET,   textBoxScroller,
      ICA_MAP,      textToScroller,
      TAG_END);

  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_AddChild, textBox,
      LAYOUT_AddChild, textBoxScroller,
      TAG_DONE);
}

static Object *buildSupportersPage(void)
{
  CONST_STRPTR text =
    "If you would like to support MediaVault's development, and be part of the list below, please visit: https://ko-fi.com/walkero\n\n" \
    "* AmigaPals supporters\n" \
    "\tlevellord\n" \
    "\tRoman Kargin\n" \
    "\n" \
    "* Supporters\n" \
    "\tAndrea Palmate\n" \
    "\tarisamiga\n" \
    "\tBreed\n" \
    "\tDaniel \"trixie\" Jedlicka\n" \
    "\tFalcon_11\n" \
    "\tHobson (cat)\n" \
    "\tjabierdlr\n" \
    "\tJerome Senay\n" \
    "\tktadd\n" \
    "\tMike Brantley\n" \
    "\tmufa\n" \
    "\tnbache\n" \
    "\tPaul Zager\n" \
    "\tPetrol\n" \
    "\tPrimax\n" \
    "\tscottcampbell\n" \
    "\tSkateman\n" \
    "\tSlygon\n" \
    "\tSoLiD-SNaKe\n" \
    "\tThomas Kolsch\n" \
    "\tTim Grooms\n" \
    "\tVagaPPC\n" \
    "\tVulture\n";

  Object *textBox = IIntuition->NewObject(TextEditorClass, NULL,
      GA_ID,                      GID_ABOUT_SUPPORTERS_TEXT,
      GA_RelVerify,               TRUE,
      GA_TEXTEDITOR_Contents,     text,
      GA_TEXTEDITOR_CursorX,      0,
      GA_TEXTEDITOR_CursorY,      0,
      GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Left,
      GA_TEXTEDITOR_ReadOnly,     TRUE,
      GA_TEXTEDITOR_TabSize,      2,
      TAG_DONE);

  Object *textBoxScroller = IIntuition->NewObject(ScrollerClass, NULL,
      GA_ID,                  GID_ABOUT_SUPPORTERS_TEXT_SCROLLER,
      SCROLLER_Orientation,   SORIENT_VERT,
      SCROLLER_Arrows,        FALSE,
      ICA_TARGET,             textBox,
      ICA_MAP,                scrollerToText,
      TAG_DONE);

  IIntuition->SetAttrs(textBox,
      ICA_TARGET,   textBoxScroller,
      ICA_MAP,      textToScroller,
      TAG_END);

  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_AddChild, textBox,
      LAYOUT_AddChild, textBoxScroller,
      TAG_DONE);
}

static Object *buildCreditsPage(void)
{
  CONST_STRPTR text =
    "Developed by George Sokianos\n\n" \
    "Icons by:\n\tMartin (Mason) Merz\n\tIconDesigner";

  Object *textBox = IIntuition->NewObject(TextEditorClass, NULL,
      GA_ID,                      GID_ABOUT_CREDITS_TEXT,
      GA_RelVerify,               TRUE,
      GA_TEXTEDITOR_Contents,     text,
      GA_TEXTEDITOR_CursorX,      0,
      GA_TEXTEDITOR_CursorY,      0,
      GA_TEXTEDITOR_Flow,         GV_TEXTEDITOR_Flow_Left,
      GA_TEXTEDITOR_ReadOnly,     TRUE,
      GA_TEXTEDITOR_TabSize,      2,
      TAG_DONE);

  Object *textBoxScroller = IIntuition->NewObject(ScrollerClass, NULL,
      GA_ID,                  GID_ABOUT_CREDITS_TEXT_SCROLLER,
      SCROLLER_Orientation,   SORIENT_VERT,
      SCROLLER_Arrows,        FALSE,
      ICA_TARGET,             textBox,
      ICA_MAP,                scrollerToText,
      TAG_DONE);

  IIntuition->SetAttrs(textBox,
      ICA_TARGET,   textBoxScroller,
      ICA_MAP,      textToScroller,
      TAG_END);

  return IIntuition->NewObject(LayoutClass, NULL,
      LAYOUT_AddChild, textBox,
      LAYOUT_AddChild, textBoxScroller,
      TAG_DONE);
}

