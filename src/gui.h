/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: gui.h
**
** Date: 06-04-2021 16:39:23
**
************************************************************

*/


#ifndef _GUI_H
#define _GUI_H

//#include <dos/dos.h>
#include <classes/window.h>
//#include <gadgets/button.h>
#include <gadgets/layout.h>

//#include <proto/intuition.h>
//#include <proto/exec.h>
//#include <proto/window.h>
//#include <proto/layout.h>
//#include <proto/button.h>

struct Screen 	*screen;
struct MsgPort 	*appPort;

enum
{
    WID_MAIN,
    WID_LAST
};

enum
{
    OID_MAIN,
    OID_LAST
};

struct Window *windows[WID_LAST];
Object *objects[OID_LAST];
Object *objMain;

#include "globals.h"

#endif