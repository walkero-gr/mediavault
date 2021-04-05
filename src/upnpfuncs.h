/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: upnpfuncs.h
**
** Date: 01-12-2020 22:47:23
**
************************************************************

*/

#ifndef _UPNPFUNCS_H
#define _UPNPFUNCS_H

/* System */
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/time.h>

/* BSDSocket */
#include <netdb.h>
#include <netinet/in.h>

/* Prototypes */
//#include <proto/exec.h>
//#include <proto/utility.h>

/* ANSI C */
#include <string.h>
#include <stdio.h>
//#include <stdlib.h>

#include "globals.h"

const unsigned int BUFLEN = 512;
const char SSDPADDR[16] = "239.255.255.250";
const unsigned int SSDPPORT = 1900;

const char DISCOVERMSG[105] = "M-SEARCH * HTTP/1.1\r\n"\
                              "HOST:239.255.255.250:1900\r\n"\
                              "ST:upnp:rootdevice\r\nMX:2\r\n"\
                              "MX: 3\r\n"\
                              "MAN:\"ssdp:discover\"\r\n"\
                              "\r\n";

#include "globals.h"

#endif
