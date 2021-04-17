
#include "mediavault.h"

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

	OpenLibs();

	showGUI();
  //discoverUPnPServers();
  //freeUpnpServers();
  //getMetadata((char *)"dummy");

  CleanExit("JustExit");

  return 0 ;
}
