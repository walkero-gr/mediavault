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

struct metadata {
  char friendlyName[128];
};


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
  //STRPTR data = malloc(BUFLEN * sizeof(char));
  STRPTR data = AllocVecTags(sizeof(char) * BUFLEN,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
    TAG_DONE);
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
  if(data != NULL) {
    FreeVec(data);
  }
}

void getMetadata( char *location )
{
  // http://192.168.0.92:8096/dlna/83cc3fd68b6b4dafa611971bdc8a2914/description.xml
  // http://192.168.0.90:9000/TMSDeviceDescription.xml
  // http://192.168.0.90:32469/DeviceDescription.xml
  // http://192.168.0.1:5000/rootDesc.xml
  int socketHandle;
  int reqRecv;
  unsigned int addrLen;
  long conn;
  //char *xmlResponse;
  STRPTR xmlResponse = AllocVecTags(sizeof(char) * 1024 * 5,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
    TAG_DONE);

  //char buf[BUFLEN];
  //char *buf;
  STRPTR buf = AllocVecTags(sizeof(char) * BUFLEN,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
    TAG_DONE);

  struct sockaddr_in ssdpAddr;
  const char reqMsg[] = "GET /dlna/83cc3fd68b6b4dafa611971bdc8a2914/description.xml HTTP/1.1\r\nHost: 192.168.0.92\r\n\r\n";
  fd_set readFds;
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;


  if ((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1) {
    die("Socket error");
  }

  //memset((char *) &ssdpAddr, 0, sizeof(ssdpAddr));
  FD_ZERO(&ssdpAddr); // This is a memset to zero
  ssdpAddr.sin_addr.s_addr = inet_addr("192.168.0.92");
  ssdpAddr.sin_family = AF_INET ;
  ssdpAddr.sin_port = htons(8096);
    
  //if (inet_aton("192.168.0.92", &ssdpAddr.sin_addr) == 0) {
  //  fprintf(stderr, "inet_aton() failed\n");
  //  exit(EXIT_FAILURE);
  //}
    
  addrLen = sizeof(ssdpAddr);
  if((conn = connect(socketHandle, (struct sockaddr*)&ssdpAddr, addrLen)) == -1) {
    die("Connect failed: ");
  }
  
  //if (sendto(socketHandle, reqMsg, strlen(reqMsg), 0, (struct sockaddr*)&ssdpAddr, addrLen) == -1) {
  if (send(socketHandle, reqMsg, strlen(reqMsg), 0) == -1) {
    die("send() failed");
  }
    
  FD_ZERO(&readFds);
  FD_SET(socketHandle, &readFds);
//  strcpy(xmlResponse, ".");
  while(select(socketHandle + 1, &readFds, NULL, NULL, &timeout) > 0) {
    if(FD_ISSET(socketHandle, &readFds)) {
      //memset(buf, '\0', BUFLEN);
      if ((reqRecv = recv(socketHandle, buf, BUFLEN, 0)) < 0)
      {
        die("recv() failed");
      }
      
      //if(strncmp(buf, "HTTP/1.1 200 OK", 12) != 0)
      //{
      //  die("err: ssdp failed");
      // }
      
      printf("Request Recv:%d\n", reqRecv);
      //puts(buf);
      Strlcat(xmlResponse, buf, sizeof(xmlResponse));
      //strcat(xmlResponse, buf);
    }
  }



  printf("%s\n", xmlResponse);
  //puts(xmlResponse);
  shutdown(socketHandle, 2);
}

void addUPnPMetadata()
{
  printf("#################################################################\n");
  upnpServerPtr currentPtr = upnpServerHead;
  while (currentPtr != NULL) {
    printf("GET Metadata for %s\n", currentPtr->server);
    printf("Retrieve XML from %s\n", currentPtr->location);
    getMetadata(currentPtr->location);
    currentPtr = currentPtr->next;
  }
}

int socketCreateUDP(void)
{
  int hSocket;
  if ((hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    die("Socket error");
  }
  return hSocket;
}

struct sockaddr_in
socketConnectAton(int hsocket, const char *address, int port)
{
  struct sockaddr_in ssdpAddr;
  memset((char *) &ssdpAddr, 0, sizeof(ssdpAddr));

  ssdpAddr.sin_family = AF_INET ;
  ssdpAddr.sin_port = htons(port);

  if (inet_aton(address, &ssdpAddr.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(EXIT_FAILURE);
  }

  return ssdpAddr;
}

void discoverUPnPServers( void )
{
  int socketHandle;
  int reqRecv;
  unsigned int addrLen;

  STRPTR buf = AllocVecTags(sizeof(char) * BUFLEN,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
    TAG_DONE);

  struct sockaddr_in ssdpAddr;

  fd_set readFds;
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  
  socketHandle = socketCreateUDP();

  ssdpAddr = socketConnectAton(socketHandle, SSDPADDR, SSDPPORT);

  addrLen = sizeof(ssdpAddr);
  if (sendto(socketHandle, discoverMsg, strlen(discoverMsg), 0, (struct sockaddr*)&ssdpAddr, addrLen) == -1) {
    die("sendto()");
  }
    
  FD_ZERO(&readFds);
  FD_SET(socketHandle, &readFds);
  while(select(socketHandle + 1, &readFds, NULL, NULL, &timeout) >0) {
    if(FD_ISSET(socketHandle, &readFds)) {
      memset(buf, '\0', BUFLEN);
      if ((reqRecv = recvfrom(socketHandle, buf, BUFLEN, 0, (struct sockaddr*)&ssdpAddr, &addrLen)) < 0)
      {
        die("recvfrom() failed");
      }
      
      if(strncmp(buf, "HTTP/1.1 200 OK", 12) != 0)
      {
        die("err: ssdp failed");
      }
      
      printf("Request Recv:%d\n", reqRecv);
      //puts(buf);
      parseUpnpServerData(buf);
    }
  }

  printf("#################################################################\n");
  printUpnpServers();
  //addUPnPMetadata();
  if(buf != NULL) {
    FreeVec(buf);
  }
  shutdown(socketHandle, 2);
}

void freeUpnpServers( void )
{
  if(upnpServerCurr != NULL) {
    free(upnpServerCurr);
  }
  if(upnpServerHead != NULL) {
    free(upnpServerHead);
  }
}
