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

#ifndef _RADIOFUNCS_H
#define _RADIOFUNCS_H

//BOOL getRadioStations(struct filters, int);
//void getRadioTrendStations(void);
//void getRadioPopularStations(void);
//size_t getRadioList(struct List *, int);
void playRadio(struct Node *);
void showRadioInfo(struct Node *);
void freeStationInfo(struct stationInfo *);
void fillRadioList(struct filters, BOOL);
void fillRadioPopularList(void);
void fillRadioTrendList(void);


#endif
