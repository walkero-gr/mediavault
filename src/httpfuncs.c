/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: httpfuncs.c
**
** Date: 04-04-2021 22:29:30
**
************************************************************

*/

#include "globals.h"
#include "libshandler.h"
#include "httpfuncs.h"

static STRPTR doRequest();
static STRPTR getResponseJSON(STRPTR);

static char *requestUrl;
static int requestPort;

NETWORKOBJ *net;

STRPTR getResponseBody(char *url, int portNum)
{
  requestUrl = url;
  requestPort = portNum;
  STRPTR httpResponse = doRequest();
	
	if (httpResponse)
	{
  	return getResponseJSON(httpResponse);
  }
  
  IDOS->Printf("getResponseBody returned NULL\n");
  return NULL;
}

static STRPTR getResponseJSON(STRPTR response)
{
  STRPTR tmp = strtok(response, "\n");
  
  while (tmp != NULL)
  {
    if (!IUtility->Strnicmp(tmp, "[{", 2))
    {
    	return tmp;
    } 
    tmp = strtok(NULL, "\n");
  }
  
  return NULL;
}

static STRPTR doRequest()
{
  STRPTR 	httpreq = NULL,
					httpresp = NULL;
					
  IDOS->Printf("getRadioStations called\n");
  net = (NETWORKOBJ *)IOO->NewNetworkObject();
  
  if (net)
  {
    IDOS->Printf("Network is fine!\n");
		
    if (net->CreateConnection(requestUrl, requestPort, TRUE, TRUE))
    {
      IDOS->Printf("Connection created just fine!\n");
      
      if (net->GetConnection())
      {
        IDOS->Printf("Connection done fine!\n");
        
        IDOS->Printf("Trying to load %s\n", requestUrl);
      	httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
      	IDOS->Printf("Create HTTP Request: %s\n", httpreq);
      	net->SendHTTPRequest(httpreq);
      	IDOS->Printf("Response code=%ld\n",(int32)net->GetHTTPResponseCode);      	
      	
      	httpresp = net->GetHTTPResponse();
      	
      	if (httpresp)
      	{ 
      		IDOS->Printf("Response GOOD\n------------------------\n%s\n", httpresp);
      		return httpresp;
        }
      	else IDOS->Printf("No response\n");        
      }
      else IDOS->Printf("Connection failed!\n");

      net->DisposeConnection();
    }
    else IDOS->Printf("Connection creation failed!\n");

    IOO->DisposeNetworkObject(net);
  }
  else IDOS->Printf("Network Object creation failed!\n");  

  return NULL;
}
