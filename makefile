all: createDirectories externalPrograms Shellder

test: all
	./bin/Shellder

Shellder: ./bin/commandEntryComponents/entryManagement.o ./bin/Shellder.o ./bin/commands/coreCommandsCd.o ./bin/commands/coreCommandsEvtVar.o
	gcc -o ./bin/Shellder ./bin/commandEntryComponents/entryManagement.o ./bin/commands/coreCommandsCd.o ./bin/commands/coreCommandsEvtVar.o ./bin/Shellder.o 

externalPrograms: ./src/commands/cat.c ./src/commands/find.c ./src/commands/ls.c
	gcc ./src/commands/cat.c -o ./bin/commands/external/cat -Wall -Werror
#	gcc ./src/commands/find.c -o ./bin/commands/external/find 
	gcc ./src/commands/ls.c -o ./bin/commands/external/ls  -lm

./bin/commandEntryComponents/entryManagement.o : ./src/commandEntryComponents/entryManagement.c ./src/commandEntryComponents/entryManagement.h
	gcc -o ./bin/commandEntryComponents/entryManagement.o -c ./src/commandEntryComponents/entryManagement.c -Wall -Werror

./bin/Shellder.o :  ./src/Shellder.c ./src/commandEntryComponents/entryManagement.h
	gcc -o ./bin/Shellder.o -c  ./src/Shellder.c -Wall -Werror

./bin/commands/coreCommandsCd.o: ./src/commands/coreCommands/cd.c ./src/commands/coreCommands/coreCommands.h 
	gcc -o ./bin/commands/coreCommandsCd.o -c ./src/commands/coreCommands/cd.c -Wall -Werror

./bin/commands/coreCommandsEvtVar.o: ./src/commands/coreCommands/EnvironementVariableManagement.c ./src/commands/coreCommands/coreCommands.h 
	gcc -o ./bin/commands/coreCommandsEvtVar.o -c ./src/commands/coreCommands/EnvironementVariableManagement.c -Wall -Werror


createDirectories:
	rm -rf ./bin
	mkdir ./bin
	mkdir ./bin/commandEntryComponents

#	commands directory
	mkdir ./bin/commands/
	mkdir ./bin/commands/external
