#ifndef _entryManagement_h
#define _entryManagement_h

typedef struct CommandEntry
{
  char* programName;
  char** parameters;
  int parameterCount;
} CommandEntry ;
  
void promptDisplay();

CommandEntry readCommand();

#endif
