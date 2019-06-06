//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//local library include
#include "entryManagement.h"

void promptDisplay()
{

  char* hostName = (char*)(malloc(32*sizeof(char)));
  size_t hostSize = 32;

  char* currentPath = (char*)(malloc(4096*sizeof(char)));
  size_t pathSize = 4096 ;
  
  gethostname(hostName,hostSize);
  getcwd(currentPath,pathSize);
  
  printf("%s@%s : %s >> ",getlogin(),hostName,currentPath); 

}

CommandEntry readCommand()
{
  char buffer[10];

  char* finalCommand = malloc(sizeof(char));
  finalCommand[0] = '\0';
  
  int readed;
  do
  {
    fgets(buffer,10,stdin);
    readed = strlen(buffer);
    
    if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen(buffer)-2] != '\\')
    {
      buffer[strlen(buffer)-1] = '\0';
    }
    else if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen(buffer)-2] == '\\')
    {
      buffer[strlen(buffer)-2] = ' ';
      buffer[strlen(buffer)-1] = '\0';
      readed = strlen(buffer);
    }

    finalCommand = realloc((void*)finalCommand,(strlen(finalCommand)+strlen(buffer))*sizeof(char));
    strcat(finalCommand,buffer);
    
  }while(readed == strlen(buffer));

  char* programName = strtok(finalCommand," ");
  
  char* newParameter;
  char** parameters = malloc(sizeof(char*));
  int parameterCount = 0;
    
  while((newParameter = strtok(NULL," ")) != NULL)
  {
    parameters[parameterCount] = newParameter;
    parameterCount ++;
    parameters = realloc((void*)parameters,(parameterCount+1)*sizeof(char*));
  }

  struct CommandEntry buildedCommand;
  buildedCommand.programName = programName;
  buildedCommand.parameters = parameters;
  buildedCommand.parameterCount = parameterCount;
  
  return buildedCommand;

}

