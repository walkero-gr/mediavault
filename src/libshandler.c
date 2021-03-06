/*
  libshandler.c
  Opening and closing libraries and resources
  :ts=2 :vs=-ta
*/

#include "globals.h"
#include "libshandler.h"

struct Library *ApplicationBase;  struct ApplicationIFace   *IApplication;
struct Library *IntuitionBase;    struct IntuitionIFace     *IIntuition;
struct Library *UtilityBase;      struct UtilityIFace       *IUtility;
struct Library *GraphicsBase;     struct GraphicsIFace      *IGraphics;
struct Library *LayersBase;       struct LayersIFace        *ILayers;
struct Library *OOBase;           struct OOIFace            *IOO;
struct Library *JanssonBase;      struct JanssonIFace       *IJansson;
struct Library *AmiSSLMasterBase;

struct ClassLibrary *BitMapBase;
struct ClassLibrary *ButtonBase;
struct ClassLibrary *ChooserBase;
struct ClassLibrary *LabelBase;
struct ClassLibrary *LayoutBase;
struct ClassLibrary *ListBrowserBase;  struct ListBrowserIFace *IListBrowser;
struct ClassLibrary *RequesterBase;
struct ClassLibrary *ScrollerBase;
struct ClassLibrary *StringBase;
struct ClassLibrary *TextEditorBase;
struct ClassLibrary *WindowBase;

Class *BitMapClass;
Class *ButtonClass;
Class *ChooserClass;
Class *LabelClass;
Class *LayoutClass;
Class *ListBrowserClass;
Class *RequesterClass;
Class *ScrollerClass;
Class *StringClass;
Class *TextEditorClass;
Class *WindowClass;

int CleanExit(const char *str)
{
  //## Close Classes and Gadgets
  if(RequesterBase)     IIntuition->CloseClass(RequesterBase);
  if(BitMapBase)        IIntuition->CloseClass(BitMapBase);
  if(TextEditorBase)    IIntuition->CloseClass(TextEditorBase);
  if(ScrollerBase)      IIntuition->CloseClass(ScrollerBase);
  if(StringBase)        IIntuition->CloseClass(StringBase);
  if(ButtonBase)        IIntuition->CloseClass(ButtonBase);
  if(ChooserBase)       IIntuition->CloseClass(ChooserBase);
  if(LabelBase)         IIntuition->CloseClass(LabelBase);
  if(LayoutBase)        IIntuition->CloseClass(LayoutBase);
  if(WindowBase)        IIntuition->CloseClass(WindowBase);

  if(IListBrowser)      IExec->DropInterface((struct Interface *) IListBrowser);
  if(ListBrowserBase)   IIntuition->CloseClass(ListBrowserBase);
  
  //## Close Libraries
  if(AmiSSLMasterBase)  IExec->CloseLibrary(AmiSSLMasterBase);

  if(IOO)               IExec->DropInterface((struct Interface *) IOO);
  if(OOBase)            IExec->CloseLibrary(OOBase);

  if(IJansson)          IExec->DropInterface((struct Interface *) IJansson);
  if(JanssonBase)       IExec->CloseLibrary(JanssonBase);

  if(ILayers)           IExec->DropInterface((struct Interface *) ILayers);
  if(LayersBase)        IExec->CloseLibrary(LayersBase);

  if(IGraphics)         IExec->DropInterface((struct Interface *) IGraphics);
  if(GraphicsBase)      IExec->CloseLibrary(GraphicsBase);

  if(IUtility)          IExec->DropInterface((struct Interface *) IUtility);
  if(UtilityBase)       IExec->CloseLibrary(UtilityBase);

  if(IIntuition)        IExec->DropInterface((struct Interface *) IIntuition);
  if(IntuitionBase)     IExec->CloseLibrary(IntuitionBase);

  if(IApplication)      IExec->DropInterface((struct Interface *) IApplication);
  if(ApplicationBase)   IExec->CloseLibrary(ApplicationBase);

  if(strcmp(str, "JustExit")) 
  {
    IDOS->Printf("Error::%s\n",str);
    return RETURN_ERROR;
  }

  return RETURN_OK;
}


int OpenLibs(void)
{
  //## Libraries opening
  if ((ApplicationBase = IExec->OpenLibrary( "application.library", 53 )))
  {
    IApplication = (struct ApplicationIFace *)IExec->GetInterface( ApplicationBase, "application", 2, NULL );
    if(!IApplication) return CleanExit("Can't open application.library Interface");
  }
  else return CleanExit("Can't open application.library version 53");

  if ((IntuitionBase = IExec->OpenLibrary( "intuition.library", 54 )))
  {
    if (LIB_IS_AT_LEAST(IntuitionBase, 54, 6))
    {
      IIntuition = (struct IntuitionIFace *)IExec->GetInterface( IntuitionBase, "main", 1, NULL );
      if(!IIntuition) return CleanExit("Can't open intuition.library Interface");
    }
    else return CleanExit("Can't open intuition.library version 54.6 and above");
  }
  else return CleanExit("Can't open intuition.library version 54");

  if ((UtilityBase = IExec->OpenLibrary( "utility.library", 53 )))
  {
    IUtility = (struct UtilityIFace *)IExec->GetInterface( UtilityBase, "main", 1, NULL );
    if(!IUtility) return CleanExit("Can't open utility.library Interface");
  }
  else return CleanExit("Can't open utility.library version 53");
  
  if ((GraphicsBase = IExec->OpenLibrary( "graphics.library", 54 )))
  {
    IGraphics = (struct GraphicsIFace *)IExec->GetInterface( GraphicsBase, "main", 1, NULL );
    if(!IGraphics) return CleanExit("Can't open graphics.library Interface");
  }
  else return CleanExit("Can't open graphics.library version 54");

  if ((LayersBase = IExec->OpenLibrary( "layers.library", 54 )))
  {
    ILayers = (struct LayersIFace *)IExec->GetInterface( LayersBase, "main", 1, NULL );
    if(!ILayers) return CleanExit("Can't open layers.library Interface");
  }
  else return CleanExit("Can't open layers.library version 54");

  if ((JanssonBase = IExec->OpenLibrary( "jansson.library", 2 )))
  {
    IJansson = (struct JanssonIFace *)IExec->GetInterface( JanssonBase, "main", 1, NULL );
    if(!IJansson) return CleanExit("Can't open jansson.library Interface");
  }
  else return CleanExit("Can't open jansson.library version 2");

  if ((AmiSSLMasterBase = IExec->OpenLibrary("amisslmaster.library", 4 )))
  {
    if (!LIB_IS_AT_LEAST(AmiSSLMasterBase, 4, 9))
      return CleanExit("Can't open amisslmaster.library version 4.9 and above");
  }
  else return CleanExit("Can't open amisslmaster.library version 4.9 and above");

  if ((OOBase = IExec->OpenLibrary("oo.library", 1 )))
  {
    if (LIB_IS_AT_LEAST(OOBase, 1, 13))
    {
      IOO = (struct OOIFace *)IExec->GetInterface( OOBase, "main", 1, NULL );
      if(!IOO) return CleanExit("Can't open oo.library Interface");
    }
    else return CleanExit("Can't open oo.library version 1.13 and above");
  }
  else return CleanExit("Can't open oo.library version 1.13 and above");

  //## Classes and Gadgets opening
  if ((ListBrowserBase = IIntuition->OpenClass( "gadgets/listbrowser.gadget", 1, &ListBrowserClass )))
  {
    IListBrowser = (struct ListBrowserIFace *)IExec->GetInterface( (struct Library *) ListBrowserBase, "main", 1, NULL );
    if(!IListBrowser) return CleanExit("Can't open ListBrowser Gadget Interface");
  }
  else return CleanExit("Can't open ListBrowser");

  WindowBase = IIntuition->OpenClass( "window.class", 53, &WindowClass );
  if (!WindowBase) return CleanExit("Can't open Window Class");

  LayoutBase = IIntuition->OpenClass( "layout.gadget", 53, &LayoutClass );
  if (!LayoutBase) return CleanExit("Can't open Layout Gadget");

  LabelBase = IIntuition->OpenClass( "label.image", 53, &LabelClass );
  if (!LabelBase) return CleanExit("Can't open Label Image");

  ChooserBase = IIntuition->OpenClass( "chooser.gadget", 53, &ChooserClass );
  if (!ChooserBase) CleanExit("Can't open Chooser Gadget");

  ButtonBase = IIntuition->OpenClass( "button.gadget", 53, &ButtonClass );
  if (!ButtonBase) return CleanExit("Can't open Button Gadget");

  StringBase = IIntuition->OpenClass( "string.gadget", 53, &StringClass );
  if (!StringBase) return CleanExit("Can't open String Gadget");

  ScrollerBase = IIntuition->OpenClass( "scroller.gadget", 53, &ScrollerClass );
  if (!ScrollerBase) return CleanExit("Can't open Scroller Gadget");

  TextEditorBase = IIntuition->OpenClass( "texteditor.gadget", 53, &TextEditorClass );
  if (!TextEditorBase) return CleanExit("Can't open TextEditor Gadget");

  BitMapBase = IIntuition->OpenClass( "bitmap.image", 53, &BitMapClass );
  if (!BitMapBase) return CleanExit("Can't open BitMap Image");

  RequesterBase = IIntuition->OpenClass( "requester.class", 53, &RequesterClass );
  if (!RequesterBase) return CleanExit("Can't open Requester Class");

  return RETURN_OK;
}
