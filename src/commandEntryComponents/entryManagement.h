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
  bool inputPiped;
  bool outputPiped;

  char* pipePath;
  
} CommandEntry ;
  
void promptDisplay();

CommandEntry readCommand();

#endif
