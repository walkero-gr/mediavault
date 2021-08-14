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
static CONST_STRPTR userAgent = "MediaVault/1.3 (AmigaOS)";

static STRPTR   requestUrl;
static uint32   requestPort;
static uint32   requestMethod;
static char     rfc3986[256] = {0},
                html5[256] = {0};


NETWORKOBJ *net;

STRPTR getResponseBody(STRPTR url, uint32 portNum, uint32 method)
{
  if (IUtility->Stricmp(url, ""))
  {
    requestUrl = url;
    requestPort = portNum;
    requestMethod = method;
    //IDOS->Printf("URL: %s\n", url);
    STRPTR httpResponse = doRequest();

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
        IDOS->Printf("DBG Before create KEYVALUE!\n");
        KEYVALUEOBJ *requestHeaders=IOO->NewKeyValueObject();
        IDOS->Printf("DBG KEYVALUE CREATED!\n");
        if (requestHeaders)
        {
          IDOS->Printf("DBG Before add a value in KEYVALUE!\n");
          requestHeaders->AddKeyValue((STRPTR)"User-Agent", (STRPTR)userAgent);
          IDOS->Printf("DBG After add a value in KEYVALUE!\n");
          //IDOS->Printf("Connection done fine!\n");

          //IDOS->Printf("Trying to load %s\n", requestUrl);
          //httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
          httpreq = net->CreateHTTPRequestMethod(requestUrl, requestPort, requestMethod, requestHeaders);
          IDOS->Printf("DBG After CreateHTTPRequestMethod!\n");

          requestHeaders->Clear();
          IOO->DisposeKeyValueObject(requestHeaders);
          requestHeaders=NULL;
          IDOS->Printf("DBG After clean of KEYVALUE!\n");
          //IDOS->Printf("Create HTTP Request: %s\n", httpreq);
          net->SendHTTPRequest(httpreq);
          IDOS->Printf("Response code=%ld\n",(int32)net->GetHTTPResponseCode());

          IDOS->Printf("GetContentType\n%s\n", net->GetContentType());

          httpRespBody = net->GetResponseBody();

          IDOS->Printf("DBG After GetResponseBody!\n");

          if (httpRespBody)
          {
            IDOS->Printf("Response\n------------------------\n%s\n", httpRespBody);
            return httpRespBody;
          }
          else IDOS->Printf("No response\n");
        }
        else IDOS->Printf("DBG KEYVALUE is null!\n");
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
  
void cacheFileFromUrl(STRPTR url, ULONG portNum, STRPTR filename, CONST_STRPTR targetFolder)
{
  if (IUtility->Stricmp(url, ""))
  {
    requestUrl = url;
    requestPort = portNum;

    //IDOS->Printf("URL: %s\n", url);

    STRPTR  httpreq = NULL;

    //IDOS->Printf("cacheFileFromUrl called\n");
    net = (NETWORKOBJ *)IOO->NewNetworkObject();

    if (net != NULL)
    {
      //IDOS->Printf("Network is fine!\n");

      if (net->CreateConnection(requestUrl, requestPort, TRUE, TRUE))
      {
        //IDOS->Printf("Connection created just fine!\n");

        if (net->GetConnection())
        {
          IDOS->Printf("Connection done fine!\n");

          KEYVALUEOBJ *requestHeaders=IOO->NewKeyValueObject();

          if (requestHeaders)
          {
            requestHeaders->AddKeyValue((STRPTR)"User-Agent", (STRPTR)userAgent);

            IDOS->Printf("Trying to load %s at port %ld \n", requestUrl, portNum);
            //httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
            httpreq = net->CreateHTTPRequestMethod(requestUrl, requestPort, HTTP_GET, requestHeaders);
            IDOS->Printf("Create HTTP Request: %s\n", httpreq);
            
            
            net->SendHTTPRequest(httpreq);

            int32 responseCode = net->GetHTTPResponseCode();
            IDOS->Printf("GetResponse\n%s\n\n", net->GetHTTPResponse());
            IDOS->Printf("GetResponseBody\n%s\n\n", net->GetResponseBody());
            IDOS->Printf("Response code=%ld\n\n", responseCode);
            IDOS->Printf("GetContentType: %s\n\n", net->GetContentType());
            IDOS->Printf("GetContentLength: %lld\n\n", net->GetContentLength());
            IDOS->Printf("GetHTTPResponseLength: %lld\n\n", net->GetHTTPResponseLength());

            if ((responseCode == 301) || (responseCode == 302))
            {
              IDOS->Printf("\n===============================\n");
              IDOS->Printf("GetAlternativeURL: %s\n\n", net->GetAlternativeURL());
              httpreq = net->CreateHTTPRequestMethod(net->GetAlternativeURL(), requestPort, HTTP_GET, requestHeaders);
              net->SendHTTPRequest(httpreq);
              responseCode = net->GetHTTPResponseCode();
              IDOS->Printf("GetResponse\n%s\n\n", net->GetHTTPResponse());
              IDOS->Printf("GetResponseBody\n%s\n\n", net->GetResponseBody());
              IDOS->Printf("Response code=%ld\n\n", responseCode);
              IDOS->Printf("GetAlternativeURL: %s\n\n", net->GetAlternativeURL());
              IDOS->Printf("GetContentType: %s\n\n", net->GetContentType());
              IDOS->Printf("GetContentLength: %lld\n\n", net->GetContentLength());
              IDOS->Printf("GetHTTPResponseLength: %lld\n\n", net->GetHTTPResponseLength());
              IDOS->Printf("\n===============================\n");
            }


            requestHeaders->Clear();
            IOO->DisposeKeyValueObject(requestHeaders);
            requestHeaders=NULL;
            IDOS->Printf("Clear KeyValueObj\n");

            STRPTR fileTypeExt = (STRPTR)getContentTypeExt(net->GetContentType());

            //if ((net->GetContentLength() < net->GetHTTPResponseLength()) && fileTypeExt)
            if (TRUE)
            {
              STRPTR httpRespBody = net->GetResponseBody();
              if (httpRespBody)
              {
                char targetFile[128];
                
                IUtility->Strlcpy(targetFile, targetFolder, sizeof(targetFile));
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
        }
        else IDOS->Printf("Connection failed!\n");

        net->DisposeConnection();
      }
      else IDOS->Printf("Connection creation failed!\n");

      IOO->DisposeNetworkObject(net);
    }
    else IDOS->Printf("Network Object creation failed!\n");
  }
  //IDOS->Printf("FIN!\n");
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
  if (!IUtility->Stricmp(contentType, "text/html"))
  {
    return ".html";
  }
  return NULL;
}

ULONG getPortByURL(STRPTR url)
{
  //if (!IUtility->Strnicmp(url, "https://", 8))  return NET_PORT_HTTPS;
  if (!IUtility->Strnicmp(url, "http://", 7))  return NET_PORT_HTTP;

  return NET_PORT_HTTPS;
}
