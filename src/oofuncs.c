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

#include "oofuncs.h"

extern void CleanExit(const char *);

static char url[255];
static uint32 portNum = 80;
static STRPTR httpreq=NULL,
							httpresp=NULL;

void getRadioStations(void) {
  IDOS->Printf("getRadioStations called\n");
  NETWORKOBJ *net=(NETWORKOBJ *)IOO->NewNetworkObject();
  
  if (net) {
    IDOS->Printf("Network is fine!\n");
    
    strcpy(url, "https://de1.api.radio-browser.info/json/countries");  portNum=443;
    if (net->CreateConnection(url, portNum, TRUE, TRUE)) {
      IDOS->Printf("Connection created just fine!\n");
      
      if (net->GetConnection()) {
        IDOS->Printf("Connection done fine!\n");
        
        IDOS->Printf("Trying to load %s\n", url);
      	httpreq = net->CreateHTTPRequest(url, portNum);
      	net->SendHTTPRequest(httpreq);
      	IDOS->Printf("Response code=%ld\n",(int32)net->GetHTTPResponseCode);
      	httpresp = net->GetHTTPResponse();
      	if (httpresp) { 
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