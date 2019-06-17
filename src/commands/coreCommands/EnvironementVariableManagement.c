//system library include
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void setEnv(int argc, char** argv)
{
  if(argc<3)
  {
    printf("Usage : setenv [variable name] [value to set]\n");
  }
  else
  {
    if( setenv(argv[1],argv[2],0)==0)
    {
      printf("environement variable %s set with value : %s\n",argv[1],getenv(argv[1]));
    }
    else
    {
      printf("environement variable setting failed\n");
    }
  }
}

void getEnv(int argc, char** argv)
{
  
  if(argc<2)
  {
    printf("Usage : getenv [variable to display]\n");
  }
  else
  {
    printf("%s = %s\n",argv[1],getenv(argv[1]));
  }
}
