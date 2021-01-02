
#include "mediavault.h"

extern void discoverUPnPServers( void );
extern void freeUpnpServers( void );
extern void getMetadata( char *location );

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  discoverUPnPServers();
  freeUpnpServers();
  //getMetadata((char *)"dummy");
  return 0 ;
}
