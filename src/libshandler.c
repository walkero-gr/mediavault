/*
**  Opening and closing libraries&resources 
*/

#include "libshandler.h"


void CleanExit(const char *str)
{
  if(strcmp(str,"JustExit"))    IDOS->Printf("Error::%s\n",str);

	if(ILayout)					IExec->DropInterface((APTR) ILayout);
	if(LayoutBase)			 	IIntuition->CloseClass((APTR) LayoutBase);
	
	if(IUtility)					IExec->DropInterface((APTR) IUtility);
  if(UtilityBase)      	IExec->CloseLibrary(UtilityBase);
  
  if(IIntuition)				IExec->DropInterface((APTR) IIntuition);
  if(IntuitionBase)    	IExec->CloseLibrary(IntuitionBase);
  
  exit(0);
}


void OpenLibs(void)
{
  if ((IntuitionBase = IExec->OpenLibrary( "intuition.library", 53 ))) {
    IIntuition = (struct IntuitionIFace *)IExec->GetInterface( IntuitionBase, "main", 1, NULL );
    if(!IIntuition)  CleanExit("Can't open intuition.library Interface");
  }
  else CleanExit("Can't open intuition.library version 53");

  if ((UtilityBase = IExec->OpenLibrary( "utility.library", 53 ))) {
    IUtility = (struct UtilityIFace *)IExec->GetInterface( UtilityBase, "main", 1, NULL );
    if(!IUtility)  CleanExit("Can't open utility.library Interface");
  }
  else CleanExit("Can't open utility.library version 53");

	if ((LayoutBase = (APTR) IIntuition->OpenClass( "gadgets/layout.gadget", 1, &LayoutClass ))) {
	  ILayout	= (struct LayoutIFace *)IExec->GetInterface( LayoutBase, "main", 1, NULL );
	  if(!ILayout)  CleanExit("Can't open Layout Gadget Interface");
	}
	else CleanExit("Can't open Layout Gadget");

}
