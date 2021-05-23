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

static  char *requestUrl;
static  int requestPort;
static  char rfc3986[256] = {0},
             html5[256] = {0};


NETWORKOBJ *net;

STRPTR getResponseBody(char *url, int portNum)
{
  requestUrl = url;
  requestPort = portNum;
  STRPTR httpResponse = doRequest();

  IDOS->Printf("URL: %s\n", url);

  if (httpResponse)
  {
    return httpResponse;
  }

  //IDOS->Printf("getResponseBody returned NULL\n");
  return NULL;
}


void rfcTablesInit(void)
{
  uint16 i;
  for (i = 0; i < 256; i++) {
    rfc3986[i]  = ( isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_' ) ? i : 0;
    html5[i]    = ( isalnum(i)||i == '*'||i == '-'||i == '.'||i == '_' ) ? i : (i == ' ') ? '+' : 0;
  }
}

void encode(unsigned char *s, char *enc, char *tb)
{
  for (; *s; s++) {
    if (tb[*s]) sprintf(enc, "%c", tb[*s]);
    else        sprintf(enc, "%%%02X", *s);
    while (*++enc);
  }
}

STRPTR urlEncode(STRPTR value)
{
  STRPTR buf = IExec->AllocVecTags( (IUtility->Strlen(value) * 3) + 1,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  rfcTablesInit();
  encode((unsigned char*)value, buf, html5);

  return buf;
}

static STRPTR doRequest()
{
  STRPTR  httpreq = NULL,
          httpRespBody = NULL;

  //IDOS->Printf("getRadioStations called\n");
  net = (NETWORKOBJ *)IOO->NewNetworkObject();

  if (net != NULL)
  {
    //IDOS->Printf("Network is fine!\n");

    if (net->CreateConnection(requestUrl, requestPort, TRUE, TRUE))
    {
      //IDOS->Printf("Connection created just fine!\n");

      if (net->GetConnection())
      {
        //IDOS->Printf("Connection done fine!\n");

        //IDOS->Printf("Trying to load %s\n", requestUrl);
        httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
        //IDOS->Printf("Create HTTP Request: %s\n", httpreq);
        net->SendHTTPRequest(httpreq);
        //IDOS->Printf("Response code=%ld\n",(int32)net->GetHTTPResponseCode());

        httpRespBody = net->GetResponseBody();

        if (httpRespBody)
        {
          IDOS->Printf("Response\n------------------------\n%s\n", httpRespBody);
          return httpRespBody;
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
