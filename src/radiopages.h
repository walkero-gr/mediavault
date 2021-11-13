 /**
  * File:    radiopages.h
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     October 2021
  *
  * Summary of File:
  *
  *   This file contains code for the different
  *   pages for the radio stations sections.
  *
  */  

#ifndef _RADIO_PAGES_H
#define _RADIO_PAGES_H

Object *buildRadioSearchPage(CONST_STRPTR *, CONST_STRPTR *, CONST_STRPTR *);
Object *buildRadioFavouritePage(void);
Object *buildRadioPopularPage(void);
Object *buildRadioTrendPage(void);
Object *buildRadioRightSidebar(struct Screen *, struct RenderHook *);

#endif
