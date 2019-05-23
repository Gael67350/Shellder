//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//local library include
#include "entryManagement.h"

void promptDisplay()
{

  char* hostName = (char*)(malloc(32));
  size_t hostSize = 32;

  char* currentPath = (char*)(malloc(4096));
  size_t pathSize = 4096 ;
  
  gethostname(hostName,hostSize);
  getcwd(currentPath,pathSize);
  
  printf("%s@%s : %s >>",getlogin(),hostName,currentPath); 

}

CommandEntry readCommand()
{
  char buffer[10];

  char* finalCommand = malloc(sizeof(char));

  printf(">>> ");

    do
    {
      fgets(buffer,10,stdin);
      if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen-2] != '\\')
      {
	buffer[strlen(buffer-1)] = '\0';
      }
      fgets(buffer,10,stdin);
      if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen-2] != '\\')
      {
	buffer[strlen(buffer-1)] = '\0';
      }
      
      finalCommand = realloc((void*)finalCommand,strlen(finalCommand)*sizeof(char)+strlen(buffer)*sizeof(char));

      strcat(finalCommand,buffer);

    }while(buffer[strlen(buffer)-1] != '\0');
    
    printf("%s",finalCommand);
    
    char* programName = strtok(finalCommand,' ');
    
    char* newParameter;
    char** parameters = malloc(1*sizeof(char*));
    int parameterCount = 0;
    
    while((newParameter = strtok(NULL,' ')) != NULL)
    {
      parameterCount ++;
      printf(newParameter);
    }

    free(finalCommand);
}
