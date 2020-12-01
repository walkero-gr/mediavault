/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: upnpfuncs.c
**
** Date: 01-12-2020 22:28:08
**
************************************************************

*/

#include "upnpfuncs.h"

struct upnpServer {
  char server[128];
  char usn[128];
  char location[128];
  char st[64];
  struct upnpServer *next;
};
typedef struct upnpServer *upnpServerPtr;
upnpServerPtr upnpServerHead = NULL;
upnpServerPtr upnpServerCurr = NULL;


int isUpnpServerEmpty()
{
  return upnpServerHead == NULL;
}

struct upnpServer *newUpnpServerList(
  char *serverPtr,
  char *usnPtr,
  char *locationPtr,
  char *stPtr
) {
  printf("\n creating list with headnode as [%s ]\n", locationPtr);
  upnpServerPtr item = (upnpServerPtr)malloc(sizeof(struct upnpServer));

  if(item == NULL) {
    die("\n Node creation failed \n");
  }
  Strlcpy(item->server, serverPtr, sizeof(item->server));
  Strlcpy(item->usn, usnPtr, sizeof(item->usn));
  Strlcpy(item->location, locationPtr, sizeof(item->location));
  Strlcpy(item->st, stPtr, sizeof(item->st));
  item->next = NULL;

  upnpServerHead = upnpServerCurr = item;
  return item;
}

struct upnpServer *searchUpnpServer(char *locationPtr)
{
  struct upnpServer *ptr = upnpServerHead;

  printf("\n Searching the list for value [%s] \n", locationPtr);

  while(ptr != NULL) {
    if(strcmp(ptr->location, locationPtr) == 0) {
      return ptr;
    } else {
      ptr = ptr->next;
    }
  }

  return NULL;
}

struct upnpServer *addUpnpServer(
  char *serverPtr,
  char *usnPtr,
  char *locationPtr,
  char *stPtr,
  BOOL add_to_end
) {
  if(isUpnpServerEmpty()) {
    return (newUpnpServerList(serverPtr, usnPtr, locationPtr, stPtr));
  }

  if(add_to_end)
    printf("\n Adding node to end of list with value [%s]\n", locationPtr);
  else
    printf("\n Adding node to beginning of list with value [%s]\n", locationPtr);

  upnpServerPtr item = (upnpServerPtr)malloc(sizeof(struct upnpServer));
  if(item == NULL) {
    die("\n Node creation failed \n");
  }
  Strlcpy(item->server, serverPtr, sizeof(item->server));
  Strlcpy(item->usn, usnPtr, sizeof(item->usn));
  Strlcpy(item->location, locationPtr, sizeof(item->location));
  Strlcpy(item->st, stPtr, sizeof(item->st));
  item->next = NULL;

  if(add_to_end) {
    upnpServerCurr->next = item;
    upnpServerCurr = item;
  } else {
    item->next = upnpServerHead;
    upnpServerHead = item;
  }
  return item;
}

void printUpnpServers( void )
{
  upnpServerPtr currentPtr = upnpServerHead;
  while (currentPtr != NULL) {
    printf("%s\n", currentPtr->server);
    currentPtr = currentPtr->next;
  }
  printf("\n=========================================================\n");
}

void parseUpnpServerData(char *upnpServerData)
{
  char *data;
  char tmpServer[128] = "";
  char tmpUsn[128] = "";
  char tmpLocation[128] = "";
  char tmpSt[64] = "";

  data = strtok(upnpServerData, "\n");
  printf("--- %s\n", data);
  if(Strnicmp(data, "HTTP/1.1 200 OK", 12) == 0) {
    while((data = strtok(NULL, "\n")) != NULL) { 
      if(Strnicmp(data, "SERVER: ", 8) == 0) {
        Strlcpy(tmpServer, data+8, strlen(data)-8);
        printf("--- %s\n", data);
        printf("##### %s ####\n", tmpServer);
      }
      if(Strnicmp(data, "USN: ", 5) == 0) {
        Strlcpy(tmpUsn, data+5, strlen(data)-5);
        printf("--- %s\n", data);
        printf("##### %s ####\n", tmpUsn);
      }
      if(Strnicmp(data, "LOCATION: ", 10) == 0) {
        Strlcpy(tmpLocation, data+10, strlen(data)-10);
        printf("--- %s\n", data);
        printf("##### %s ####\n", tmpLocation);
      }
      if(Strnicmp(data, "ST: ", 4) == 0) {
        Strlcpy(tmpSt, data+4, strlen(data)-4);
        printf("--- %s\n", data);
        printf("##### %s ####\n", tmpSt);
      }
    }
    if(!searchUpnpServer(tmpLocation)) {
      addUpnpServer(tmpServer, tmpUsn, tmpLocation, tmpSt, TRUE);
    }
  }
  //puts(upnpServerData);

}

void discoverUPnPServers( void )
{
    int socketHandle;
    struct sockaddr_in ssdpAddr;
    unsigned int addrLen;
    char buf[BUFLEN];
    int reqRecv;
    const char *reqMsg = "M-SEARCH * HTTP/1.1\r\n"\
                          "HOST:239.255.255.250:1900\r\n"\
                          "ST:upnp:rootdevice\r\nMX:2\r\n"\
                          "MX: 3\r\n"\
                          "MAN:\"ssdp:discover\"\r\n"\
                          "\r\n";
    fd_set readFds;
    struct timeval timeout;


    if ((socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("Socket error");
    }

    memset((char *) &ssdpAddr, 0, sizeof(ssdpAddr));
    ssdpAddr.sin_family = AF_INET ;
    ssdpAddr.sin_port = htons(SSDPPORT);
    
    if (inet_aton(SSDPADDR, &ssdpAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(EXIT_FAILURE);
    }
    
    addrLen=sizeof(ssdpAddr);
    
    if (sendto(socketHandle, reqMsg, strlen(reqMsg), 0, (struct sockaddr*)&ssdpAddr, addrLen) == -1)
    {
        die("sendto()");
    }
    
    
    FD_ZERO(&readFds);
    FD_SET(socketHandle, &readFds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while(select(socketHandle + 1, &readFds, NULL, NULL, &timeout) >0) 
    {
        if(FD_ISSET(socketHandle, &readFds))
        {
            memset(buf, '\0', BUFLEN);
            if ((reqRecv = recvfrom(socketHandle, buf, BUFLEN, 0, (struct sockaddr*)&ssdpAddr, &addrLen)) < 0)
            {
                die("recvfrom()");
            }
            
            if(strncmp(buf, "HTTP/1.1 200 OK", 12) != 0)
            {
                die("err: ssdp failed");
            }
            
            printf("Request Recv:%d\n", reqRecv);
            // puts(buf);
            parseUpnpServerData(buf);
        }
    }

  printf("#################################################################\n");
  printUpnpServers();
}
