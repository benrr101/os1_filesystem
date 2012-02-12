//////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - OS1Shell
// 
// @file	exec.c
// @descrip	This file defines functions for executing the commands provided
// 		as well as helper functions for generating argument lists.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <wait.h>

#include "os1shell.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

void execCommand(char* command) {
	// ARGUMENTS ///////////////////////////////////////////////////////
	int background = false;
	int argCount = 0;

	// Duplicate the command so strtok can mangle it all to hell
	char *commandPass1 = malloc(strlen(command) + 1);
	char *commandPass2 = malloc(strlen(command) + 1);
	strncpy(commandPass1, command, strlen(command) + 1);
	strncpy(commandPass2, command, strlen(command) + 1);

	// Pass 1: Get the number of arguments
	char *tok = strtok(commandPass1, " ");
	while(tok != NULL) {
		// If the token is &, then the command will be ran in the bkgrd
		if(strcmp(tok, "&") == 0) {
			background = true;

			// Stop processing tokens
			break;
		}

		// Increment the # of args
		++argCount;

		tok = strtok(NULL, " ");
	}

	// Allocate enough space to store this array
	char **argList = malloc(sizeof(char*) * (argCount + 1));

	// Pass 2: Store the arguments in the array (going by # of args)
	tok = strtok(commandPass2, " ");
	int i;
	for(i = 0; i < argCount; ++i) {
		// If we have an & at the end, then we'll put a null there
		int ampLoc = strcspn(tok, "&");
		if(ampLoc && ampLoc != strlen(tok)) {
			background = true;
			tok[ampLoc] = '\0';
		}
		
		// Store the argument
		argList[i] = tok;
		tok = strtok(NULL, " ");
	}

	// Null terminate the end of the list
	argList[argCount] = NULL;

	// FORK AND EXEC ///////////////////////////////////////////////////
	int pid = fork();
	switch(pid) {
		case 0:
			// CHILD ///////////////////////////////////////////
			if(execvp(argList[0], argList) == -1) {
				// Exec failed!
				perror("exec");
			}
			exit(0);
			break;

		case -1:
			// FAILURE /////////////////////////////////////////
			perror("fork");
			exit(errno);
			break;
		
		default:
			// PARENT //////////////////////////////////////////
			if(!background) {
				// We must wait for the child to finish
				waitpid(pid, NULL, WUNTRACED);
			}
			break;
	}

	// CLEAN UP ////////////////////////////////////////////////////////
	free(commandPass1);
	free(commandPass2);
	free(argList);
}
