/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: mainWin.h
**
** Date: 14-04-2021 17:12:10
**
************************************************************

*/


#ifndef _MAINWIN_H
#define _MAINWIN_H

Object *buildMainWindow(struct MsgPort *, Object *, struct Screen *);
Object *buildMainMenu(struct Screen *);
CONST_STRPTR getChooserText(int gadgetId, uint16 code);
void getLeftSidebarContent(void);

#endif
