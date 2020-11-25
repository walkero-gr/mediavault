// gcc -D__USE_INLINE__ httpget.c -o httpget
// vc +aosppc -I$AOS4_SDK_INC -I$AOS4_NET_INC -I$AOS4_NLIB_INC -I$AMISSL_INC -D__USE_INLINE__ -lauto httpget.c -o httpget
// vc +aosppc -I$AOS4_SDK_INC -I$AOS4_NET_INC -I$AOS4_NLIB_INC -D__USE_INLINE__ -lauto httpget.c -o httpget

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main ( void ) {
    int socket_handle ;
    struct sockaddr_in socket_detials ;
    char * input_buffer;
    char * httpget = "GET HTTP 1.1 / \x0D\x0A\n\x0D\x0A\n" ;

    input_buffer = malloc(20000);

    socket_handle = socket ( AF_INET, SOCK_STREAM, 0) ;
    socket_detials.sin_family = AF_INET ;
    socket_detials.sin_addr.s_addr=inet_addr("68.90.68.66");
    socket_detials.sin_port = htons(80);
    bzero ( &(socket_detials.sin_zero), 8 ) ;

    if ( connect (socket_handle,(struct sockaddr*)&socket_detials, sizeof ( struct sockaddr)) == -1 ){
        printf ( "Couldnt connect to server\n" ) ;
    }
    printf ( "Sending %d bytes\n", send ( socket_handle , httpget, strlen(httpget), 0 ) ) ;
    printf ( "Received %d bytes\n", recv ( socket_handle , input_buffer , 20000, 0 ) ) ;
    printf ( "%s\n", input_buffer ) ;

    return 0 ;
}
