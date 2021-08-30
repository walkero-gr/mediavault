/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: stringfuncs.c
**
** Date: 23-05-2021 13:48:03
**
************************************************************

*/

#include <iconv.h>

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

