test: all
	./bin/Shellder

all: createDirectories Shellder

Shellder: ./bin/commandEntryComponents/entryManagement.o ./bin/Shellder.o
	gcc -o ./bin/Shellder ./bin/commandEntryComponents/entryManagement.o ./bin/Shellder.o

./bin/commandEntryComponents/entryManagement.o : ./src/commandEntryComponents/entryManagement.c ./src/commandEntryComponents/entryManagement.h
	gcc -o ./bin/commandEntryComponents/entryManagement.o -c ./src/commandEntryComponents/entryManagement.c -Wall -Werror

./bin/Shellder.o :  ./src/Shellder.c ./src/commandEntryComponents/entryManagement.h 
	gcc -o ./bin/Shellder.o -c  ./src/Shellder.c -Wall -Werror

createDirectories:
	rm -rf ./bin
	mkdir ./bin
	mkdir ./bin/commandEntryComponents