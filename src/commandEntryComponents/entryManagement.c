//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

int checkEOF()
{  
    if(feof(stdin))
    {
      return 1;
    }

    int c = getc(stdin);

    if(c == EOF)
    {
      return 1;
    }
    ungetc(c, stdin);
    return 0;
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
    if(checkEOF())
    {
      struct CommandEntry exitEntry;
      exitEntry.programName = "exit";
      return exitEntry;
    }
    
    fgets(buffer,10,stdin);
    readed = strlen(buffer);

    if(readed > 0)
    {
      if(buffer[strlen(buffer)-1] == '\n' && strlen(buffer)== 1)
      {
	struct CommandEntry nullEntry;
	nullEntry.programName = "null";
	return nullEntry;
      }
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

  // extract commands between pipes
  struct CommandEntry** cmds = (struct CommandEntry**) malloc(sizeof(struct CommandEntry*));
  int nbCmd = 0;
  
  const char delim[2] = "|";
  char* saveptr;
  char* token = strtok_r(finalCommand, delim, &saveptr);

  while(token != NULL) {
    cmds = (struct CommandEntry**) realloc(cmds, (nbCmd+1)*sizeof(struct CommandEntry*));
    cmds[nbCmd] = (struct CommandEntry*) malloc(sizeof(struct CommandEntry*));

    token = trim(token);

    struct CommandEntry pipedCommand = buildCommand(token);
    cmds[nbCmd] = &pipedCommand;

    if(nbCmd == 0) {
      pipedCommand.piped = false;
    }else{
      pipedCommand.piped = true;
      pipedCommand.pipedCommand = cmds[nbCmd-1];
    }

    token = strtok_r(NULL, delim, &saveptr);
    nbCmd += 1; 
  }

  cmds[0]->background = background;

  return *cmds[0];
}

struct CommandEntry buildCommand(char* inputManaged)
{
  struct CommandEntry buildedCommand;

  //pipes between program management

  //external redirection pipes management

  buildedCommand.inputRedirected = false;
  buildedCommand.outputRedirected = false;
  buildedCommand.errorRedirected = false;


  //redirection management from initial string
  buildedCommand.inputRedirected = getRedirectInput(&inputManaged,&buildedCommand.IRedirectionPath);

  buildedCommand.outputRedirected = getRedirectOutput(&inputManaged,&buildedCommand.ORedirectionPath);

  if(!buildedCommand.inputRedirected)
  {
    buildedCommand.inputRedirected = getRedirectInput(&inputManaged,&buildedCommand.IRedirectionPath);
  }

  buildedCommand.errorRedirected = getRedirectError(&inputManaged,&buildedCommand.ERedirectionPath);

  if(!buildedCommand.inputRedirected)
  {
    buildedCommand.inputRedirected = getRedirectInput(&inputManaged,&buildedCommand.IRedirectionPath);
  }

  if(!buildedCommand.outputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&inputManaged,&buildedCommand.ORedirectionPath);
  }

  //redirection management from input
  if(!buildedCommand.outputRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.IRedirectionPath,&buildedCommand.ORedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.IRedirectionPath,&buildedCommand.ERedirectionPath);
  }

  //redirection management from output
  if(!buildedCommand.inputRedirected && buildedCommand.outputRedirected)
  {
    buildedCommand.inputRedirected = getRedirectInput(&buildedCommand.ORedirectionPath,&buildedCommand.IRedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.outputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.ORedirectionPath,&buildedCommand.ERedirectionPath);
  }


  //parsing input again in case of input discover in the output
  if(!buildedCommand.outputRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.IRedirectionPath,&buildedCommand.ORedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.IRedirectionPath,&buildedCommand.ERedirectionPath);
  }

  //parsing the error field
  if(!buildedCommand.inputRedirected && buildedCommand.errorRedirected)
  {
    buildedCommand.inputRedirected = getRedirectInput(&buildedCommand.ERedirectionPath,&buildedCommand.IRedirectionPath);
  }

  if(!buildedCommand.outputRedirected && buildedCommand.errorRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.ERedirectionPath,&buildedCommand.ORedirectionPath);
  }

  //parsing input and output again if needed.

  //redirection management from input
  if(!buildedCommand.outputRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.outputRedirected = getRedirectOutput(&buildedCommand.IRedirectionPath,&buildedCommand.ORedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.inputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.IRedirectionPath,&buildedCommand.ERedirectionPath);
  }

  //redirection management from output
  if(!buildedCommand.inputRedirected && buildedCommand.outputRedirected)
  {
    buildedCommand.inputRedirected = getRedirectInput(&buildedCommand.ORedirectionPath,&buildedCommand.IRedirectionPath);
  }

  if(!buildedCommand.errorRedirected && buildedCommand.outputRedirected)
  {
    buildedCommand.errorRedirected = getRedirectError(&buildedCommand.ORedirectionPath,&buildedCommand.ERedirectionPath);
  }

  char* saveptr;
  char* programName = strtok_r(inputManaged, " ", &saveptr);

  char** parameters = (char**)(malloc(sizeof(char*)));

  parameters[0] = (char*)(malloc(strlen(programName)*sizeof(char)));

  int parameterCount = 0;

  strcpy(parameters[parameterCount],programName);
  parameterCount ++;
  parameters = realloc((void*)parameters,(parameterCount+1)*sizeof(char*));

  char* newParameter;

  while((newParameter = strtok_r(NULL, " ", &saveptr)) != NULL)
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

      while(((newParameter = strtok_r(NULL, " ", &saveptr)) != NULL) && (strchr(newParameter,'"') == NULL))
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

char *ltrim(char *str) {
    int totrim = strspn(str, "\t\n\v\f\r ");

    if (totrim > 0) {
        int len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }

    return str;
}

char *rtrim(char *str) {
    int i = strlen(str) - 1;

    while (i >= 0 && strchr("\t\n\v\f\r ", str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }

    return str;
}

char *trim(char *str) {
    return ltrim(rtrim(str));
}
