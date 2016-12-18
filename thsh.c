/* COMP 530: Tar Heel SHell */
//I certify that no unauthorized assistance has been received or given in the completion of this work
//Brandon Clark

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "Functions.h"
int pid;

void sigbg(int p){
	//print("crtz");
    signal(SIGTSTP,&sigbg);

    kill(pid, SIGTSTP);
}

// Ctrl+C
void sigkill(int p){
    signal(SIGINT,&sigkill);  
    kill(pid, SIGKILL);
    print("\n");
}


int main (int argc, char ** argv, char **envp) {

	int finished = 0;
	char rawinput[MAX_INPUT];
	int debug = 0;
	int commandreturn = 0;
	int bg;


    signal(SIGTSTP, &sigbg);
    signal(SIGINT, &sigkill);

	//check for debug enabled
	if(argc > 1){
		int i;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "-d") == 0){
				debug = 1;
			}

			struct stat filestat;
			if(stat(argv[i], &filestat) == 0){
				int in = open(argv[i], O_RDWR);
				dup2(in, 0);
			}
		}
	}

	while (!finished) {
		char *cursor;
		int rv;
		int set = 0;


    	// Print the prompt
		rv = printprompt();

		if (!rv) { 
			finished = 1;
			break;
		}

    	// read and parse the input
		memcpy(rawinput, getinput(rv), sizeof(rawinput));

		if (!rv) { 
			finished = 1;
			break;
		}

		//Parse arguments and place into arg array
		int cmdcount = countcommands(rawinput);

		char ***commandlist = parsearguments(rawinput);		

		int argcount[cmdcount];
		//printint(cmdcount);
	    memcpy(argcount, countargs(rawinput), sizeof(argcount));
	    // for(int i = 0; i < cmdcount; i++){
	    // 	printint(argcount[i]);
	    // }
		if(debug){
			Debug("# Commands: ");
			Debugint(cmdcount);
			Debug("\n");
			Debug("# Arguments: [");
			int i;
			for(i = 0; i < cmdcount; i++){
		    	Debugint(argcount[i]);
		    	if(i != cmdcount-1)
		    		Debug(", ");
		    }

			Debug("]\n\nCommand List\n");
		 	for(i = 0; i < cmdcount; i++){
		 		Debugint(i);
		 		Debug(": ");
		 		int j;
				for(j = 0; j < argcount[i]; j++){
					Debug(commandlist[i][j]);
					if(j != argcount[i]-1)
						Debug(", ");
				}
				Debug("\n");
			}
			Debug("\n");
		}

		//Check if command should run in the background
		if(*commandlist[cmdcount-1][argcount[cmdcount-1]-1] == '&'){
			bg = 1;
			commandlist[cmdcount-1][argcount[cmdcount-1]-1] = 0;
			argcount[cmdcount-1]--;
		}

		//Check if command is set so we can avoid changing env vars in the command to their values
		if(strcmp("set", commandlist[0][0]) == 0){
			set = 1;
		}

		//replace environment vars with their values
		if(!set){
			int i;
			for(i = 0; i < cmdcount; i++){
				int j;
				for(j = 0; j < argcount[i]; j++){
					if(*commandlist[i][j] == '$'){
						//Set the special return value variable
						if(strcmp(commandlist[i][j], "$?") == 0){

							char tmp[sizeof(commandreturn)];

							snprintf(tmp, sizeof(commandreturn), "%d", commandreturn);
							//printf("rv: %d\n", commandreturn);
							//write(1, "test", strlen("test"));
							//write(1, tmp, strlen(tmp));

							commandlist[i] = malloc(strlen(tmp) + 1);
							strcpy(commandlist[i][j], tmp);
							break;
						}

						//Otherwise replace the env var with its corresponding value

						cursor = commandlist[i][j];
						cursor++;

						char tmp[MAX_INPUT];
						memset(&tmp[0], 0, sizeof(tmp)); 


						int k = 0;
						while(*cursor != '\0' && *cursor != ' '){
							tmp[k] = *cursor;
							k++;
							cursor++;
						}
						//write(1, tmp, strlen(tmp));
						//write(1, "\n", strlen("\n"));

						char tmp2[MAX_PATH];
						snprintf(tmp2, MAX_PATH, "%s", getenv(tmp));


						commandlist[i][j] = malloc(strlen(tmp2) + 1);

						strcpy(commandlist[i][j], tmp2);
						// printint(i);
						// printint(j);
						// print(commandlist[i][j]);
					}
				}
			}
		}


		int input = 0;

		//check for input redirection
		if(argcount[0] > 2){
			int i;
			for(i = 0; i < argcount[0]; i++){
				if(*commandlist[0][i] == '<'){	
					//Debug("changing in\n");
					input = open(commandlist[0][i+1], O_RDWR);
					//printint(input);
					int j;
					for(j = i; j < argcount[0]; j++){
						if(commandlist[0][j+1] != NULL)
							commandlist[0][j] = commandlist[0][j+1];
					}
					argcount[0]--;
					for(j = i; j < argcount[0]; j++){
						if(commandlist[0][j+1] != NULL)
							commandlist[0][j] = commandlist[0][j+1];
					}
					argcount[0]--;
					break;
				}
			}
		}
		
		//Debug message
		if(debug){
			char tmp[MAX_PATH];
			snprintf(tmp, MAX_PATH, "RUNNING: %s\n", commandlist[0][0]);
			write(2, tmp, strlen(tmp));
		}

		//Check if command is built im, otherwise attempt to run a binary
		if(checkifbuiltin(commandlist[0][0])){
			runbuiltin(commandlist);
		}
		else if(!checkifbuiltin(commandlist[0][0])){
			//Run command
			int p[2];
			

			while (*commandlist != NULL)
			{
				//Create pipe then create child
				pipe(p);
				pid = fork();

				if (pid == 0)
				{
					//Child process
		          	dup2(input, STDIN);
		          	close(input);

		          	//If another command exist, we need to pipe our output to it
		         	if (*(commandlist + 1) != 0){
		         		dup2(p[1], STDOUT);
		         	}
		         	
		         	//Check to see if output redirection is specified
		         	//Only when we have reached the last command (or a single command)
		         	if(*(commandlist+1) == NULL){
		         		// Debug(commandlist[0][0]);
		         		// Debug("-");
		         		// Debugint(argcount[0]);
		         		// Debug("\n");
			         		int index = 0;

			         		//go through arguments and try to find the > char
			         		while(commandlist[0][index + 1] != NULL){
					         	if(strchr(commandlist[0][index], '>') != NULL){
					         		//Debug("test");

					         		// Debug("\nChanging out to: ");
					         		// Debug(commandlist[0][index + 1]);
					         		// Debug("\n");

					         		//If output is detected, set the stdout to our file
									int output = open(commandlist[0][index + 1], O_RDWR|O_CREAT|O_TRUNC, 0777);

									if(*commandlist[0][index] != '>'){
										//Debug("redirecting out again");
										char *pointer = strchr(commandlist[0][index], '>');
										*pointer = 0;
										//*commandlist[i][j] = '\0';

										//print(commandlist[cmdcount-1][argcount[cmdcount-1]-2]);

										//Debug(commandlist[0][index]);
										//Debug("test");
										dup2(output, *commandlist[0][index] - '0');
									}
									else{
										dup2(output, STDOUT);
									}
									close(output);
									commandlist[0][index] = 0;
									
								}
								//Debug("test");
								if(commandlist[0][index+1] != NULL){
									index++;
								}
							}
						
					}
		         	
		         	close(p[0]);

				   	//Debug("Command list before exec\n");
				   	//       	for(int i = 0; i < cmdcount; i++){
					// 	for(int j = 0; j < argcount[i]; j++){
					// 		Debug(commandlist[i][j]);
					// 		Debug("_");
					// 	}
					// 	Debug("\n");
					// }
					// Debug("\n");
					// Debug()

		         	execvp(*commandlist[0], *commandlist);
		         	Debug(commandlist[0][0]);
		         	Debug(" is not a valid command.");
		         	Debug("\n");
		         	exit(-1);
		    }
		    else if (pid == -1){
				//Fork did not work
				Debug("Failed to fork.");
				exit(-1);
			}
		    else{
		      	//Parent code

		      	//Wait for child to return if it is running in foreground
		      	if(!bg){
		      		waitpid(pid, &commandreturn, WUNTRACED);
		      	}

		      	close(p[1]);
		        input = p[0];

		        //Break if we are on the last command, else go to the next command
		        if(*(commandlist+1) == NULL)
		        	break;
		        else
		        	commandlist++;

		        //move the argument counts to match our new commandlist
		        int i;
		        for(i = 0; i < cmdcount; i++){
		        	if(argcount[i+1] != '\0'){
		        		argcount[i] = argcount[i+1];
		        	}
		        }
		        cmdcount--;
		        
		    }
			}
		}
		//
		//Debug message
		if(debug){
			char tmp[MAX_PATH];
			snprintf(tmp, MAX_PATH, "ENDED: %s (ret=%d)\n", commandlist[0][0], commandreturn);
			write(2, tmp, strlen(tmp));
		}
	}

	return 0;
}

