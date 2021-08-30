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

#define __USE_INLINE__

#include <curl/curl.h>
#include <proto/dos.h>

#include "version.h"
#include "httpfuncs.h"

static CONST_STRPTR userAgent = APPNAME "/" STR(VERSION) "." STR(REVISION) " (AmigaOS)";
//static CONST_STRPTR getContentTypeExt(STRPTR);

struct memory response = {0};

static size_t gotData(STRPTR buf, size_t itemSize, size_t numItems, void *userData)
{
  size_t bytes = itemSize * numItems;
  struct memory *mem = (struct memory *)userData;

  char *ptr = realloc(mem->body, mem->size + bytes + 1);
  if(ptr == NULL) return 0;  /* out of memory! */

  mem->body = ptr;
  memcpy(&(mem->body[mem->size]), buf, bytes);
  mem->size += bytes;
  mem->body[mem->size] = 0;

  return bytes;
}

static void cleanResponse(void)
{
  if (response.body) response.body = NULL;
  if (response.size) response.size = 0;
}

void doHTTPRequest(STRPTR url)
{
  if (stricmp(url, ""))
  {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
      cleanResponse();

      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gotData);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

      res = curl_easy_perform(curl);

      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);
      curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response.type);

      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
  }
}

void cacheFileFromUrl(STRPTR url, ULONG portNum, STRPTR filename)
{
  //STRPTR fileTypeExt = (STRPTR)getContentTypeExt((STRPTR)"image/webp");
  printf("Trying to load %s %ld \n", url, portNum);
  //Printf("filename: %s, %s\nsure", filename, fileTypeExt);
  printf("filename: %s\nsure", filename);
}
/*
void cacheFileFromUrl(STRPTR url, ULONG portNum, STRPTR filename)
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
          //IDOS->Printf("Connection done fine!\n");

          //IDOS->Printf("Trying to load %s at port %ld \n", requestUrl, portNum);
          httpreq = net->CreateHTTPRequest(requestUrl, requestPort);
          //IDOS->Printf("Create HTTP Request: %s\n", httpreq);
          net->SendHTTPRequest(httpreq);
          //IDOS->Printf("GetResponseBody\n%s\n", net->GetResponseBody());

          //IDOS->Printf("GetContentType: %s\n", net->GetContentType());
          //IDOS->Printf("GetContentLength: %lld\n", net->GetContentLength());
          //IDOS->Printf("GetHTTPResponseLength: %lld\n", net->GetHTTPResponseLength());
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
                //IDOS->Printf("File opened just fine!\n");
                size_t i = 0;
                for (; i < net->GetContentLength(); i++) {
                  IDOS->FPutC(fh, httpRespBody[i]);
                }
                //IDOS->Printf("After FPutC!\n");
                IDOS->FClose(fh);
                //IDOS->Printf("After FClose!\n");
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
  //IDOS->Printf("FIN!\n");
}
*/

/*
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
*/
