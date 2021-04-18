/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: oofuncs.c
**
** Date: 04-04-2021 22:29:30
**
************************************************************

*/

#include "globals.h"
#include "libshandler.h"
#include "oofuncs.h"

static char url[255];
static CONST_STRPTR radioAPIUrl= "https://de1.api.radio-browser.info/json";
static STRPTR httpreq=NULL,
							httpresp=NULL;
							

// TODO: Need to break this method to smaller ones
void getRadioStations(char *selName, char *selGenre, char *selLanguage, char *selCountry) 
{
  IDOS->Printf("getRadioStations\n~~~~~~~~~~~~~~~~~~~~~~~\nName: %s\nGenre: %s\nLanguage: %s\nCountry: %s\n", selName, selGenre, selLanguage, selCountry);
  
  IDOS->Printf("getRadioStations called\n");
  NETWORKOBJ *net=(NETWORKOBJ *)IOO->NewNetworkObject();
  
  if (net)
  {
    IDOS->Printf("Network is fine!\n");
    
    IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
    IUtility->Strlcat(url, "/stations/search?limit=2", sizeof(url));
		if (IUtility->Stricmp(selName, ""))
		{
		  IDOS->Printf("Name is set!\n");
		  IUtility->Strlcat(url, "&name=", sizeof(url));
		  IUtility->Strlcat(url, selName, sizeof(url));
		}
		if (IUtility->Stricmp(selGenre, ""))
		{
		  IDOS->Printf("Genre is set!\n");
		  IUtility->Strlcat(url, "&tag=", sizeof(url));
		  IUtility->Strlcat(url, selGenre, sizeof(url));
		}
		if (IUtility->Stricmp(selLanguage, ""))
		{
		  IDOS->Printf("language is set!\n");
		  IUtility->Strlcat(url, "&language=", sizeof(url));
		  IUtility->Strlcat(url, selLanguage, sizeof(url));
		}
		if (IUtility->Stricmp(selCountry, ""))
		{
		  IDOS->Printf("country is set!\n");
		  IUtility->Strlcat(url, "&country=", sizeof(url));
		  IUtility->Strlcat(url, selCountry, sizeof(url));
		}
		
		
    if (net->CreateConnection(url, NET_PORT_HTTPS, TRUE, TRUE))
    {
      IDOS->Printf("Connection created just fine!\n");
      
      if (net->GetConnection())
      {
        IDOS->Printf("Connection done fine!\n");
        
        IDOS->Printf("Trying to load %s\n", url);
      	httpreq = net->CreateHTTPRequest(url, NET_PORT_HTTPS);
      	net->SendHTTPRequest(httpreq);
      	IDOS->Printf("Response code=%ld\n",(int32)net->GetHTTPResponseCode);
      	httpresp = net->GetHTTPResponse();
      	if (httpresp)
      	{ 
      		IDOS->Printf("%s\n", httpresp); 
      	}
      	else IDOS->Printf("No response\n");
        
      }
      else CleanExit("Connection failed!\n");
      
      net->DisposeConnection();
    }
    else CleanExit("Connection creation failed!\n");
    
    IOO->DisposeNetworkObject(net);
  }
  else CleanExit("Network Object creation failed!\n");
}