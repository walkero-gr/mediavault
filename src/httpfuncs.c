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
static CONST_STRPTR getContentTypeExt(STRPTR);
static struct curl_slist *headersList = NULL;

extern uint32 cacheFilenameSize;

struct memory
{
  char    *body;
  char    *type;
  size_t  size;
  LONG    code;
};
static struct memory response = {0};

static size_t gotData(STRPTR buf, size_t itemSize, size_t numItems, void *userData)
{
  size_t bytes = itemSize * numItems;
  struct memory *mem = (struct memory *)userData;

  char *ptr = realloc(mem->body, mem->size + bytes + 1);
  if(ptr == NULL) return 0;  /* out of memory! */

  mem->body = ptr;
  memcpy(&(mem->body[mem->size]), buf, bytes);
  mem->size += bytes;;
  mem->body[mem->size] = 0;
  return bytes;
}

void cleanupHTTPRequest(void)
{
  if (response.body) response.body = NULL;
  if (response.size) response.size = 0;
  if (headersList)   headersList = NULL;
}

STRPTR getResponseBody(void)
{
  if (response.body) return response.body;

  return NULL;
}

LONG getResponseCode(void)
{
  if (response.code > 0) return response.code;

  return 0;
}

STRPTR getResponseType(void)
{
  if (response.type) return response.type;

  return NULL;
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
      curl_easy_setopt(curl, CURLOPT_URL, url);

      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headersList);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);

      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gotData);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

      res = curl_easy_perform(curl);

      if(res != CURLE_OK)
      {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      }

      if(res == CURLE_OK)
      {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response.type);
      }
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
  }
  headersList = NULL; // Clear the header list for the next call
}

BOOL downloadFile(STRPTR url, STRPTR filename, STRPTR destination)
{
  BOOL result = FALSE;

  if (stricmp(url, ""))
  {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);

      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gotData);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

      res = curl_easy_perform(curl);
      if(res == CURLE_OK)
      {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response.type);
      }
      else
      {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      }

      curl_easy_cleanup(curl);

      CONST_STRPTR ext = getContentTypeExt(response.type);
      if (ext)
      {
        char targetFile[cacheFilenameSize];
        FILE *pagefile;
        
        strcpy(targetFile, destination);
        strcat(targetFile, filename);
        strcat(targetFile, ext);

        pagefile = fopen(targetFile, "wb");
        if (pagefile)
        {
          fwrite(response.body, 1, response.size, pagefile);
          result = TRUE;
        }

        fclose(pagefile);
      }

    }
    cleanupHTTPRequest();
    curl_global_cleanup();
  }

  return result;
}

 /**
  *
  * void addRequestHeader(STRPTR)
  *
  * Summary:
  *
  *     This function adds a header in headersList for the next
  *     curl call
  *
  * Parameters   : headerString: the header as a string including
  *                the name and the value
  *
  * Return Value : 
  *
  * Description:
  *
  *     This function uses curl's header list and appends a
  *     new item. This is used with CURLOPT_HTTPHEADER curl option
  *
  */
void addRequestHeader(STRPTR headerString)
{
  headersList = curl_slist_append(headersList, headerString);
}

static CONST_STRPTR getContentTypeExt(STRPTR contentType)
{
  if(contentType)
  {
    if (!stricmp(contentType, "image/jpg"))
    {
      return ".jpg";
    }
    if (!stricmp(contentType, "image/jpeg"))
    {
      return ".jpg";
    }
    if (!stricmp(contentType, "image/png"))
    {
      return ".png";
    }
    if (!stricmp(contentType, "image/webp"))
    {
      return ".webp";
    }
    if (!stricmp(contentType, "image/vnd.microsoft.icon"))
    {
      return ".ico";
    }
    if (!stricmp(contentType, "image/gif"))
    {
      return ".gif";
    }
    if (!stricmp(contentType, "application/octet-stream"))
    {
      return ".lha";
    }
  }
  return NULL;
}
