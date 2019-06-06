all: createDirectories externalPrograms Shellder

test: all
	./bin/Shellder

Shellder: ./bin/commandEntryComponents/entryManagement.o ./bin/Shellder.o ./bin/commands/coreCommands.o
	gcc -o ./bin/Shellder ./bin/commandEntryComponents/entryManagement.o ./bin/commands/coreCommands.o ./bin/Shellder.o

externalPrograms: ./src/commands/cat.c
	gcc ./src/commands/cat.c -o ./bin/commands/external/cat

./bin/commandEntryComponents/entryManagement.o : ./src/commandEntryComponents/entryManagement.c ./src/commandEntryComponents/entryManagement.h
	gcc -o ./bin/commandEntryComponents/entryManagement.o -c ./src/commandEntryComponents/entryManagement.c -Wall -Werror

./bin/Shellder.o :  ./src/Shellder.c ./src/commandEntryComponents/entryManagement.h 
	gcc -o ./bin/Shellder.o -c  ./src/Shellder.c -Wall -Werror

./bin/commands/coreCommands.o: ./src/commands/coreCommands/cd.c ./src/commands/coreCommands/coreCommands.h
	gcc -o ./bin/commands/coreCommands.o -c ./src/commands/coreCommands/cd.c -Wall -Werror

createDirectories:
	rm -rf ./bin
	mkdir ./bin
	mkdir ./bin/commandEntryComponents

#	commands directory
	mkdir ./bin/commands/
	mkdir ./bin/commands/external
