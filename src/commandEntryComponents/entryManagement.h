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

bool getRedirectInput(char** entry, char** pathFound);
bool getRedirectError(char** entry, char** pathFound);
bool getRedirectOutput(char** entry,char** pathFound);

#endif
