//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

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
  printf("                        WELCOME TO SHELLDER V 0.5                         \n");
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
    else
    {
      //management of external programs
      if(!(strchr(loadedCommand.programName,'/')!= NULL))
      {
	char* tmp = (char*)(malloc(4096*sizeof(char)));
	
	tmp = strcpy(tmp,programsPath);
	strcat(programsPath,loadedCommand.programName);
	free(loadedCommand.programName);
	loadedCommand.programName = programsPath;
	programsPath = tmp;
      }

      pid_t pid = fork();

      if(pid == 0)
      {
	execvp(loadedCommand.programName,loadedCommand.parameters);
	printf("Program not found\n");
	return 1;
      }

      if(!loadedCommand.background)
      {
	int programStatus;
	wait(&programStatus);
      }
    }
  }

  printf("quitting terminal, killing all child processes\n");
  signal(SIGQUIT, SIG_IGN);
  kill(0,SIGQUIT);
  printf("exit...\n");
  return 0;
}
