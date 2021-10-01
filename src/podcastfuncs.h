/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: radiofuncs.h
**
** Date: 24-04-2021 10:01:42
**
************************************************************

*/

#ifndef _PODCASTFUNCS_H
#define _PODCASTFUNCS_H

size_t getPodcastList(struct List *, int);
void showPodcastInfo(struct Node *);
void showPodcastEpisodeInfo(struct Node *);
void fillPodcastList(struct filters);
void playPodcast(struct Node *);

#endif
