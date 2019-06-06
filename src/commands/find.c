#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

/*
 * find -
 *
 * Usage: find [path...] [expression]
 *
 */

int find(const char*);

int main(int argc, char* argv[]) {
	// -name
	// -exec

	if(argc > 1) {
		return find(argv[1]);
	}

	return find(".");
}

int find(const char* path) {
	int fd, n;
	struct dirent **dirs;

	if((fd = open(path, O_RDONLY)) < 0) {
		fprintf(stderr, "find: '%s': %s\n", path, strerror(errno));
		return EXIT_FAILURE;
	}

	printf("%s\n", path); // Print current filename in the standard output
	close(fd);

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
			find(concat);
		}else{
			printf("%s\n", concat);
		}

		free(dirs[i]);
	}

	free(dirs);
	return EXIT_SUCCESS;
}
