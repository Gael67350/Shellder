#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <stdlib.h>
#include "coreCommands.h"

/*
 * cd - Change the shell working directory
 *
 * Usage : cd [dir]
 */
int cd(int argc, char** argv) {
	int status;

	if(argc == 1) {
	  status = chdir(getenv("HOME"));
	}else{
		status = chdir(argv[1]);
	}

	if(status == -1) {
		switch(errno) {
			case EACCES:
				fprintf(stderr, "Permission denied\n");
				break;
			case EFAULT:
				fprintf(stderr, "Invalid path\n");
				break;
			case EIO:
				fprintf(stderr, "Remote I/O error\n");
				break;
			case ELOOP:
				fprintf(stderr, "Too many symbolic links encountered\n");
				break;
			case ENAMETOOLONG:
				fprintf(stderr, "File name too long\n");
				break;
			case ENOENT:
				fprintf(stderr, "No such file or directory\n");
				break;
			case ENOMEM:
				fprintf(stderr, "Out of memory\n");
				break;
			case ENOTDIR:
				fprintf(stderr, "Not a directory\n");
				break;
			default:
				fprintf(stderr, "An unexpected error occured\n");
		}

		return 1;
	}

	return 0;
}
