#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *ltrim(char*);
char *rtrim(char*);
char *trim(char *str);

int main(int argc, char* argv[]) {
	if(argc < 2) {
		fprintf(stderr, "missing arguments\n");
		return EXIT_FAILURE;
	}

	char* cmd = malloc(strlen(argv[1]));

	if(cmd == NULL) {
		fprintf(stderr, "allocation error\n");
		return EXIT_FAILURE;
	}

	strcpy(cmd, argv[1]);
	const char delim[2] = "|";
	char* token = strtok(cmd, delim);

	while(token != NULL) {
		token = trim(token);
		printf("|%s|\n", token);
		// Passer cmd -> Recuperer structure, piped = false

		// Ajouter 2 a N command a pipedCommand de la structure et piped = true

		token = strtok(NULL, delim);
	}

	// retourner la 1ere structure command entry
}

char *ltrim(char *str) {
    int totrim = strspn(str, "\t\n\v\f\r ");

    if (totrim > 0) {
        int len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }

    return str;
}

char *rtrim(char *str) {
    int i = strlen(str) - 1;

    while (i >= 0 && strchr("\t\n\v\f\r ", str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }

    return str;
}

char *trim(char *str) {
    return ltrim(rtrim(str));
}
