/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: oofuncs.c
**
** Date: 04-04-2021 22:29:30
**
************************************************************

*/


#include <oo/ooNetwork.h>
#include <proto/oo.h>

struct Library *OOBase=NULL;
struct OOIFace *IOO=NULL;


void ooDiscoverUPnPServers( void )
{
	OOBase=(struct Library *)IExec->OpenLibrary("oo.library",1);
 	if (OOBase!=NULL) {
 	  
 	}



}