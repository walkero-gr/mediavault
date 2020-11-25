#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFLEN 512
#define SSDPADDR "239.255.255.250"
#define SSDPPORT 1900

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main ( void ) {
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
	    	// if ((reqRecv = recv(socketHandle, buf, BUFLEN, 0)) < 0)
		    {
		        die("recvfrom()");
		    }
		    
		    if(strncmp(buf, "HTTP/1.1 200 OK", 12) != 0)
		    {
		        die("err: ssdp failed");
		    }
		    
		    printf("Request Recv:%d\n", reqRecv);
		    puts(buf);
		    free(buf);
		}
    }
    
    return 0 ;
}
