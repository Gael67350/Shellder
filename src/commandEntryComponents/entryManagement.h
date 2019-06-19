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

  //fileRedirectionManagement
  bool inputRedirected;
  bool outputRedirected;
  bool errorRedirected;

  char* IRedirectionPath;
  char* ORedirectionPath;
  char* ERedirectionPath;

  //pipe management
  bool piped;
  struct CommandEntry* pipedCommand;
  
} CommandEntry ;
  
void promptDisplay();

CommandEntry readCommand();

bool getRedirectInput(char** entry, char** pathFound);
bool getRedirectError(char** entry, char** pathFound);
bool getRedirectOutput(char** entry,char** pathFound);
struct CommandEntry buildCommand(char* inputManaged);

char *ltrim(char*);
char *rtrim(char*);
char *trim(char *str);

#endif
