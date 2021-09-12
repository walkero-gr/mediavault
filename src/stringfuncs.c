 /**
  * File:    stringfuncs.c
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     May 2021
  *
  * Summary of File:
  *
  *   This file contains code that manipulate strings
  *   like conversion or clearance.
  *
  */

#include <iconv.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "globals.h"
#include "stringfuncs.h"

#define ICONV_BUFFER 256

static STRPTR useIconv(iconv_t, CONST_STRPTR);
static char rfc3986[256] = {0},
            html5[256] = {0};


//// TODO: Find a way to use different codesets and charsets based on station language
STRPTR charConv(CONST_STRPTR value)
{
  iconv_t cd = (iconv_t) -1;
  char charsetName[128];
  STRPTR convertedValue;

  IDOS->GetVar("Charset", charsetName, sizeof(charsetName), GVF_GLOBAL_ONLY);
  cd = iconv_open(charsetName, "UTF-8");
  convertedValue = useIconv(cd, value);

  if (cd != (iconv_t)-1) iconv_close(cd);

  return convertedValue;
}

ULONG convertVersionToInt(STRPTR version)
{
  char buf[10], *tmp;
  uint32 versionNumber = 0;

  if (!strncmp(version, "v", 1))
    IUtility->Strlcpy(buf, version + 1, sizeof(buf));

  tmp = strtok(buf, ".");
  versionNumber = versionNumber + (atoi(tmp) * 100);
  
  tmp = strtok(NULL, ".");
  versionNumber = versionNumber + (atoi(tmp) * 10);

  tmp = strtok(NULL, ".");
  versionNumber = versionNumber + atoi(tmp);

  return versionNumber;
}

static STRPTR useIconv(iconv_t cd, CONST_STRPTR string)
{
  static char buffer[ICONV_BUFFER];
  size_t inleft, outleft = ICONV_BUFFER - 1;
  CONST_STRPTR inbuf = string;
  STRPTR outbuf = buffer;

  if (string == NULL) return NULL;

  inleft = strlen(string);

  if (cd == (iconv_t)-1) return (STRPTR) string;

  memset(buffer, 0, ICONV_BUFFER);

  if (iconv(cd, &inbuf, &inleft, &outbuf, &outleft) == (size_t)-1)
    return (STRPTR) string;
  else
    return buffer;
}

static void rfcTablesInit(void)
{
  uint16 i;
  for (i = 0; i < 256; i++) {
    rfc3986[i]  = ( isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_' ) ? i : 0;
    html5[i]    = ( isalnum(i)||i == '*'||i == '-'||i == '.'||i == '_' ) ? i : (i == ' ') ? '+' : 0;
  }
}

static void encode(unsigned char *s, char *enc, char *tb)
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


 /**
  *
  * STRPTR now(void)
  *
  * Summary:
  *
  *    This function get the system time and returns
  *    the seconds as a string
  *
  * Parameters   :
  *
  * Return Value : pointer to the string in memory
  *
  */
STRPTR now(void)
{
  uint8 timestampLen = 12;
  STRPTR buf = IExec->AllocVecTags(sizeof(char) * timestampLen,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);
  struct TimeVal tv;

  ITimer->GetSysTime(&tv);
  IUtility->SNPrintf(buf, sizeof(char) * timestampLen, "%lu", tv.Seconds);
  
  return buf;
}

 /**
  *
  * STRPTR SHA1Encrypt(STRPTR string, uint32 stringLen)
  *
  * Summary:
  *
  *    This function encrypts with SHA1 a sting and
  *    and returns the description
  *
  * Parameters  : string: contains the string to be encrypted
  *
  * Return Value: pointer to the string in memory
  *
  * Description:
  *
  *     This function uses EVP methods from openssl
  *     package to encrypt the given string with SHA1.
  *     The EVP result is hex string which needs to be
  *     converted to chars and then returns the result
  *     with a pointer to memory
  *
  */
STRPTR SHA1Encrypt(STRPTR string)
{
  char tmp[8] = "";
  unsigned int stringLen = 41;

  STRPTR buf = IExec->AllocVecTags(sizeof(char) * stringLen,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  EVP_MD_CTX *mdctx;
  const EVP_MD *md = EVP_get_digestbyname("SHA1");
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;

  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, string, stringLen);
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_free(mdctx);

  for (i = 0; i < md_len; i++)
  {
    sprintf(tmp, "%02x", md_value[i]);

    if (i == 0)
    {
      IUtility->Strlcpy(buf, tmp, stringLen);
    }
    else
    {
      IUtility->Strlcat(buf, tmp, stringLen);
    }

  }

  return buf;
}

