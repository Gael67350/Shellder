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
