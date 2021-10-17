
/* ANSI C */
#include <stdio.h>
#include <stdlib.h>

#include "libshandler.h"
#include "gui.h"
#include "sqldb.h"
#include "fsfuncs.h"
#include "mediavault.h"

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main(void)
{
  int libsOpened;
  libsOpened = OpenLibs();

  if (libsOpened)
  {
    return libsOpened;
  }

  if (!fileExists((STRPTR)"PROGDIR:data1.db"))
  {
    // TODO: Check if this was succesful and show an error message or disable the db features
    createDB();
  }

  showGUI();
  //discoverUPnPServers();
  //freeUpnpServers();
  //getMetadata((char *)"dummy");

  return CleanExit("JustExit");
}
