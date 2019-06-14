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

  bool background = false;
  
  char* finalCommand = malloc(sizeof(char));
  finalCommand[0] = '\0';
  
  int readed;
  do
  {
    fgets(buffer,10,stdin);
    readed = strlen(buffer);

    if(readed > 0)
    {
      if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen(buffer)-2] != '\\' && buffer[strlen(buffer)-2] != '&')
      {
	buffer[strlen(buffer)-1] = '\0';
      }
      else if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen(buffer)-2] == '\\')
      {
	buffer[strlen(buffer)-2] = ' ';
	buffer[strlen(buffer)-1] = '\0';
	readed = strlen(buffer);
      }
      else if(buffer[strlen(buffer)-1] == '\n' && buffer[strlen(buffer)-2] == '&')
      {
	buffer[strlen(buffer)-2] = ' ';
	buffer[strlen(buffer)-1] = '\0';
	background = true;
      }

      finalCommand = realloc((void*)finalCommand,(strlen(finalCommand)+strlen(buffer))*sizeof(char));
      strcat(finalCommand,buffer);
    }
  }while(readed == strlen(buffer));
 
  struct CommandEntry buildedCommand;
  
  //pipes between program management
  
  //external redirection pipes management

  buildedCommand.inputRedirected = false;
  buildedCommand.outputRedirected = false;
  buildedCommand.errorRedirected = false;


  //input redirection management
  buildedCommand.inputRedirected = getRedirectInput(&finalCommand,&buildedCommand.IRedirectionPath);
  
  //output redirection management
  buildedCommand.outputRedirected = getRedirectOutput(&finalCommand,&buildedCommand.ORedirectionPath);
  
  if(!buildedCommand.outputRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.IRedirectionPath,&buildedCommand.ORedirectionPath);
  }

  if(!buildedCommand.inputRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.IRedirectionPath,&buildedCommand.ORedirectionPath);
  }

  //error redirection management
  buildedCommand.errorRedirected = getRedirectError(&finalCommand,&buildedCommand.ERedirectionPath);

  if(!buildedCommand.errorRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.IRedirectionPath,&buildedCommand.ERedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.outputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.ORedirectionPath,&buildedCommand.ERedirectionPath);
  }
  
  char* programName = strtok(finalCommand," ");

  char** parameters = (char**)(malloc(sizeof(char*)));

  parameters[0] = (char*)(malloc(strlen(programName)*sizeof(char)));
  
  int parameterCount = 0;
  
  strcpy(parameters[parameterCount],programName);
  parameterCount ++;
  parameters = realloc((void*)parameters,(parameterCount+1)*sizeof(char*));
  
  char* newParameter;
    
  while((newParameter = strtok(NULL," ")) != NULL)
  {

    if(strchr(newParameter,'"') == NULL)
    {
      parameters[parameterCount] = (char*)(malloc(strlen(newParameter)*sizeof(char)));
      strcpy(parameters[parameterCount],newParameter);
    }
    else
    {
      parameters[parameterCount] = (char*)(malloc((strlen(newParameter))*sizeof(char)));
      strcpy(parameters[parameterCount],++newParameter);
      strcat(parameters[parameterCount]," ");
      
      while(((newParameter = strtok(NULL," ")) != NULL) && (strchr(newParameter,'"') == NULL))
      {
	parameters[parameterCount] = realloc((void*)parameters[parameterCount],(strlen(parameters[parameterCount])+strlen(newParameter)+1)*sizeof(char));
	strcat(parameters[parameterCount],newParameter);
	strcat(parameters[parameterCount]," ");
      }

      parameters[parameterCount] = realloc((void*)parameters[parameterCount],(strlen(parameters[parameterCount])+strlen(newParameter))*sizeof(char));
      strcat(parameters[parameterCount],newParameter);
      parameters[parameterCount][strlen(parameters[parameterCount])-1]='\0';
      
    }
    
    parameterCount ++;
    parameters = realloc((void*)parameters,(parameterCount+1)*sizeof(char*));
  }
  parameters[parameterCount] = NULL;
  
  buildedCommand.programName = programName;
  buildedCommand.parameters = parameters;
  buildedCommand.parameterCount = parameterCount;

  buildedCommand.background = background;
  
  return buildedCommand;

}

bool getRedirectInput(char** entry, char** pathFound)
{
  char* tmp = strrchr(*entry,'<');
   
  if(tmp!=NULL)
  {   
    *tmp = '\0';

    tmp += 1;
    if(*tmp == ' ')
      tmp += 1;
    
    *pathFound = tmp;     
     return true;
   }
   else
   {
     return false;
   }
}

bool getRedirectError(char** entry, char** pathFound)
{
  char* tmp = strrchr(*entry,'>');
  
  if(tmp !=NULL)
  {
    tmp -= 1;
    if(*tmp == '2')
    {;
      *tmp = '\0';

      tmp+=2;
      if(*tmp == ' ')
	tmp += 1;
    
      *pathFound = tmp;

      return true;
    }
  }
    
  return false;
  
}

bool getRedirectOutput(char** entry,char** pathFound)
{

  char* tmp = strrchr(*entry,'>');

  if(tmp != NULL)
  {
    tmp-=1;

    if(*tmp != '2')
    {

      tmp +=1;
      *tmp = '\0';

      tmp += 1;
      if(*tmp == ' ')
	tmp += 1;
    
      *pathFound = tmp;
      
      return true;
    }
  }
    
  return false;
}
