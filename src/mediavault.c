
#include "mediavault.h"

extern void discoverUPnPServers( void );

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  discoverUPnPServers();
    
  return 0 ;
}
