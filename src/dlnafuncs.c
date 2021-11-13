 /**
  * File:    dlnafuncs.c
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     November 2021
  *
  * Summary of File:
  *
  *   This file contains code about DLNA servers
  *
  */


//#include <netdb.h>
//#include <netinet/in.h>
//#include <proto/dos.h>
#include <proto/listbrowser.h>
#include <proto/socket.h>


#include "globals.h"
#include "gui.h"
#include "guifuncs.h"
#include "dlnafuncs.h"


static CONST_STRPTR SSDPIP = "239.255.255.250";
static ULONG SSDPPORT = 1900;

const char DISCOVERMSG[] =  "M-SEARCH * HTTP/1.1\r\n"\
                            "HOST:239.255.255.250:1900\r\n"\
                            "ST:upnp:rootdevice\r\nMX:2\r\n"\
                            "MX: 3\r\n"\
                            "MAN:\"ssdp:discover\"\r\n"\
                            "\r\n";

extern struct ColumnInfo *mediaServerColInfo;
extern struct List mediaServerList;



static struct sockaddr_in socketConnectAton(void)
{
  struct sockaddr_in ssdpAddr;
  memset((STRPTR)&ssdpAddr, 0, sizeof(ssdpAddr));

  ssdpAddr.sin_family = AF_INET ;
  ssdpAddr.sin_port = htons(SSDPPORT);
  IDOS->Printf("DBG: %s\t%lu\n", SSDPIP, SSDPPORT);
  if (ISocket->inet_aton((STRPTR)SSDPIP, &ssdpAddr.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(EXIT_FAILURE);
    //return NULL;
  }

  return ssdpAddr;
}


static void addServerData(STRPTR upnpServerData, int bufSize, struct List *itemsList)
{
  struct Node *itemNode;
  struct mediaServerInfo *itemData = NULL;
  itemData = (struct mediaServerInfo *)IExec->AllocVecTags(sizeof(struct mediaServerInfo),
            AVT_Type,            MEMF_PRIVATE,
            AVT_ClearWithValue,  "\0",
            TAG_DONE);

  STRPTR data = IExec->AllocVecTags(sizeof(char) * bufSize,
        AVT_Type,            MEMF_SHARED,
        AVT_ClearWithValue,  "\0",
        TAG_DONE);

  data = strtok(upnpServerData, "\n");
  //printf("--- %s\n", data);
  if(IUtility->Strnicmp(data, "HTTP/1.1 200 OK", 12) == 0) {
    while((data = strtok(NULL, "\n")) != NULL) {
      if(IUtility->Strnicmp(data, "SERVER: ", 8) == 0) {
        IUtility->Strlcpy(itemData->title, data+8, strlen(data)-8);
        //printf("--- %s\n", data);
        //printf("##### %s ####\n", itemData->title);
      }
      if(IUtility->Strnicmp(data, "USN: ", 5) == 0) {
        IUtility->Strlcpy(itemData->usn, data+5, strlen(data)-5);
        //printf("--- %s\n", data);
        //printf("##### %s ####\n", itemData->usn);
      }
      if(IUtility->Strnicmp(data, "LOCATION: ", 10) == 0) {
        IUtility->Strlcpy(itemData->url, data+10, strlen(data)-10);
        //printf("--- %s\n", data);
        //printf("##### %s ####\n", itemData->url);
      }
      if(IUtility->Strnicmp(data, "ST: ", 4) == 0) {
        IUtility->Strlcpy(itemData->st, data+4, strlen(data)-4);
        //printf("--- %s\n", data);
        //printf("##### %s ####\n", itemData->st);
      }
    }
    //if(!searchUpnpServer(tmpLocation)) {
    //  addUpnpServer(tmpServer, tmpUsn, tmpLocation, tmpSt, TRUE);
    //}

    // TODO: Check if the server already exists in the list, before add it

    itemNode = IListBrowser->AllocListBrowserNode( 2,
        LBNA_UserData,          itemData,
        LBNA_Column,            0,
          LBNCA_CopyText,       TRUE,
          LBNCA_Text,           itemData->title,
        TAG_DONE);

    if(itemNode)
    {
      IExec->AddTail(itemsList, itemNode);
    }
  }

  if(data != NULL) {
    IExec->FreeVec(data);
  }
}



static BOOL discoverUPnPServers(struct List *itemsList)
{
  BOOL success = FALSE;
  LONG socketHandle;

  socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(socketHandle != -1) {
    //IDOS->Printf("DBG: %ld\n", socketHandle);

    struct sockaddr_in ssdpAddr;
    ssdpAddr = socketConnectAton();

    socklen_t addrLen = sizeof(ssdpAddr);
    //IDOS->Printf("DBG: %lu\t%ld\n", addrLen, strlen(DISCOVERMSG));

    LONG reqSent = sendto(socketHandle, (APTR)DISCOVERMSG, strlen(DISCOVERMSG), 0, (struct sockaddr *)&ssdpAddr, addrLen);
    //IDOS->Printf("DBG: reqSent %ld\n", reqSent);
    if (reqSent >= 0)
    {
      fd_set readFds;
      struct timeval timeout;
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;


      //ULONG signals;
      int bufSize = 512;
      STRPTR buf = IExec->AllocVecTags(sizeof(char) * bufSize,
            AVT_Type,            MEMF_SHARED,
            AVT_ClearWithValue,  "\0",
            TAG_DONE);
        
      //struct TimeVal timeout;
      //timeout.Seconds = 5;
      //timeout.Microseconds = 0;
        
      FD_ZERO(&readFds);
      FD_SET(socketHandle, &readFds);
      int reqRecv;
      
      IExec->NewList(itemsList);
      
      //while(ISocket->WaitSelect(socketHandle + 1, &readFds, NULL, NULL, &timeout, &signals) > 0) {
      while(select(socketHandle + 1, &readFds, NULL, NULL, &timeout) > 0) {
        if(FD_ISSET(socketHandle, &readFds)) {
          IUtility->SetMem(buf, '\0', bufSize);
            
          //int reqRecv = ISocket->recvfrom(socketHandle, buf, sizeof(char) * bufSize, 0, (struct sockaddr *)&ssdpAddr, &addrLen);
          //reqRecv = recvfrom(socketHandle, buf, sizeof(char) * bufSize, 0, (struct sockaddr *)&ssdpAddr, &addrLen);
          if ((reqRecv = recvfrom(socketHandle, buf, sizeof(char) * bufSize, 0, (struct sockaddr *)&ssdpAddr, &addrLen)) < 0)
          {
            IDOS->Printf("Socket recvfrom failed!\n");
            break;
          }
            
          //IDOS->Printf("Request Recv: %d\n", reqRecv);
          if(strncmp(buf, "HTTP/1.1 200 OK", 12) != 0)
          {
            IDOS->Printf("SSDP failed!\n");
            break;
          }

          //puts(buf);
          addServerData(buf, bufSize, itemsList);
        }
      }

      //IDOS->Printf("Socket WaitSelect finished!\n");
      //IDOS->Printf("DBG: WaitSelect Signals %ld\n", signals);

      if(buf) {
        success = TRUE;
        IExec->FreeVec(buf);
      }
    }
    else IDOS->Printf("Socket sendto failed!\n");

    shutdown(socketHandle, 2);
  }
  else IDOS->Printf("Socket creation failed!\n");

  return success;
}

static void addListItems(struct Gadget *listbrowser, struct List *list, struct ColumnInfo *colInfo)
{
  // Detach list before modify it
  IIntuition->SetAttrs(listbrowser,
      LISTBROWSER_Labels, NULL,
      TAG_DONE);

  IIntuition->SetGadgetAttrs(listbrowser, windows[WID_MAIN], NULL,
      LISTBROWSER_Labels,         list,
      LISTBROWSER_SortColumn,     0,
      LISTBROWSER_Selected,       -1,
      LISTBROWSER_ColumnInfo,     colInfo,
      TAG_DONE);
}

void fillMediaServersList(void)
{
  char jsonErrorMsg[] = "There was an error with the returned data.";

  if (discoverUPnPServers(&mediaServerList))
  {
    if(listCount(&mediaServerList))
    {
      addListItems((struct Gadget*)gadgets[GID_MEDIA_SERVER_LISTBROWSER], &mediaServerList, mediaServerColInfo);
    }
    else
    {
      char notFoundMsg[] = "No media servers found on your local network!";
      showMsgReq(gadgets[GID_MSG_REQ], "MediaVault info", (char *)notFoundMsg, 0, NULL, 0);
    }
  }
  else showMsgReq(gadgets[GID_MSG_REQ], "MediaVault error", (char *)jsonErrorMsg, 0, NULL, 0);
}
