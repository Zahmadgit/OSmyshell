#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <time.h> 
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <sys/ioctl.h> 

//might as well include everything I might need

int cdCommand(char **args);
int clrCommand(char **args);
int dirCommand(char **args);
int environCommand(char **args);
int echoCommand(char **args);
int helpCommand(char **args);
int pauseCommand(char **args);
int builtIn(char* argv[]);
void shellPath();
//quit will just need exit()

char* builtInNames[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause"};

int (*builtInPtr[])(char **) = {&cdCommand, &clrCommand, &dirCommand, &environCommand, &echoCommand, &helpCommand, &pauseCommand}; 

int builtIn(char* argv[]){
	/* for loop to shift through builtInNames array to check if user is asking for 
	these commands, if loop to check if user input is same,returning 1 if true and 
	0 otherwise */
	for(int i = 0; i < sizeof(builtInNames) / sizeof(builtInNames[0]); i++){
		if(strcmp(argv[0], builtInNames[i]) == 0){
			return 1;
		}
	}
	return 0;
}


int cdCommand(char **args){
	if(args[1] == NULL){		//if there isnt an args then current dir is printed 
		char dirName[1024];
		getcwd(dirName, sizeof(dirName));
		printf("%s\n", dirName);
		
	} else {
		if(chdir(args[1]) != 0){
			printf("directory does not exist or cd command error\n");
		}
	}
}

int clrCommand(char **args){
	printf("\e[1;1H\e[2J"); 	//clears and returns to top of screen 
}

int dirCommand(char **args){
	DIR *pdir; 				//pointer directory 
    	struct dirent *directory;  		//pointer for opening directory
    	pdir = opendir("./");
    	if(pdir != NULL){ 			//if directorory isnt empty
    	while(directory = readdir(pdir)){ 	//while directory isnt empty, reads through
		printf("%s\n",directory->d_name);
        }
        (void) closedir(pdir);
    	} else {
	    	printf("Not a Directory or dir command error\n");
    	}
    	return 0;
}

int environCommand(char **args){
	extern char **environ; 	//variable points to an array of pointers to strings
	for(int i = 0; environ[i] != NULL; i++){
        	printf("%s\n", environ[i]);	//prints through environ strings
        }
        return 0;
}


int echoCommand(char **args){
	
	for(int i = 1;args[i] != NULL; i++){	//increment through argument to print entire sentence
		printf("%s ", args[i]);
        	}
    	printf("\n");
}

int helpCommand(char **args){
	
	FILE *filePtr;
	int counter = 0;
	char helpFile[1024];
	filePtr = fopen("readme_doc", "r");
	while(fgets(helpFile, 1024, filePtr) != NULL){
		printf("%s", helpFile);
		counter++;
	}
	
	return 1;
}

int pauseCommand(char **args){
	char pause[1024];
	printf("Paused, click enter to unpause: ");
	while(strcmp((pause), "\n") != 0){ 			//returns 0 if true
		printf("Paused, click enter to unpause: "); 	//ask for input again and fgets
		//enters into array
		fgets(pause, sizeof(pause), stdin);
	}
	printf("Shell resumed\n");
	
}


void shellPath(){
	size_t len = 1024;
    	char name[1024]; //for hostname working on 
    	char currentDir[1024]; //for current working directory
    	getcwd(currentDir, sizeof(currentDir));
    	gethostname(name, len); 
        printf("Shell=%s%s~>", name, currentDir); 
}




















