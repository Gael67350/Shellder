#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

/*
 * find -
 *
 * Usage: find [path...] [expression]
 *
 */

int find(const char*, char const* const*, int);
int execute(const char*, char const* const*, int);
int containsAt(char const* const*, int, const char*);

int main(int argc, char* argv[]) {
	// -name

	if(argc > 1) {
		int execPos = containsAt((char const* const*) argv, argc, "-exec");

		int nbPath = 0, cmdArgc = 0, j = 0;

		char** path = (char**) malloc(sizeof(char*));
		char** cmd = (char**) malloc(sizeof(char*));

		if(path == NULL || cmd == NULL) {
                	fprintf(stderr, "find: not enough memory\n");
                	return EXIT_FAILURE;
                }

		if(execPos <= 0) { // No -exec option in the command line
			for(int i=1; i<argc; i++) {
				find(argv[i], NULL, 0);
			}
		}else if(execPos == 1) { // -exec option is the 2nd parameter of the command line
			path[0] = (char*) malloc(1 * sizeof(char));

			if(path[0] == NULL) {
				fprintf(stderr, "find: not enough memory\n");
				return EXIT_FAILURE;
			}

			strcpy(path[0], ".");
			nbPath++;
		}else{
			for(int i=0; i<execPos; i++) {
				path = (char**) realloc(path, (nbPath + 1) * sizeof(char*));

				if(path == NULL) {
					fprintf(stderr, "find: not enough memory\n");
                                        return EXIT_FAILURE;
				}

				path[j] = (char*) malloc(strlen(argv[i]));

				if(path[j] == NULL) {
					fprintf(stderr, "find: not enough memory\n");
					return EXIT_FAILURE;
				}

				strcpy(path[j], argv[i]);
				nbPath++;
				j++;
			}
		}

		if(execPos == argc-1) {
			fprintf(stderr, "find: missing argument to `-exec`\n");
			return EXIT_FAILURE;
		}

		j=0;

		for(int i=execPos+1; i<argc; i++) {
			cmd = (char**) realloc(cmd, (cmdArgc + 2) * sizeof(char*));

			if(cmd == NULL) {
				fprintf(stderr, "find: not enough memory\n");
				return EXIT_FAILURE;
			}

			cmd[j] = (char*) malloc(strlen(argv[i]));

			if(cmd[j] == NULL) {
				fprintf(stderr, "find: not enough memory\n");
                                return EXIT_FAILURE;
			}

			strcpy(cmd[j], argv[i]);
			cmdArgc++;
			j++;
		}

		for(int i=0; i<nbPath; i++) {
			find(path[i], (char const* const*) cmd, cmdArgc);
		}

		for(int i=0; i<cmdArgc; i++) {
			free(cmd[i]);
		}

		free(cmd);
	}else{
		find(".", NULL, 0);
	}
}

int find(const char* path, char const* const* cmd, int cmdArgc) {
	int fd, n;
	struct stat s;
	struct dirent **dirs;

	if((fd = open(path, O_RDONLY)) < 0) {
		fprintf(stderr, "find: '%s': %s\n", path, strerror(errno));
		return EXIT_FAILURE;
	}

	if(execute(path, cmd, cmdArgc) < 0) { // Execute current filename
		return EXIT_FAILURE;
	}

	if(fstat(fd, &s) < 0) {
		fprintf(stderr, "find: '%s': %s\n", path, strerror(errno));
		return EXIT_FAILURE;
	}

	if(s.st_mode & S_IFDIR) {
		if((n = scandir(path, &dirs, NULL, NULL)) < 0) {
			fprintf(stderr, "find: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		for(int i=0; i<n; i++) {
			if(strcmp(dirs[i]->d_name, "..") == 0 || strcmp(dirs[i]->d_name, ".") == 0) {
				continue;
			}

			char* concat = (char*) malloc(strlen(path) + strlen(dirs[i]->d_name) + 2);
                	strcpy(concat, path);

			if(path[strlen(path)-1] != '/') {
                		strcat(concat, "/");
			}

                	strcat(concat, dirs[i]->d_name);

			if(dirs[i]->d_type == DT_DIR) { // If the file considered is a directory
				find(concat, cmd, cmdArgc);
			}else{
				execute(concat, cmd, cmdArgc);
			}

			free(dirs[i]);
		}

		free(dirs);
	}

	close(fd);
	return EXIT_SUCCESS;
}

int execute(const char* path, char const* const* cmd, int cmdArgc) {
	if(cmd == NULL) {
		printf("%s\n", path);
	}else{
		// Copy cmd array
		char** cmdcp = (char**) malloc((cmdArgc + 1) * sizeof(char*));

		if(cmdcp == NULL) {
                	fprintf(stderr, "find: not enough memory\n");
                	return EXIT_FAILURE;
               	}

		for(int i=0; i<cmdArgc; i++) {
			cmdcp[i] = (char*) malloc(strlen(cmd[i]));

			if(cmdcp[i] == NULL) {
                        	fprintf(stderr, "find: not enough memory\n");
                               	return EXIT_FAILURE;
                        }

			strcpy(cmdcp[i], cmd[i]);

			if(strcmp(cmdcp[i], "{}") == 0) {
				cmdcp[i] = (char*) realloc(cmdcp[i], strlen(path) + 1);

				if(cmdcp[i] == NULL) {
					fprintf(stderr, "find: not enough memory\n");
					return EXIT_FAILURE;
				}

				strcpy(cmdcp[i], path);
				strcat(cmdcp[i], "\0");
			}
		}

		cmdcp[cmdArgc] = NULL;

		// Execute the command given in the parameters
		int proc = fork();

		if(proc == 0) {
			if(execvp(cmdcp[0], cmdcp) == -1) {
				fprintf(stderr, "%s: %s\n", cmdcp[0], strerror(errno));
				return EXIT_FAILURE;
			}
		}else{
			wait(NULL);
		}

		free(cmdcp);
	}

	return EXIT_SUCCESS;
}

int containsAt(char const* const* array, int size, const char* str) {
	int i=0;

	while(i<size) {
		if(strcmp(array[i], str) == 0) {
			return i;
		}

		i++;
	}

	return -1;
}
