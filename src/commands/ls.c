#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#define DEFAULT_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define AQUA "\x1B[36m"

/*
 * ls - list directory contents
 *
 * Usage : ls [option] [dir]
 */

bool contains(char const* const*, int, const char*);
void displayFilename(struct dirent*);
int displayFile(struct dirent*, int, int, int, int);
bool isSpecificFile(char*);

int main(int argc, char* argv[]) {
	struct dirent **dirs;
	int n, i;
	int maxDigitsNLink = 0, maxDigitsUsr = 0, maxDigitsGrp = 0, maxDigitsFileSize = 0;

	bool hasListArg = (contains(argv, argc, "-l") || contains(argv, argc, "-la") || contains(argv, argc, "-al"));
	bool hasAllArg = (contains(argv, argc, "-a") || contains(argv, argc, "-la") || contains(argv, argc, "-al"));

	// Determine where apply ls
	if(argc > 1) {
		if(argv[argc-1][0] != '-') {
			n = scandir(argv[argc-1], &dirs, NULL, alphasort);

			if(chdir(argv[argc-1]) < 0) {
				fprintf(stderr, "ls: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
		}else if(strlen(argv[argc-1]) == 1){
			fprintf(stderr, "ls: cannot access '-': No such file or directory\n");
		}else{
			n = scandir(".", &dirs, NULL, alphasort);
		}
	}else{
		n = scandir(".", &dirs, NULL, alphasort);
	}

	if(n < 0) {
		fprintf(stderr, "ls: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// List directory contents
	for(i=0; i<n; i++) {
		if((isSpecificFile(dirs[i]->d_name) && hasListArg) || hasAllArg) {
			struct stat fileStat;

			if(stat(dirs[i]->d_name, &fileStat) < 0) {
				continue;
        		}

			int fileNLinkDigits = floor(log10(abs(fileStat.st_nlink))) + 1;

			if(fileNLinkDigits > maxDigitsNLink) {
				maxDigitsNLink = fileNLinkDigits;
			}

			int fileSizeDigits = floor(log10(abs(fileStat.st_size))) + 1;

			if(fileSizeDigits > maxDigitsFileSize) {
				maxDigitsFileSize = fileSizeDigits;
			}

			struct passwd *pw = getpwuid(fileStat.st_uid);
			int fileUsrDigits = strlen(pw->pw_name);

			if(fileUsrDigits > maxDigitsUsr) {
				maxDigitsUsr = fileUsrDigits;
			}

			struct group *grp = getgrgid(fileStat.st_gid);
        		int fileGrpDigits = strlen(grp->gr_name);

			if(fileGrpDigits > maxDigitsGrp) {
				maxDigitsGrp = fileGrpDigits;
			}
		}
	}

	// Print number of block used
	i = 0;
	while(i<n) {
		if(isSpecificFile(dirs[i]->d_name) || hasAllArg) {
			if(hasListArg) {
				// Print file details
				displayFile(dirs[i], maxDigitsFileSize, maxDigitsUsr, maxDigitsGrp, maxDigitsNLink);
			}else{
				// Print filename only
				displayFilename(dirs[i]);
				printf("  ");
			}
		}

		free(dirs[i]);
		i++;
	}

	if(!hasListArg) {
		printf("\n");
	}

	free(dirs);

	return 0;
}

bool contains(char const* const* array, int size, const char* str) {
	bool found = false;
	int i=0;

	while(!found && i<size) {
		if(strcmp(array[i], str) == 0) {
			found = true;
		}

		i++;
	}

	return found;
}

void displayFilename(struct dirent* dir) {
	if(dir->d_type == DT_DIR) { // if the type is directory print in blue
		printf("%s%s", BLUE, dir->d_name);
	}else if(dir->d_type == DT_LNK) {
		printf("%s%s", AQUA, dir->d_name);
	}else if(access(dir->d_name, X_OK) == 0) { // if the file is executable print in green
		printf("%s%s", GREEN, dir->d_name);
	}else{
		printf("%s%s", DEFAULT_COLOR, dir->d_name);
	}

	printf("%s", DEFAULT_COLOR);
}

int displayFile(struct dirent* dir, int nbDigitsFileSize, int nbDigitsFileUsr, int nbDigitsFileGrp, int nbDigitsFileNLink) {
	struct stat fileStat;
	int counter;

        if(lstat(dir->d_name, &fileStat) < 0) {
		fprintf(stderr, "ls: cannot access '%s': %s\n", dir->d_name, strerror(errno));
       		return EXIT_FAILURE;
        }

	// Print file type
	if(S_ISDIR(fileStat.st_mode)) {
		printf("d");
	}else if(S_ISLNK(fileStat.st_mode)) {
		printf("l");
	}else{
		printf("-");
	}

        // Print user permissions
        printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");

        // Print group permissions
        printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");

        // Print other permissions
        printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf(" ");

	// Print number of link
	counter = 0;
	while(counter < (nbDigitsFileNLink-(floor(log10(abs(fileStat.st_nlink))) + 1))) {
		printf(" ");
		counter++;
	}

	printf("%d ", fileStat.st_nlink);

        // Print user
        struct passwd *pw = getpwuid(fileStat.st_uid);
	counter = 0;

	while(counter < (nbDigitsFileUsr-strlen(pw->pw_name))) {
		printf(" ");
		counter++;
	}

        printf("%s ", pw->pw_name);

        // Print group
        struct group *grp = getgrgid(fileStat.st_gid);
        counter = 0;

	while(counter < (nbDigitsFileGrp-strlen(grp->gr_name))) {
		printf(" ");
		counter++;
	}

	printf("%s ", grp->gr_name);

        // Print file size
        printf("%*.f ", nbDigitsFileSize, (float) fileStat.st_size);

        // Print last modification date
        char date[13];

        if(strftime(date, sizeof(date), "%b %d %H:%M", localtime(&fileStat.st_mtime)) < 0) {
        	return EXIT_FAILURE;
        }

        printf("%s ", date);

        // Print filename
        displayFilename(dir);
        printf("%s\n", DEFAULT_COLOR);
	return 0;
}

bool isSpecificFile(char* filename) {
	return strcmp(".", filename) != 0 && strcmp("..", filename) != 0 && filename[0] != '.';
}
