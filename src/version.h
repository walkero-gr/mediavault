/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: version.h
**
** Date: 08-04-2021 14:37:34
**
************************************************************

*/
#ifndef _VERSION_H
#define _VERSION_H

#define STR_HELPER(s) #s      //stringify argument
#define STR(s) STR_HELPER(s)  //indirection to expand argument macros

#ifndef VERSION
#define VERSION 1
#endif

#ifndef REVISION
#define REVISION 4
#endif

#ifndef PATCH
#define PATCH 0
#endif

#ifndef RELEASE_DATE
#define RELEASE_DATE "5.10.2021"
#endif

#define APPNAME     "MediaVault"
#define VERS        APPNAME " " STR(VERSION) "." STR(REVISION) "." STR(PATCH)
#define VSTRING     APPNAME " " STR(VERSION) "." STR(REVISION) "." STR(PATCH) " (" RELEASE_DATE ")"
#define VERSTAG     "\0$VER: " VSTRING

#endif
