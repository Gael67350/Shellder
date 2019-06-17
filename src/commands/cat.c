#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096

/*
 * cat - concatenate files and print on the standard output
 *
 * Usage : cat [FILE]...
 *
 */

int main(int argc, char* argv[]) {
	char buffer[BUFFER_SIZE];
	int readBytes, writtenBytes;

	if(argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && argv[1][0] == '-')) { // copy standard input to standard output
		while((readBytes = read(STDIN_FILENO, buffer, BUFFER_SIZE)) >= 0) {
			while((writtenBytes = write(STDOUT_FILENO, buffer, readBytes)) < readBytes) {
				if(writtenBytes < 0) {
					fprintf(stderr, "cat: %s\n", strerror(errno));
					return EXIT_FAILURE;
				}
			}
		}

		if(readBytes < 0) {
			fprintf(stderr, "cat: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}else if(argc == 2 && strcmp(argv[1], "--help") == 0) { // print usage to standard output
		printf("Usage: cat [FILE]...\n");
		printf("Concatenate FILE(s) to standard output.\n");
		printf("\n");
		printf("With no FILE, or when FILE is -, read standard input.\n");
	}else{ // concatenate file(s) to standard output
		for(int i=1; i<argc; i++) {
			int fd = open(argv[i], O_RDONLY);

			if(fd < 0) {
				fprintf(stderr, "cat: %s\n", strerror(errno));
				continue;
			}

			while((readBytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
				while((writtenBytes = write(STDOUT_FILENO, buffer, readBytes)) < readBytes) {
					if(writtenBytes < 0) {
						fprintf(stderr, "cat: %s\n", strerror(errno));
						continue;
					}
				}
			}

			if(readBytes < 0) {
				fprintf(stderr, "cat: %s\n", strerror(errno));
				continue;
			}

			close(fd);
		}
	}

	return EXIT_SUCCESS;
}
