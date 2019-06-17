#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int copyElement(const char* pathSrc , const char* pathDest)
{

  int toCopy = open(pathSrc , O_RDONLY);

  if(toCopy < 0)
  {
    printf("the specified file to copy does not exists \n");
    return 1;
  }

  char buffer[4096] ;
  int readed;

  struct stat fileStat;

  if(stat(pathSrc,&fileStat) < 0)
  {
    printf("error while reading the access right of the element to copy \n");
    return 1;
  }
   

  if(S_ISDIR(fileStat.st_mode))
  {
    if(mkdir(pathDest,fileStat.st_mode) == -1)
    {
      printf("error while creating the copy directory \n");
      return 1;
    }

    DIR* folder;
    if( (folder = fdopendir(toCopy)) == 0)
    {
      printf("error while openning the folder to copy\n");
      return 1;
    }

    //recursive copy of each elements included in the folder

    struct dirent* currentFile;
    
    while((currentFile = readdir(folder)) != 0)
    {
      char fileOriginPathBuffer[10000] = "";
      char fileDestPathBuffer[10000] = "";
      
      if(strcmp(currentFile->d_name,".") != 0  && strcmp(currentFile->d_name,"..") !=       0)
      {
	strcat(fileOriginPathBuffer,pathSrc);
	strcat(fileOriginPathBuffer,"/");
	strcat(fileOriginPathBuffer,currentFile->d_name);

	strcat(fileDestPathBuffer,pathDest);
	strcat(fileDestPathBuffer,"/");
	strcat(fileDestPathBuffer,currentFile->d_name);
	copyElement(fileOriginPathBuffer,fileDestPathBuffer);
      }
    }
    closedir(folder);
  }
  else
  {
    int toWrite = open(pathDest , O_RDWR | O_CREAT | O_EXCL);
    //file opening
    
    if(toWrite < 0)
    {
      printf("error while creating the copied file \n");
      return 1;
    }
    
    //file copy
    while((readed = read(toCopy,buffer,4096)) > 0)
    {	
      int writtenPos = 0;
    
      while(readed != 0)
      {
	int writtenCount = write(toWrite,buffer+writtenPos,readed);
	readed -= writtenCount;
	writtenPos = writtenCount;

      }
    }
  
    if(readed == -1)
    {
      printf("an error occured while copying \n");
    }

    //managing file rights
    if(chmod(pathDest,fileStat.st_mode) < 0)
    {
      printf("error while editing the rights associated with the copied file\n");
      return 1; 
    }
    
    //closing files
    close(toCopy);
    close(toWrite);  
  }
  
  return 0;
}


int main(int argc, char** argv)
{
  //input verification
  if(argc < 3)
  {
    printf("usage : cp <path of the original file> <path of the output file> \n");
    return 1;
  }

  return copyElement(argv[1],argv[2]);
  
}

