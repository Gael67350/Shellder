//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>

//std library include
#include <stdbool.h>

//local library include
#include "commandEntryComponents/entryManagement.h"
#include "commands/coreCommands/coreCommands.h"

int main(int argc, char**argv)
{

  //building path storage
  char* programsPath = (char*)(malloc(4096*sizeof(char)));
  size_t pathLength = 0 ;
  getcwd(programsPath,pathLength);
  strcat(programsPath,"/commands/external/");

  printf("%s\n",programsPath);

  
  printf("====================================\n");
  printf("     WELCOME TO SHELLDER V 0.1      \n");
  printf("====================================\n");

  //display shellder here

  bool exitPerformed = false;

  while(!exitPerformed)
  {
    struct CommandEntry loadedCommand;
    
    promptDisplay();
    loadedCommand = readCommand();

    if(strcmp(loadedCommand.programName,"cd")==0)
    {
      cd(loadedCommand.parameterCount, loadedCommand.parameters); 
    }
    else if(strcmp(loadedCommand.programName,"exit")==0)
    {
      exitPerformed = true;
    }
    else
    {
      //management of external programs
      if(!(strchr(loadedCommand.programName,'/')!= NULL))
      {
	//addition of the path directory header
	//loadedCommand.programName = strcat("")
      }
    }
  }

  printf("quitting terminal, killing all child processes\n");
  signal(SIGQUIT, SIG_IGN);
  kill(0,SIGQUIT);
  printf("exit...\n");
  return 0;
}
