/* COMP 530: Tar Heel SHell */
//I certify that no unauthorized assistance has been received or given in the completion of this work
//Brandon Clark

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include "definitions.h"

int Sum(int a, int b);
int print(char *);
int printprompt();
char * getinput(int rv);
int * countargs(char * input);
int countcommands(char * input);
int printint(int number);
char *** parsearguments(char * rawinput);
int runbuiltin(char *** arguments);
int checkifbuiltin(char * arguments);
int Debug(char * string);
int Debugint(int number);
#endif