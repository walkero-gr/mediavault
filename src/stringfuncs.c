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
  *   and dates, like conversion or clearance.
  *
  */

#include <iconv.h>
#include <math.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "globals.h"
#include "libshandler.h"
#include "stringfuncs.h"

#define ICONV_BUFFER 256
#define UNIX_AMIGA_EPOCH_DIFF ((365 * 8 + 2) * 24 * 60 * 60)

static STRPTR useIconv(iconv_t, CONST_STRPTR);
static char rfc3986[256] = {0},
            html5[256] = {0};


// TODO: Find a way to use different codesets and charsets based on station language
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
  * ULONG now(void)
  *
  * Summary:
  *
  *    This function get the system time and returns
  *    the seconds
  *
  * Parameters   :
  *
  * Return Value : pointer to the string in memory
  *
  * Description:
  *
  *     This function uses the timer.device to get the
  *     current system date and time in epoch seconds
  *
  */
ULONG now(void)
{
  struct TimeVal tv;
  ITimer->GetSysTime(&tv);

  return tv.Seconds;
}

 /**
  *
  * ULONG nowUnix(ULONG)
  *
  * Summary:
  *
  *     This function add the Unix epoch diff to the
  *     given seconds
  *
  * Parameters   : seconds: contains the seconds
  *
  * Return Value : the given seconds + the unix epoch
  *                difference
  *
  * Description:
  *
  *     Because the epoch of Amiga starts at 1/1/1978
  *     but the UNIX one starts at 1/1/1970, there is
  *     a difference of 8 years, that needs to be added
  *     at the result.
  *
  */
ULONG nowUnix(ULONG seconds)
{
  return seconds + UNIX_AMIGA_EPOCH_DIFF;
}

 /**
  *
  * int32 getOffsetUTC(void)
  *
  * Summary:
  *
  *     This function gets the system UTC offset in
  *     minutes, based on the timezone
  *
  * Parameters   :
  *
  * Return Value : the UTC offset in seconds
  *
  * Description:
  *
  *     This function uses the timezone.library
  *     to get the system timezone and get the
  *     UTC offset in minutes, and returns it in
  *     seconds
  *
  */
int32 getOffsetUTC(void)
{
  int32 utcOffset = 0;
  ITimezone->GetTimezoneAttrs(NULL, TZA_UTCOffset, &utcOffset, TAG_END);

  return utcOffset * 60;
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
  unsigned int stringLen = 70;

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

 /**
  *
  * void stringToLower(STRPTR string)
  *
  * Summary:
  *
  *    This function converts a string to lower case
  *
  * Parameters  : string: contains the string to convert
  *
  * Return Value: pointer to the string in memory
  *
  */
void stringToLower(STRPTR subject)
{
  size_t i = 0;
  for(i = 0; subject[i] != '\0'; i++)
    subject[i] = IUtility->ToLower(subject[i]);
}

 /**
  *
  * void strReplace(CONST_STRPTR search, CONST_STRPTR replace, STRPTR subject)
  *
  * Summary:
  *
  *    This function replaces all the occurences of a
  *    string with an other
  *
  * Parameters  : search: contains the string to replace
  *               replace: the new string
  *               subject: the original string
  *
  * Return Value:
  *
  */
void strReplace(CONST_STRPTR search, CONST_STRPTR replace, STRPTR subject)
{
  char buf[2048];
  //to store the pointer returned from strstr
  STRPTR ch;
  
  ch = strstr(subject, search);
  if (ch == NULL)
    return;

  //copy all the content to buffer before the first occurrence of the search string
  strncpy(buf, subject, ch - subject);

  //prepare the buffer for appending by adding a null to the end of it
  buf[ch - subject] = 0;

  //append using sprintf function
  sprintf(buf + (ch - subject), "%s%s", replace, ch + strlen(search));

  //empty subject for copying
  subject[0] = 0;
  strcpy(subject, buf);

  //pass recursively to replace other occurrences
  return strReplace(search, replace, subject);
}

STRPTR durationPretty(ULONG duration)
{
  STRPTR buf = IExec->AllocVecTags(sizeof(char) * 12,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  ULONG minutes = ceil(duration / 60);
  uint8 hours = floor(minutes / 60);

  if (hours > 0)
  {
    minutes = minutes - (hours * 60);
    IUtility->SNPrintf(buf, sizeof(char) * 12, "%ld hr %lu mins", hours, minutes);
  }
  else
  {
    IUtility->SNPrintf(buf, sizeof(char) * 12, "%lu mins", minutes);
  }

  return buf;
}
