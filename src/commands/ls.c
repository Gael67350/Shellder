#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define DEFAULT_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

#define FILENAME_SIZE 256

/*
 * ls - list directory contents
 *
 * Usage : ls [option] [dir]
 */
int main(int argc, char* argv[]) {
	char* options;
	struct dirent **dirs;
	int n, i;

	options = malloc(sizeof(char));

	if(argc > 1) {
		// Determine where to apply ls
		if(argv[argc-1][0] != '-') {
			n = scandir(argv[argc-1], &dirs, NULL, alphasort);
		}else{
			n = scandir(".", &dirs, NULL, alphasort);
		}

		// Parse options
		for(int i=1; i<argc; i++) {
			if(argv[i][0] == '-') {
				//options = realloc(options, sizeof(options) + sizeof(argv[i]) - sizeof(char));
			}
		}
	}

	if(n < 0) {
		fprintf(stderr, "Unable to get file list\n");
		return EXIT_FAILURE;
	}else{
		// List directory contents
		i = 0;
		while(i<n) {
			if(strcmp(".", dirs[i]->d_name) != 0 && strcmp("..", dirs[i]->d_name) != 0 && dirs[i]->d_name[0] != '.') {
				if(dirs[i]->d_type == DT_DIR) { // if the type is directory print in blue
					printf("%s%s", BLUE, dirs[i]->d_name);
				}else if(access(dirs[i]->d_name, X_OK) == 0) { // if the file is executable print in green
					printf("%s%s", GREEN, dirs[i]->d_name);
				}else{
					printf("%s%s", DEFAULT_COLOR, dirs[i]->d_name);
				}

				printf("  ");
			}

			free(dirs[i]);
			i++;
		}
		printf("\n");
		free(dirs);
	}

	return 0;
}
