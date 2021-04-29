/*
  libshandler.c
  Opening and closing libraries and resources
  :ts=2 :vs=-ta
*/

#include "globals.h"
#include "libshandler.h"


void CleanExit(const char *str)
{
  if(strcmp(str, "JustExit")) IDOS->Printf("Error::%s\n",str);

  if(AmiSSLMasterBase)  IExec->CloseLibrary(AmiSSLMasterBase);

  if(IListBrowser)      IExec->DropInterface((APTR) IListBrowser);
  if(ListBrowserBase)   IIntuition->CloseClass((APTR) ListBrowserBase);

  if(ILabel)            IExec->DropInterface((APTR) ILabel);
  if(LabelBase)         IIntuition->CloseClass((APTR) LabelBase);

  if(ILayout)           IExec->DropInterface((APTR) ILayout);
  if(LayoutBase)        IIntuition->CloseClass((APTR) LayoutBase);

  if(IOO)               IExec->DropInterface((APTR) IOO);
  if(OOBase)            IExec->CloseLibrary(OOBase);

  if(IJansson)          IExec->DropInterface((APTR) IJansson);
  if(JanssonBase)       IExec->CloseLibrary(JanssonBase);

  if(IUtility)          IExec->DropInterface((APTR) IUtility);
  if(UtilityBase)       IExec->CloseLibrary(UtilityBase);

  if(IIntuition)        IExec->DropInterface((APTR) IIntuition);
  if(IntuitionBase)     IExec->CloseLibrary(IntuitionBase);

  if(IApplication)      IExec->DropInterface((APTR) IApplication);
  if(ApplicationBase)   IExec->CloseLibrary(ApplicationBase);

  exit(0);
}


void OpenLibs(void)
{
  if ((ApplicationBase = IExec->OpenLibrary( "application.library", 53 )))
  {
    IApplication = (struct ApplicationIFace *)IExec->GetInterface( ApplicationBase, "application", 2, NULL );
    if(!IApplication) CleanExit("Can't open application.library Interface");
  }
  else CleanExit("Can't open application.library version 53");

  if ((IntuitionBase = IExec->OpenLibrary( "intuition.library", 54 )))
  {
    if (LIB_IS_AT_LEAST(IntuitionBase, 54, 6))
    {
      IIntuition = (struct IntuitionIFace *)IExec->GetInterface( IntuitionBase, "main", 1, NULL );
      if(!IIntuition) CleanExit("Can't open intuition.library Interface");
    }
    else CleanExit("Can't open intuition.library version 54.6 and above");
  }
  else CleanExit("Can't open intuition.library version 54");

  if ((UtilityBase = IExec->OpenLibrary( "utility.library", 53 )))
  {
    IUtility = (struct UtilityIFace *)IExec->GetInterface( UtilityBase, "main", 1, NULL );
    if(!IUtility) CleanExit("Can't open utility.library Interface");
  }
  else CleanExit("Can't open utility.library version 53");

  if ((JanssonBase = IExec->OpenLibrary( "jansson.library", 2 )))
  {
    IJansson = (struct JanssonIFace *)IExec->GetInterface( JanssonBase, "main", 1, NULL );
    if(!IJansson) CleanExit("Can't open jansson.library Interface");
  }
  else CleanExit("Can't open jansson.library version 2");

  if ((OOBase=IExec->OpenLibrary("oo.library", 1 )))
  {
    if (LIB_IS_AT_LEAST(OOBase, 1, 9))
    {
      IOO = (struct OOIFace *)IExec->GetInterface( OOBase, "main", 1, NULL );
      if(!IOO) CleanExit("Can't open oo.library Interface");
    }
    else CleanExit("Can't open oo.library version 1.11 and above");
  }
  else CleanExit("Can't open oo.library version 1.11 and above. Is it installed?");

  if ((LayoutBase = (APTR) IIntuition->OpenClass( "gadgets/layout.gadget", 1, &LayoutClass )))
  {
    ILayout = (struct LayoutIFace *)IExec->GetInterface( LayoutBase, "main", 1, NULL );
    if(!ILayout) CleanExit("Can't open Layout Gadget Interface");
  }
  else CleanExit("Can't open Layout Gadget");

  if ((LabelBase = (APTR) IIntuition->OpenClass( "images/label.image", 1, &LabelClass )))
  {
    ILabel = (struct LabelIFace *)IExec->GetInterface( LabelBase, "main", 1, NULL );
    if(!ILabel) CleanExit("Can't open Label Image Interface");
  }
  else CleanExit("Can't open Label Image");

  if ((ListBrowserBase = (APTR) IIntuition->OpenClass( "gadgets/listbrowser.gadget", 1, &ListBrowserClass )))
  {
    IListBrowser = (struct ListBrowserIFace *)IExec->GetInterface( ListBrowserBase, "main", 1, NULL );
    if(!IListBrowser) CleanExit("Can't open ListBrowser Gadget Interface");
  }
  else CleanExit("Can't open ListBrowser");

  if ((AmiSSLMasterBase = IExec->OpenLibrary("amisslmaster.library", 4 )))
  {
    if (!LIB_IS_AT_LEAST(AmiSSLMasterBase, 4, 9))
      CleanExit("Can't open amisslmaster.library version 4.9 and above");
  }
  else CleanExit("Can't open amisslmaster.library version 4.9 and above");
}
