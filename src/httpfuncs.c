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

static STRPTR doRequest(void);
static CONST_STRPTR getContentTypeExt(STRPTR);         

static char *requestUrl;
static int  requestPort;
static char rfc3986[256] = {0},
            html5[256] = {0};


NETWORKOBJ *net;

STRPTR getResponseBody(char *url, int portNum)
{
  if (IUtility->Stricmp(url, ""))
  {
    requestUrl = url;
    requestPort = portNum;
    STRPTR httpResponse = doRequest();

    IDOS->Printf("URL: %s\n", url);

    if (httpResponse)
    {
      return httpResponse;
    }
  }
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

static STRPTR doRequest(void)
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

        //IDOS->Printf("GetContentType\n%s\n", net->GetContentType());

        httpRespBody = net->GetResponseBody();

        if (httpRespBody)
        {
          //IDOS->Printf("Response\n------------------------\n%s\n", httpRespBody);
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
  
void cacheFileFromUrl(STRPTR url, ULONG portNum, STRPTR filename)
{
  if (IUtility->Stricmp(url, ""))
  {
    requestUrl = url;
    requestPort = portNum;

    IDOS->Printf("URL: %s\n", url);

    STRPTR  httpreq = NULL;

    IDOS->Printf("cacheFileFromUrl called\n");
    net = (NETWORKOBJ *)IOO->NewNetworkObject();

    if (net != NULL)
    {
      IDOS->Printf("Network is fine!\n");

      if (net->CreateConnection(requestUrl, requestPort, TRUE, TRUE))
      {
        IDOS->Printf("Connection created just fine!\n");

        if (net->GetConnection())
        {
          IDOS->Printf("Connection done fine!\n");

          IDOS->Printf("Trying to load %s at port %ld \n", requestUrl, portNum);
          httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
          IDOS->Printf("Create HTTP Request: %s\n", httpreq);
          net->SendHTTPRequest(httpreq);
          //IDOS->Printf("GetResponseBody\n%s\n", net->GetResponseBody());

          IDOS->Printf("GetContentType: %s\n", net->GetContentType());
          IDOS->Printf("GetContentLength: %lld\n", net->GetContentLength());
          IDOS->Printf("GetHTTPResponseLength: %lld\n", net->GetHTTPResponseLength());
          STRPTR fileTypeExt = (STRPTR)getContentTypeExt(net->GetContentType());

          if ((net->GetContentLength() < net->GetHTTPResponseLength()) && fileTypeExt)
          {
            STRPTR httpRespBody = net->GetResponseBody();
            if (httpRespBody)
            {
              char targetFile[128];
              
              IUtility->Strlcpy(targetFile, CACHE_DIR, sizeof(targetFile));
              IUtility->Strlcat(targetFile, filename, sizeof(targetFile));
              IUtility->Strlcat(targetFile, fileTypeExt, sizeof(targetFile));
                        
              BPTR fh = IDOS->FOpen(targetFile, MODE_NEWFILE, 0);
              if (fh)
              {
                IDOS->Printf("File opened just fine!\n");
                size_t i = 0;
                for (; i < net->GetContentLength(); i++) {
                  IDOS->FPutC(fh, httpRespBody[i]);
                }
                IDOS->Printf("After FPutC!\n");
                IDOS->FClose(fh);
                IDOS->Printf("After FClose!\n");
              }
              else IDOS->Printf("File Open failed\n");
              //return httpRespBody;
              
            }
            else IDOS->Printf("No response\n");
          }

        }
        else IDOS->Printf("Connection failed!\n");

        net->DisposeConnection();
      }
      else IDOS->Printf("Connection creation failed!\n");

      IOO->DisposeNetworkObject(net);
    }
    else IDOS->Printf("Network Object creation failed!\n");
  }
  IDOS->Printf("FIN!\n");
}

static CONST_STRPTR getContentTypeExt(STRPTR contentType)
{
  if (!IUtility->Stricmp(contentType, "image/jpeg"))
  {
    return ".jpg";
  }
  if (!IUtility->Stricmp(contentType, "image/png"))
  {
    return ".png";
  }
  if (!IUtility->Stricmp(contentType, "image/webp"))
  {
    return ".webp";
  }
  if (!IUtility->Stricmp(contentType, "image/vnd.microsoft.icon"))
  {
    return ".ico";
  }
  if (!IUtility->Stricmp(contentType, "image/gif"))
  {
    return ".gif";
  }
  return NULL;
}

ULONG getPortByURL(STRPTR url)
{
  //if (!IUtility->Strnicmp(url, "https://", 8))  return NET_PORT_HTTPS;
  if (!IUtility->Strnicmp(url, "http://", 7))  return NET_PORT_HTTP;

  return NET_PORT_HTTPS;
}
