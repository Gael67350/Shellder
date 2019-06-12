#ifndef _entryManagement_h
#define _entryManagement_h

//std library include
#include <stdbool.h>

typedef struct CommandEntry
{
  char* programName;
  char** parameters;
  int parameterCount;
  bool background;

  //pipingManagement
  bool inputRedirected;
  bool outputRedirected;
  bool errorRedirected;

  char* IRedirectionPath;
  char* ORedirectionPath;
  char* ERedirectionPath;
  
} CommandEntry ;
  
void promptDisplay();

CommandEntry readCommand();

#endif
