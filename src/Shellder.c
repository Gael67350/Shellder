//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>

//library inclusion for pipes management
#include <memory.h>

//std library include
#include <stdbool.h>

//local library include
#include "commandEntryComponents/entryManagement.h"
#include "commands/coreCommands/coreCommands.h"

int main(int argc, char**argv)
{

  //building path storage
  char* programsPath = (char*)(malloc(4096*sizeof(char)));
  size_t pathLength = 4096 ;
  getcwd(programsPath,pathLength);
  strcat(programsPath,"/commands/external/");

  //splash screen
  
  printf("==========================================================================\n");
  printf("                        WELCOME TO SHELLDER V 1.0                         \n");
  printf("==========================================================================\n\n");

  printf("               _,.-'\"\"\"''--..._\n");
  printf("           _,-'               `.\"-..\n");
  printf("         .\"     _..-'\"'\"\"--._   `.   `-._\n");
  printf("       ,'   _.-'             `._  `.     `-._\n");
  printf("      /   .`                    `.  \\        `.\n");
  printf("     /  .'                        `. `.        `.       _,..\n");
  printf("   .'  /                            `. `.        `...-\"'    \\\n");
  printf("  /   /                               _. `.               ,-'\n");
  printf(" j   /                   ,-\"\"'`.   ,\"'  `. `.           .'\n");
  printf(" |  .     _..------...__'  \"   |  |   \"   |  `.       ,'\n");
  printf(" `._...-'\"_,.-\"'        `..__,\"    `._ _,.'`.  `    .'\\\n");
  printf("   ,\"  _,'             __..-\"\"'`\"'.  ,'    `..  `.     .\n");
  printf(" ,'  .\"        _..-''\"\"            \\/        `.   \\    '\n");
  printf(":         _..+'----\"'               `.         `.  \\    \\\n");
  printf(":      _,'    `-._                               \\  \\    \\\n");
  printf(" `...-'           `.                              \\  \\\n");
  printf("                   \\                              \\  \\   .\n");
  printf("                    \\                              .     '\n");
  printf("                    / \\                    .        '  `  :	 \n");
  printf("                  ,'   \\               .    \\        \\  \\  `-,._\n");
  printf("                ,'    __\\               \\    \\        \\  \\  /._ `.\n");
  printf("               .  _.\"'   \\               \\    \\        `._'/._ \"-.\\\n");
  printf("                `\"        `.              `._.'        ,'.-.. `-._ `\n");
  printf("                            `-._                    _.'.  `,\"`-._ `.`\n");
  printf("                                `--...__     ___..-\"  \\ `. '     `._`|\n");
  printf("                                        `\"'\"\" \\   :    \\  `.`.      \"\n");    
  printf("                                               \\  :     `   `.`.\n");
  printf("                                                '\":      `.__,.'\n");
  printf("                                                  `-.....' mh\n\n");

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
    else if(strcmp(loadedCommand.programName,"setenv") == 0)
    {
      setEnv(loadedCommand.parameterCount, loadedCommand.parameters);
    }
    else if(strcmp(loadedCommand.programName,"getenv") == 0)
    {
      getEnv(loadedCommand.parameterCount, loadedCommand.parameters);
    }
    else if(strcmp(loadedCommand.programName,"null") == 0)
    {
      printf("no command entered\n");
    }
    else
    {
      //management of external programs
      if(strchr(loadedCommand.programName,'/')== NULL)
      {
	char* tmp = (char*)(malloc(4096*sizeof(char)));
	
	tmp = strcpy(tmp,programsPath);
	strcat(programsPath,loadedCommand.programName);
	//free(loadedCommand->programName);
	loadedCommand.programName = programsPath;
	programsPath = tmp;
      }
      
      pid_t pid = fork();

      int pipeFD[2];


      if(loadedCommand.piped)
      {
	pipe(pipeFD);
      }
      
      if(pid == 0)
      {

	if(loadedCommand.inputRedirected)
        {
	  int newInput = open(loadedCommand.IRedirectionPath,O_RDWR | O_CREAT | O_EXCL);
	  if(newInput != -1)
	  {
	    dup2(newInput,fileno(stdin));
	  }
	  else
	  {
	    printf("incorrect or existing file specified, can't perform redirection\n");
	  }
	}

	
	if(loadedCommand.piped)
	{
	  dup2(pipeFD[0],fileno(stdout));
        }
	else if(loadedCommand.outputRedirected)
        {
	  int newOutput = open(loadedCommand.ORedirectionPath,O_RDWR | O_CREAT | O_EXCL);
	  if(newOutput != -1)
	  {
	    dup2(newOutput,fileno(stdout));	    
	  }
	  else
	  {
	    printf("incorrect or existing file specified, can't perform redirection\n");
	  }

	}

	if(loadedCommand.errorRedirected)
        {
	  int newError = open(loadedCommand.ERedirectionPath,O_RDWR | O_CREAT | O_EXCL);
	  if(newError != -1)
	  {
	    dup2(newError,fileno(stderr)); 
	  }
	  else
	  {
	    printf("incorrect or existing file specified, can't perform redirection\n");
	  }

	}
	
	execvp(loadedCommand.programName,loadedCommand.parameters);
	printf("Program not found\n");
	return 1;
      }

      if(!loadedCommand.background)
      {
	int programStatus;
	wait(&programStatus);
      }
      else
      {
	printf("job launched with pid : %d\n",pid);
      }
      
      while(loadedCommand.piped)
      {
	printf("top\n");
	//management of external programs
	if(strchr(loadedCommand.programName,'/')== NULL)
	{
	  char* tmp = (char*)(malloc(4096*sizeof(char)));
	
	  tmp = strcpy(tmp,programsPath);
	  strcat(programsPath,loadedCommand.programName);
	  //free(loadedCommand.programName);
	  loadedCommand.programName = programsPath;
	  programsPath = tmp;
	}
      
	if(pid == 0)
	{

	  dup2(pipeFD[1],fileno(stdin));
	
	  if(loadedCommand.piped)
	  {
	    pipe(pipeFD);
	  }
      
	  if(loadedCommand.piped)
	  {
	    dup2(pipeFD[0],fileno(stdout));
	  }
	  else if(loadedCommand.outputRedirected)
	  {
	    int newOutput = open(loadedCommand.ORedirectionPath,O_RDWR | O_CREAT | O_EXCL);
	    if(newOutput != -1)
	    {
	      dup2(newOutput,fileno(stdout));	    
	    }
	    else
	    {
	      printf("incorrect or existing file specified, can't perform redirection\n");
	    }

	  }

	  if(loadedCommand.errorRedirected)
	  {
	    int newError = open(loadedCommand.ERedirectionPath,O_RDWR | O_CREAT | O_EXCL);
	    if(newError != -1)
	    {
	      dup2(newError,fileno(stderr)); 
	    }
	    else
	    {
	      printf("incorrect or existing file specified, can't perform redirection\n");
	    }
	  }
	
	  execvp(loadedCommand.programName,loadedCommand.parameters);
	  printf("Program not found\n");
	  return 1;
	}
	
	if(!loadedCommand.background)
	{
	  int programStatus;
	  wait(&programStatus);
	}
	else
	{
	  printf("job launched with pid : %d\n",pid);
	}
      }
    }
  }

  printf("\nquitting terminal, killing all child processes\n");
  signal(SIGQUIT, SIG_IGN);
  kill(0,SIGQUIT);
  printf("exit...\n");
  return 0;
}
