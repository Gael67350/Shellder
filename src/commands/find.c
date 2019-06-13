#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fnmatch.h>
#include <libgen.h>
#include <errno.h>

/*
 * find - search for files in a directory hierarchy
 *
 * Usage: find [path...] [expression]
 *
 */

int find(const char*, char const* const*, int, const char*);
int execute(const char*, char const* const*, int, const char*);
int containsAt(char const* const*, int, const char*);
char** getCmdLineAfter(char const* const*, int*, int);
char** getCmdLineBefore(char const* const*, int*, int);
void notEnoughMemory();

int main(int argc, char* argv[]) {
	int status = EXIT_SUCCESS;

	if(argc > 1) {
		int execPos = containsAt((char const* const*) argv, argc, "-exec");
		int namePos = containsAt((char const* const*) argv, argc, "-name");

		if(execPos == -1 && namePos == -1) { // The command line doesn't contain any option
			for(int i=1; i<argc; i++) {
				if(find(argv[i], NULL, 0, NULL) < 0) {
					status = EXIT_FAILURE;
				}
			}

			return status;
		}

		int nbCmd = argc, nbPath = argc;

		char** cmds = NULL;
		char** paths = NULL;
		char* pattern = NULL;

		if(execPos != -1 && namePos != -1) { // The command line contains both -name and -exec options
			if(namePos > execPos) {
                        	fprintf(stderr, "find: `-name` option cannot be used after `-exec` option\n");
                        	return EXIT_FAILURE;
                	}

			cmds = getCmdLineAfter((char const* const*) argv, &nbCmd, execPos);

			if((execPos-namePos) > 2) {
				fprintf(stderr, "find: too many arguments after `-name` option\n");
				return EXIT_FAILURE;
			}else if((execPos-namePos) < 2) {
				fprintf(stderr, "find: missing argument to `-name`\n");
				return EXIT_FAILURE;
			}

			paths = getCmdLineBefore((char const* const*) argv, &nbPath, namePos);
			pattern = argv[namePos+1];

			if(cmds == NULL || paths == NULL) {
				fprintf(stderr, "find: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
		}else if(execPos != -1) { // The command line contains only -exec option
			cmds = getCmdLineAfter((char const* const*) argv, &nbCmd, execPos);
			paths = getCmdLineBefore((char const* const*) argv, &nbPath, execPos);

			if(cmds == NULL || paths == NULL) {
                                fprintf(stderr, "find: %s\n", strerror(errno));
                                return EXIT_FAILURE;
                        }
		}else{ // The command line contains only -name option
			paths = getCmdLineBefore((char const* const*) argv, &nbPath, namePos);

			if(paths == NULL) {
				fprintf(stderr, "find: %s\n", strerror(errno));
                                return EXIT_FAILURE;
			}

			if((argc-namePos) > 2) {
				fprintf(stderr, "find: too many arguments after `-name` option\n");
                                return EXIT_FAILURE;
			}else if((argc-namePos) < 2) {
				fprintf(stderr, "find: missing argument to `-name`\n");
                                return EXIT_FAILURE;
			}

			pattern = argv[namePos+1];
		}

		if(nbPath == 1) { // There is no path given in the command line (use `.` in this case)
			status = find(".", (char const* const*) cmds, nbCmd, pattern);
		}else{
			for(int i=1; i<nbPath; i++) {
				if(find(paths[i], (char const* const*) cmds, nbCmd, pattern) < 0) {
					status = EXIT_FAILURE;
				}
			}
		}

		// Free all resources
		if(cmds != NULL) {
			for(int i=0; i<nbCmd; i++) {
				free(cmds[i]);
			}

			free(cmds);
		}

		if(paths != NULL) {
			for(int i=0; i<nbPath; i++) {
				free(paths[i]);
			}

			free(paths);
		}
	}else{
		find(".", NULL, 0, NULL);
	}

	return status;
}

int find(const char* path, char const* const* cmd, int cmdArgc, const char* pattern) {
	int fd, n;
	struct stat s;
	struct dirent **dirs;

	if((fd = open(path, O_RDONLY)) < 0) {
		fprintf(stderr, "find: '%s': %s\n", path, strerror(errno));
		return EXIT_FAILURE;
	}

	if(execute(path, cmd, cmdArgc, pattern) < 0) { // Execute current filename
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
				find(concat, cmd, cmdArgc, pattern);
			}else{
				execute(concat, cmd, cmdArgc, pattern);
			}

			free(dirs[i]);
		}

		free(dirs);
	}

	close(fd);
	return EXIT_SUCCESS;
}

int execute(const char* path, char const* const* cmd, int cmdArgc, const char* pattern) {
	if(pattern != NULL && fnmatch(pattern, basename((char*) path), FNM_PATHNAME) == FNM_NOMATCH) {
		return EXIT_FAILURE;
	}

	if(cmd == NULL) {
		printf("%s\n", path);
	}else{
		// Copy cmd array
		char** cmdcp = (char**) malloc((cmdArgc + 1) * sizeof(char*));

		if(cmdcp == NULL) {
                	notEnoughMemory();
                	return EXIT_FAILURE;
               	}

		for(int i=0; i<cmdArgc; i++) {
			cmdcp[i] = (char*) malloc(strlen(cmd[i]));

			if(cmdcp[i] == NULL) {
                        	notEnoughMemory();
                               	return EXIT_FAILURE;
                        }

			strcpy(cmdcp[i], cmd[i]);

			if(strcmp(cmdcp[i], "{}") == 0) {
				cmdcp[i] = (char*) realloc(cmdcp[i], strlen(path) + 1);

				if(cmdcp[i] == NULL) {
					notEnoughMemory();
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

char** getCmdLineAfter(char const* const* array, int* size, int position) {
	if(position < 0 || position >= (*size)) {
		errno = EINVAL;
		return NULL;
	}

	char** result = (char**) malloc(sizeof(char*));
	int j=0;

	if(result == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	for(int i=position+1; i<(*size); i++) {
		result = (char**) realloc(result, (i+1) * sizeof(char*));

		if(result == NULL) {
			errno = ENOMEM;
			return NULL;
		}

		result[j] = (char*) malloc(strlen(array[i]));
		strcpy(result[j], array[i]);

		if(result[j] == NULL) {
			errno = ENOMEM;
			return NULL;
		}

		j++;
	}

	*size = j;
	return result;
}

char** getCmdLineBefore(char const* const* array, int* size, int position) {
	if(position < 0 || position >= (*size)) {
		errno = EINVAL;
		return NULL;
	}

	char** result = (char**) malloc(sizeof(char*));
        int j=0;

        if(result == NULL) {
		errno = ENOMEM;
                return NULL;
        }

        for(int i=0; i<position; i++) {
                result = (char**) realloc(result, (i+1) * sizeof(char*));

                if(result == NULL) {
			errno = ENOMEM;
                        return NULL;
                }

                result[j] = (char*) malloc(strlen(array[i]));
		strcpy(result[j], array[i]);

                if(result[j] == NULL) {
			errno = ENOMEM;
                        return NULL;
                }

                j++;
        }

        *size = j;
        return result;
}

void notEnoughMemory() {
	fprintf(stderr, "find: not enough memory\n");
}
