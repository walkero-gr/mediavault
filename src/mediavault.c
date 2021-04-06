
#include "mediavault.h"

extern void discoverUPnPServers( void );
extern void freeUpnpServers( void );
extern void getMetadata( char *location );

extern void OpenLibs(void);
extern void CleanExit(const char *str);

extern void showGUI(void);

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
