
#include <sys/types.h>
#include <proto/socket.h>
// #include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  int socket_handle ;
  struct sockaddr_in socket_details ;
  char * input_buffer;
  char * httpget = "GET HTTP 1.1 / \x0D\x0A\n\x0D\x0A\n" ;

  input_buffer = malloc(200000);

  socket_handle = socket ( AF_INET, SOCK_STREAM, 0) ;
  socket_details.sin_family = AF_INET ;
  socket_details.sin_addr.s_addr=inet_addr("192.168.0.90");
  socket_details.sin_port = htons(80);
  // bzero ( &(socket_details.sin_zero), 8 ) ;

  if ( connect (socket_handle,(struct sockaddr*)&socket_details, sizeof ( struct sockaddr)) == -1 ){
      printf ( "Couldnt connect to server\n" ) ;
  }
  printf ( "Sending %d bytes\n", send ( socket_handle , httpget, strlen(httpget), 0 ) ) ;
  printf ( "Received %d bytes\n", recv ( socket_handle , input_buffer , 200000, 0 ) ) ;
  printf ( "%s\n", input_buffer ) ;

  return 0;
}
