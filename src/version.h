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
#define VERSION	0
#endif

#ifndef REVISION
#define REVISION 1
#endif

#ifndef RELEASE_DATE
#define RELEASE_DATE "8.4.2021"
#endif

#define APPNAME			"MediaVault"
#define VERS				APPNAME " " STR(VERSION) "." STR(REVISION)
#define VSTRING			APPNAME " " STR(VERSION) "." STR(REVISION) " (" RELEASE_DATE ")"
#define VERSTAG			"\0$VER: " STR(VERSION) "." STR(REVISION) " (" RELEASE_DATE ")"

#endif