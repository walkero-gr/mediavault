 /**
  * File:    podcastpages.h
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
  *   pages for the podcast sections.
  *
  */  

#ifndef _PODCAST_PAGES_H
#define _PODCAST_PAGES_H

Object *buildPodcastSearchPage(void);
Object *buildPodcastFavouritePage(void);
Object *buildPodcastListenLaterPage(void);
Object *buildPodcastTrendingPage(CONST_STRPTR *, CONST_STRPTR *);
Object *buildPodcastRightSidebar(struct Screen *, struct RenderHook *);
Object *buildPodcastEpisodesPage(void);

#endif
