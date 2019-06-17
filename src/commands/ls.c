#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

#include <math.h>

//std library include
#include <stdbool.h>

#define DEFAULT_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define AQUA "\x1B[36m"

/*
 * ls - list directory contents
 *
 * Usage : ls [option] [dir]
 *
 */

int ls(char*, bool, bool);
bool isSpecificFile(char*);
void displayFileWithDetails(struct stat *, const char*, int, int, int, int);
void displayFile(struct stat *, const char*);
void displayFileList(struct dirent **, int, bool);
int initList(struct dirent **, int, bool, int*, int*, int*, int*);
bool contains(char const* const*, int, const char*);
void notEnoughMemory();

int main(int argc, char* argv[]) {
	int status = EXIT_SUCCESS;

	// Determine where apply ls
	if(argc > 1) {
		bool hasListArg = contains((char const* const*) argv, argc, "-l") || contains((char const* const*) argv, argc, "-la") || contains((char const* const*) argv, argc, "-al");
		bool hasAllArg = contains((char const* const*) argv, argc, "-a") || contains((char const* const*) argv, argc, "-la") || contains((char const* const*) argv, argc, "-al");

		int nbPath = 0;
		char** paths = (char**) malloc(sizeof(char**));

		if(paths == NULL) {
			notEnoughMemory();
			return EXIT_FAILURE;
		}

		for(int i=1; i<argc; i++) {
			if(argv[i][0] == '-') { // Do not consider the options
				continue;
			}

			paths = (char**) realloc(paths, (i+1) * sizeof(char**));

			if(paths == NULL) {
				notEnoughMemory();
				return EXIT_FAILURE;
			}

			paths[nbPath] = (char*) malloc(strlen(argv[i]));

			if(paths[nbPath] == NULL) {
				notEnoughMemory();
				return EXIT_FAILURE;
			}

			strcpy(paths[nbPath], argv[i]);
			nbPath++;
		}

		if(nbPath > 1) {
			for(int i=0; i<nbPath; i++) {
				printf("%s:\n", paths[i]);

				if(ls(paths[i], hasListArg, hasAllArg) < 0) {
					status = EXIT_FAILURE;
				}

				if((nbPath-i) > 1) {
					printf("\n");
				}
			}
		}else if(nbPath == 1) {
			status = ls(paths[0], hasListArg, hasAllArg);
		}else{
			status = ls(".", hasListArg, hasAllArg);
		}

		// Free all resources
		for(int i=0; i<nbPath; i++) {
			free(paths[i]);
		}

		free(paths);
	}else{
		status = ls(".", false, false);
	}


	return status;
}

int ls(char* path, bool hasListArg, bool hasAllArg) {
	struct stat s;

	if(stat(path, &s) < 0) {
		fprintf(stderr, "ls: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(S_ISDIR(s.st_mode)) { // The file is a directory
		if(chdir(path) < 0) {
        		fprintf(stderr, "ls: %s\n", strerror(errno));
        		return EXIT_FAILURE;
		}

		struct dirent **dirs;
		int nbFile = 0;

		nbFile = scandir(path, &dirs, NULL, alphasort);

		if(nbFile < 0) {
			fprintf(stderr, "ls: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		if(hasListArg) {
			displayFileList(dirs, nbFile, hasAllArg);
		}else{
			for(int i=0; i<nbFile; i++) {
				struct stat st;

				if(stat(dirs[i]->d_name, &st) < 0) {
					continue;
				}

				if(!hasAllArg && isSpecificFile(dirs[i]->d_name)) {
					continue;
				}

				displayFile(&st, dirs[i]->d_name);
				printf("  ");
			}
			printf("\n");
		}
	}else{ // The file is not a directory
		if(hasListArg) {
			displayFileWithDetails(&s, path, 0, 0, 0, 0);
		}else{
			displayFile(&s, path);
			printf("\n");
		}
	}

	return 0;
}

bool isSpecificFile(char* filename) {
	return strcmp(".", filename) == 0 || strcmp("..", filename) == 0 || filename[0] == '.';
}


void displayFileWithDetails(struct stat *s, const char* path, int nbDigitsFileSize, int nbDigitsFileUsr, int nbDigitsFileGrp, int nbDigitsFileNLink) {
	int counter;

	// Print file type
	if(S_ISDIR(s->st_mode)) {
		printf("d");
	}else if(S_ISLNK(s->st_mode)) {
		printf("l");
	}else{
		printf("-");
	}

	// Print user permissions
	printf((s->st_mode & S_IRUSR) ? "r" : "-");
	printf((s->st_mode & S_IWUSR) ? "w" : "-");
	printf((s->st_mode & S_IXUSR) ? "x" : "-");

	// Print group permissions
	printf((s->st_mode & S_IRGRP) ? "r" : "-");
	printf((s->st_mode & S_IWGRP) ? "w" : "-");
	printf((s->st_mode & S_IXGRP) ? "x" : "-");

	// Print other permissions
	printf((s->st_mode & S_IROTH) ? "r" : "-");
	printf((s->st_mode & S_IWOTH) ? "w" : "-");
	printf((s->st_mode & S_IXOTH) ? "x" : "-");
	printf(" ");

	// Print number of link
	printf("%d ", s->st_nlink);

	counter = 0;
        while(counter < (nbDigitsFileNLink-(floor(log10(abs(s->st_nlink))) + 1))) {
                printf(" ");
                counter++;
        }

	// Print user
	struct passwd *pw = getpwuid(s->st_uid);
	printf("%s ", pw->pw_name);

	counter = 0;
        while(counter < (nbDigitsFileUsr-strlen(pw->pw_name))) {
                printf(" ");
                counter++;
        }

	// Print group
	struct group *grp = getgrgid(s->st_gid);
	printf("%s ", grp->gr_name);

	counter = 0;
        while(counter < (nbDigitsFileGrp-strlen(grp->gr_name))) {
                printf(" ");
                counter++;
        }

	// Print file size
	printf("%*.f ", nbDigitsFileSize, (float) s->st_size);

	// Print last modification date
	char date[13];

	if(strftime(date, sizeof(date), "%b %d %H:%M", localtime(&s->st_mtime)) < 0) {
                return;
        }

        printf("%s ", date);

        // Print filename
        displayFile(s, path);
        printf("%s\n", DEFAULT_COLOR);
}

void displayFile(struct stat *s, const char* path) {
	int useColor = isatty(STDOUT_FILENO);

	if(useColor == 1) {
		if(S_ISDIR(s->st_mode)) { // if the type is directory print in blue
			printf("%s%s", BLUE, basename((char*) path));
		}else if(S_ISLNK(s->st_mode)) { // if the type is symlink print in aqua
			printf("%s%s", AQUA, basename((char*) path));
		}else if(access(path, X_OK) == 0) { // if the file is executable print in green
			printf("%s%s", GREEN, basename((char*) path));
		}else{
			printf("%s%s", DEFAULT_COLOR, basename((char*) path));
		}

		printf("%s", DEFAULT_COLOR);
	}else{
		printf("%s", basename((char*) path));
	}
}

void displayFileList(struct dirent **dirs, int nbFile, bool hasAllArg) {
	// Determine number of block used and init file list
	int nbDigitsFileSize, nbDigitsFileUsr, nbDigitsFileGrp, nbDigitsFileNLink;

        blkcnt_t nbBlockUsed = initList(dirs, nbFile, hasAllArg, &nbDigitsFileSize, &nbDigitsFileUsr, &nbDigitsFileGrp, &nbDigitsFileNLink);
       	printf("total %ld\n", nbBlockUsed);

	// Print file list
	for(int i=0; i<nbFile; i++) {
		struct stat s;

		if(!hasAllArg && isSpecificFile(dirs[i]->d_name)) {
			continue;
		}

		if(stat(dirs[i]->d_name, &s) < 0) {
			fprintf(stderr, "ls: cannot access '%s': %s\n", dirs[i]->d_name, strerror(errno));
			continue;
		}

		displayFileWithDetails(&s, dirs[i]->d_name, nbDigitsFileSize, nbDigitsFileUsr, nbDigitsFileGrp, nbDigitsFileNLink);
	}
}

int initList(struct dirent **dirs, int nbFile, bool hasAllArg, int* nbDigitsFileSize, int* nbDigitsFileUsr, int* nbDigitsFileGrp, int* nbDigitsFileNLink) {
	blkcnt_t nbBlockUsed = 0;

	int maxDigitsFileSize = 0;
	int maxDigitsFileUsr = 0;
	int maxDigitsFileGrp = 0;
	int maxDigitsFileNLink = 0;

	for(int i=0; i<nbFile; i++) {
		if(!hasAllArg && isSpecificFile(dirs[i]->d_name)) {
			continue;
		}

		struct stat s;

		if(stat(dirs[i]->d_name, &s) < 0) {
			continue;
		}

		// Get number of block used
		nbBlockUsed += (s.st_blocks/2);

		// Init digits
		int fileSizeDigits = floor(log10(abs(s.st_size))) + 1;

               	if(fileSizeDigits > maxDigitsFileSize) {
              		maxDigitsFileSize = fileSizeDigits;
            	}

		struct passwd *pw = getpwuid(s.st_uid);
               	int fileUsrDigits = strlen(pw->pw_name);

              	if(fileUsrDigits > maxDigitsFileUsr) {
                	maxDigitsFileUsr = fileUsrDigits;
               	}

		struct group *grp = getgrgid(s.st_gid);
               	int fileGrpDigits = strlen(grp->gr_name);

       	        if(fileGrpDigits > maxDigitsFileGrp) {
      	               maxDigitsFileGrp = fileGrpDigits;
              	}

		int fileNLinkDigits = floor(log10(abs(s.st_nlink))) + 1;

               	if(fileNLinkDigits > maxDigitsFileNLink) {
                	maxDigitsFileNLink = fileNLinkDigits;
               	}
	}

	*nbDigitsFileSize = maxDigitsFileSize;
	*nbDigitsFileUsr = maxDigitsFileUsr;
	*nbDigitsFileGrp = maxDigitsFileGrp;
	*nbDigitsFileNLink = maxDigitsFileNLink;

	return nbBlockUsed;
}

bool contains(char const* const* array, int size, const char* str) {
	int i=0;

	while(i<size) {
		if(strcmp(array[i], str) == 0) {
			return true;
		}

		i++;
	}

	return false;
}

void notEnoughMemory() {
	fprintf(stderr, "ls: not enough memory\n");
}
