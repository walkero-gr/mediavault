
#include "mediavault.h"

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main(void)
{

	OpenLibs();

	showGUI();
  //discoverUPnPServers();
  //freeUpnpServers();
  //getMetadata((char *)"dummy");

  return CleanExit("JustExit");

  //return 0;
}
