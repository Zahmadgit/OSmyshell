#include "builtInCommands.h"
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


char *readInput();
int runBuiltIn(char* argv[1024]);
void inputParse(char *line, char* argv[1024]);
int inputLength; 
void forkProgram(char* argv[1024], int backgroundCheck);
int parseEntire(char *line);
int redirectCheck(char *argv[1024], int backgroundCheck);
void redirect(char* argv[1024], int redirectCheck, int input);


/* function for reading user input
pointer is returned with char of user input
*/
char *readInput(){
	char *line = NULL;
    	size_t len = 0;
    	getline(&line, &len, stdin);
    	return line;	
}


/*
comparing different command names 
returning arry location of command
*/
int runBuiltIn(char* argv[1024]){
    for(int i = 0; i < 7; i++){
        if(strcmp(argv[0], builtInNames[i]) == 0){
            return (*builtInPtr[i])(argv); 
        }
    }
    return 0;
}
/*	function to tokenize input 
*/
void inputParse(char *line, char* argv[1024]){
	const char delim[]={" \t\n"};		//tokenize spaces, tabs and new lines
	char *token =strtok(line, delim);	//token char that manipulates line pasesed in and takes first input
	int counter = 0;			//counter for keeping track of input location at loop
	argv[counter] = token; 		//store first command for input
	counter++;
	
	while(token != NULL){			//while there is tokens, continue getting tokens and increment counter
		token = strtok(NULL, delim);
		argv[counter] = token;
		counter++;
	}
	inputLength = counter-1;		//command length deducted, can use for other functions to get total user input length
}


/*
	fork function for running commands that arent built in 
*/

void forkProgram(char* argv[1024], int backgroundCheck){
	int pid = fork(); //save fork 
	
	if(pid < 0){
        	perror("Error: fork failed\n"); //handles error
    	} else if(pid == 0){ 					//child process
			if(execvp(argv[0], argv) < 0){//checks for error in executing
                    		printf("%s: No such command\n", argv[0]);
            			exit(0);
        	}
    	}else{ 							//parent process
        	waitpid(pid, NULL, 0);
    	}
}

/*
function for parsing line after getting it from inputParse
function will run the command if it is built in and if it isnt its forked
*/

int parseEntire(char *line){
    	int backgroundCheck = 0;			//probably putting in each function, checking for character &
    	char* argv[1024]; 
    	inputParse(line, argv); 
   	if(argv[0] == NULL){
   		 return 0; 
        }
        if(strcmp(argv[inputLength-1], "&") == 0){	//comparing command line and setting backgroundCheck to 1 if there is &
        	backgroundCheck = 1;
        	argv[inputLength - 1] = NULL;		//removes & to avoid conflicts with other functions
        }
        int updatedLine = redirectCheck(argv, backgroundCheck);	//redirectCheck gives back int value to check for redirection
        if (updatedLine == 0){				//no redirection
        	if(builtIn(argv) == 1){
            		runBuiltIn(argv); 
        	}else{
            		forkProgram(argv, backgroundCheck); 
            		return 1;
        	}
        return 1;
        } 
}


int redirectCheck(char *argv[1024], int backgroundCheck){
	int isRedirect = 0; 			//returning 1 if their is < >, 0 if none
    	for(int i = 0; argv[i] != NULL; i++){	//loop to check through commands
        	if(strcmp(argv[i], "<") == 0){ 
            		redirect(argv, 1, i);	//will redirect < and redirec function reads check as 1
            		isRedirect = 1; 	
        	}else if(strcmp(argv[i], ">") == 0){
            		redirect(argv, 2, i); 	//will redirect > and redirec function reads check as 2
            		isRedirect = 1; 	
        	}else if(strcmp(argv[i], ">>") == 0){
            		redirect(argv, 3, i); 	//will redirect >> and redirec function reads check as 3
            		isRedirect = 1; 	
        	}
   	 }
    	return isRedirect; 
}





void redirect(char* argv[1024], int redirectCheck, int input){
	int counter = input; 	//counter for input
    	int read = dup(0); 	//read command
    	int write = dup(1); 	//write command
    	int pid = fork(); 	
    	
    	if(pid == 0){		//child
        	if(redirectCheck == 1){ 							// < is the redirect token
            		int input = open(argv[counter + 1], O_CREAT | O_RDONLY, S_IRWXU); 	//opens the file for reading with permissions
            		dup2(input, 0); 							//dups
            		close(input); 				
            		argv[counter] = NULL; 							//redirection is turned to NULL
            		argv[counter+1] = NULL; 						//file is turned to NULL
            		counter++; 								
        	}else if(redirectCheck == 2){							// > is the redirect token
            		int fd = open(argv[counter + 1], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU); 	//opens file to write or create with permissions
            		dup2(fd, 1); //writing						
            		close(fd); 							
            		argv[counter] = NULL; 							
            		argv[counter+1] = NULL; 				
            		counter++; 								
        	}else if(redirectCheck == 3){ 							// >> is the redirect token
            		int fd = open(argv[counter + 1], O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);//opens file to append or create with permissions
            		dup2(fd, 1); //writing
            		close(fd); 			
            		argv[counter] = NULL; 			
            		argv[counter+1] = NULL; 						
            		counter++; 									
        	}
        	execvp(argv[0], argv); 		//commands executed
      	}else if(pid < 0){			
        	printf("%s", "Redirection Error\n"); 	
    	}else if(pid > 0){				//parent
        	waitpid(pid, NULL, WCONTINUED); 	//wait till child process finished
    	}
    	dup2(read, 0); 	//restoring and then closing
    	close(read); 		
    	dup2(write, 1); 	
    	close(write); 		
}



int main(int argc, char* argv[]){


	setenv("SHELL", "/home/TU/tui79498/demolab0/myshell", 1);
	if(argc == 1){
		char *commandInput; 		//commandline input
        	while(1){			//endless loop to maintain shell running
        		shellPath();
			commandInput = readInput(); 
			if(strcmp(commandInput, "quit\n") == 0 || feof(stdin)){
				return EXIT_SUCCESS; 	//command for quit/exiting shell
			}
            		int activate = parseEntire(commandInput); 
        	}
		
	} else if(argc == 2){
		FILE *fileptr;			//used to open txt file
		char batch[1024];		
		char *batchCommandInput;
		char *filetext[] = {"batchfile.txt"};
		fileptr = fopen(filetext[0], "r"); //opened in read only with batch array to hold input
		while(fgets(batch,sizeof(batch), fileptr) != NULL){
			batchCommandInput = batch;
			int activate = parseEntire(batchCommandInput);
		}
		fclose(fileptr);
        }
}


