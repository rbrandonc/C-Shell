/* COMP 530: Tar Heel SHell */
//I certify that no unauthorized assistance has been received or given in the completion of this work
//Brandon Clark

#include "Functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int goheels(){
int rv =
print("                                                         D7DDD\n");
print("                  DDDDDD                               .D=====IDDD\n");              
print("              DDDD=====ID          DDDDDDDDDDDDD?     DD===========DDD\n");          
print("          DDDI===========DDDDDDDD==================DDDD===============DD\n");        
print("         DD==========================================================D\n");          
print("          DD========================================================D\n");           
print("           DD======================================================DD\n");           
print("            DD========================================================DD\n");        
print("          DD============================================================DD\n");      
print("        DD================================================================DD\n");    
print("      DD====================================================================DD\n");  
print("     D===============================DDDDDDDDDDDDD?==========================DD\n"); 
print("   8D=================================DD         D?=========================DD\n");  
print("  DD====================================DD       D?=======================DD\n");    
print("  D======================================DD      D?============D=========DD\n");     
print(" D===============DD========================D8    D?============DDD=====7D\n");       
print("DD==============DDDZ========================DD   D?===========ZD  DD==DD\n");        
print("D==============D   D==========================DZ D?===========DD   DDD\n");          
print("D=============D    D===========================DDD?===========DD\n");                
print("D============DD    D=============================D?===========DD\n");                
print("D============DD    DD=============================?===========DD\n");                
print("D=============D    $D===========D=============================ZD    $D\n");          
print("D==============D    D===========DDO============================D   DD=D\n");         
print("DD==============DD ~D===========D DD===========================D  DD===DD\n");       
print(" D===============DDDD===========D   D$=========================DDD======DD\n");      
print("  D================DD===========D    DD===================================DD\n");    
print("  DD============================D      D===================================DD\n");   
print("   DD===========================D       DD===================================D\n");  
print("     D==========================DDDDDDDDDDD==================================+D\n"); 
print("      DD====================================================================DD\n");  
print("        DD================================================================DD\n");    
print("          DD============================================================DD\n");      
print("            DD========================================================DD\n");        
print("              7D====================================================DD\n");    
print("              D======================================================\n");     
print("             D========================================================D\n");     
print("           DD==============DDDDDD==================DDDD=============DDD\n");         
print("         DDDDD============DD       DDDDDDDDDDDDD$      DD========DDD\n");            
print("              8DDDD======DD                             ~D===IDDO\n");               
print("                   IDDDDDD                                DDD\n");
	return rv;
}

int Sum(int a, int b){
	return a+b;
}

int print(char * string){
	int rv = write(1, string, strlen(string));
	return rv;
}

int Debug(char * string){
	int rv = write(2, string, strlen(string));
	return rv;
}

int Debugint(int number){
	char temp[sizeof(int)];
	snprintf(temp, sizeof(temp), "%d", number);
	int rv = write(2, temp, strlen(temp));
	return rv;
}

int printint(int number){
	char temp[sizeof(int)];
	snprintf(temp, sizeof(temp), "%d", number);
	int rv = write(1, temp, strlen(temp));
	return rv;
}

int printprompt(){
	char prompt[MAX_PATH];
	snprintf(prompt, MAX_PATH, "[%s] thsh> ", getenv("PWD"));

	int rv = print(prompt);
	return rv;
}

//Gets the input from command line
char * getinput(int rv){
	char *rawinput = malloc(MAX_INPUT);
	char *cursor;
	int count;
	char last_char;

	for(rv = 1, count = 0, cursor = rawinput, last_char = 1; 
			rv && (++count < (MAX_INPUT-1)) && (last_char != '\n'); 
			cursor++) { 

			rv = read(0, cursor, 1);

		last_char = *cursor;
	} 

	*cursor = '\0';

	return rawinput;
}

//Counts the number of arguments for each command, and returns the values in an array
int * countargs(char * input){
	int cmdcount = countcommands(input);
	int *argcount = malloc(cmdcount*sizeof(int));
	memset(argcount, 0, cmdcount*sizeof(int));
	char *cursor;
	cursor = input;
	int index = 0;

	while(*cursor != '\0'){

		if(*cursor == '<' || *cursor == '>'){
			argcount[index]++;
			cursor++;
		}

		if(*cursor != '\0' && *cursor != '|' && *cursor != ' ' && *cursor != '\n'){
			argcount[index]++;
		}

		while(*cursor != ' ' && *cursor != '|' && *cursor != '\0'){
			if(*cursor == '<' || *cursor == '>'){
				if(*(cursor+1) != ' ' && *(cursor+1) != '|' && *(cursor+1) != '\0' && *(cursor+1) != 'n')
					argcount[index]++;
			}
			cursor++;
		}

		while(*cursor == ' '){
			cursor++;
		}

		if(*cursor == '|'){
			index++;
			cursor++;
		}
	}

	// for(int i = 0; i < cmdcount; i++){
	// 	print("number of args counted:");
	// 	printint(argcount[i]);
	// }


	return argcount;
}

//Counts the number of commands seperated by pipes in an input
int countcommands(char * rawinput){
	int cmdcount = 0;
	char *cursor;
	cursor = rawinput;

	while(1==1){
		if(*cursor == '\0'){
			cmdcount++;
			break;
		}
		if(*cursor == '|'){
			cmdcount++;
		}
		cursor++;
	}

	return cmdcount;
}
//
//Stores commands seperated by a pipe in an array in the following form:
//ls, -l, -a
//grep, "txt"
//wc, -l, >, output.txt
char *** parsearguments(char * rawinput){
		int cmdcount = countcommands(rawinput);
		int argcount[cmdcount];
	    memcpy(argcount, countargs(rawinput), sizeof(argcount));

		char ***commandlist = (char***) malloc((cmdcount+1)*sizeof(char **));
		char *cursor = rawinput;
		int cmdno;
		for(cmdno = 0; cmdno < cmdcount; cmdno++){
			// printint(cmdno);
			// print("\n");
	    	//Create the array of arguments to pass to exec
	    	//Number of arguments +1 for null terminator
			commandlist[cmdno] =  malloc(sizeof(char *));
			memset(commandlist[cmdno], '\0', sizeof(char *));

			char **currentcommand = malloc(sizeof(char *) * (argcount[cmdno]+1));
			int i;
			for(i = 0; i < argcount[cmdno]; i++){
				char arg[MAX_INPUT];
				memset(&arg[0], 0, sizeof(arg));

				int j = 0;

			//Process each argument
				if(*cursor == ' '){
						while(*cursor == ' '){
								cursor++;
						}
				}
				while(1==1){
					//print(cursor);
					if(*cursor == '|'){
						cursor++;
						if(*cursor == ' '){
							while(*cursor == ' '){
								cursor++;
							}
						}
					}

					if(*cursor == '~'){
						strcpy(arg, getenv("HOME"));
						j += strlen(getenv("HOME"));
						cursor++;
					}

					if(*cursor == '"'){
						cursor++;
						while(*cursor != '"' && *cursor != '\n'){
							arg[j] = *cursor;
					
							cursor++;
							j++;
						}
						if(*cursor == '"'){
							cursor++;
						}
					}

					if(*cursor == '\''){
						cursor++;
						while(*cursor != '\'' && *cursor != '\n'){
							arg[j] = *cursor;
					
							cursor++;
							j++;
						}
						if(*cursor == '\''){
							cursor++;
						}
					}
					
					if(*cursor == '\n'){
						break;
					}
					

					if(*cursor == ' '){
						arg[j] = '\0';
						cursor++;
						break;
					}

					if(*cursor == '<' || *cursor == '>'){
						arg[j] = *cursor;
						arg[j+1] = '\0';
						cursor++;
						if(*cursor == ' '){
							while(*cursor == ' '){
								cursor++;
							}
						}
						break;
					}


					arg[j] = *cursor;
					
					cursor++;
					j++;

				}

				// print("arg: ");
				// print(arg);
				// print(" ");
				// print("\n");
				

				//Allocate space for each argument then copy it into the argument array

				currentcommand[i] = malloc(strlen(arg));
				strcpy(currentcommand[i], arg);
				
				// print(currentcommand[i]);
				// print("\n");
			}



			currentcommand[argcount[cmdno]] = 0;
			commandlist[cmdno] = currentcommand;

			

			
		}
		commandlist[cmdcount] = 0;
		
		return commandlist;
}

//returns true or false depending on if the input is a built in command
int checkifbuiltin(char * arguments){
	if(strcmp("cd", arguments) == 0 || strcmp("exit", arguments) == 0 || strcmp("set", arguments) == 0 || strcmp("goheels", arguments) == 0 || strcmp("fg", arguments) == 0 || strcmp("bg", arguments) == 0 || strcmp("jobs", arguments) == 0){
		return 1;
	}
	else{
		return 0;
	}
}

//executes a command and returns a status code
int runbuiltin(char *** commandlist){
	char * cursor;
	int commandreturn;

	//Exit if command is exit
	if(0 == strcmp(commandlist[0][0], "exit")){
		exit(0);
	}

	if(strcmp("goheels", commandlist[0][0]) == 0){
		commandreturn = goheels();
	}

	if(strcmp("fg", commandlist[0][0]) == 0){
		//kill(commandlist[0][1], SIGCONT);
	}

	if(strcmp("bg", commandlist[0][0]) == 0){
	}

	if(strcmp("jobs", commandlist[0][0]) == 0){
	}

	//If command is set, set appropriate environment variable
	else if(strcmp(commandlist[0][0], "set")==0){
		//Debug(commandlist[0][0]);
		if(*commandlist[0][1] == '$'){
			//Parse new value
			char var[MAX_INPUT];
			char val[MAX_INPUT];
			cursor = commandlist[0][1];
			cursor++;

			int i = 0;
			while(*cursor != '='){
				var[i] = *cursor;
				cursor++;
				i++;
			}

			var[i] = '\0';

			i = 0;
			cursor++;

			while(*cursor != '\0'){
				val[i] = *cursor;
				cursor++;
				i++;
			}
			// write(1, var, strlen(var));
			// write(1, val, strlen(val));
			val[i] = '\0';


			if(strcmp(var, "?") == 0){
				commandreturn = *val;
			}
			else{
				setenv(var, val, 1);
			}
		}
		else{
			Debug("Invalid variable\n");
		}
	}

	//Processing if the command is CD
	//First check if we must go to the last directory visited
	//If not, set the current directory to old pwd, and cd to the appropriate directory
	//(home or wherever the second argument dictates)
	//Then set PWD to the new directory
	if(0 == strcmp(commandlist[0][0], "cd")){

		char cwd[MAX_PATH];
		getcwd(cwd, MAX_PATH);


		if(commandlist[0][1] != NULL && strcmp("-", commandlist[0][1]) ==  0){
			commandreturn = chdir(getenv("OLDPWD"));
		}

		setenv("OLDPWD", cwd, 1);
		
		if(commandlist[0][1] == NULL){
			commandreturn = chdir(getenv("HOME"));
		}
		else{
			commandreturn = chdir(commandlist[0][1]);
		}


		setenv("PWD", getcwd(cwd, MAX_PATH), 1);

	}
	return commandreturn;
}

