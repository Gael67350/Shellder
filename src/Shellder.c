//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//std library include
#include <stdbool.h>

//local library include
#include "commandEntryComponents/entryManagement.h"

int main(int argc, char**argv)
{

  printf("====================================\n");
  printf("     WELCOME TO SHELLDER V 0.1      \n");
  printf("====================================\n");

  display shellder here

  bool exitPerformed = false;

  while(!exitPerformed)
  {
    promptDisplay();
  }
}
