////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	os1shell.c
// @descrip	This file contains the main entry point for the program. It
//		will set up the infinite loop for printing a prompt and
//		executing child processes. It will also take care of setting
//		up anything prior to presenting the prompt.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "os1shell.h"
#include "history.h"
#include "signalHandlers.h"
#include "exec.h"

#include "fs.h"
#include "fsinit.h"
#include "fsread.h"
#include "fswrite.h"
#include "fsops.h"

// MAIN FUNCTION /////////////////////////////////////////////////////////////

void trim(char *string) {
	// Set up temp pointers
	char *last = string;

	// Iterate to the end of the string (ie, null byte)
	while(*last != '\0') {
		++last;
	}

	// Now that we're at the end, start backing up until we find abc
	while(*last == ' ' || *last == '\n' || *last == '\r' || *last == '\t'
		|| *last == '\0'){
		--last;
	}

	// Now that we have non-whitespace, advance one and place the null byte
	*(++last) = '\0';
}


/**
 *
 */
int main(int argc, char *argv[]) {
	// ARGUMENT PROCESSING /////////////////////////////////////////////
	if(argc != 2) {
		// There is not the correct number of arguments
		printf("Usage: os1shell fsname\n");
		printf("  fsname - name of existing virtual filesystem or one you wish to create\n");
		exit(0);
	}

	// Check if the filesystem exists
	FILE *temp = fopen(argv[1], "r");
	if(temp) {
		// Filesystem exists. Close it up and load it
		fclose(temp);
		loadFS(argv[1]);
	} else {
		// Filesystem does not exist. We need to create it
		createFS(argv[1]);
	}

	// SETUP PROCEDURES ////////////////////////////////////////////////
	// Initialize the history list
	historyList = historyInit(MAX_HISTORY);
	
	// Initialize the command buffer
	char command[BUFFER_SIZE];
	memset(command, 0, BUFFER_SIZE);

	// We're initially in the filesystem
	int inFS = true;

	// Compute the address of the filesystem
	char path[100];
	strcpy(path, ROOT_DIR);
	strcat(path, argv[1]);

	// Change signal dispositions
	registerHandlers();

	// MAIN LOOP ///////////////////////////////////////////////////////
	while(true) {
		// Print the prompt and flush since it's line buffered
		printf(PROMPT);
		fflush(stdout);

		// Read into the prompt from standard in
		int bytesread = read(0, command, BUFFER_SIZE);
		if(bytesread < 0) {
			// Error! Couldn't read
			perror("read");
			exit(errno);
		}

		
		// We need to trim the command for any trailing \n, etc
		command[bytesread] = '\0'; 
		trim((char *)&command);

		// How I'll detect EOF at the end of a line:
		// Original # of bytes read in == # of bytes after trimming
		// It's janky, but theoretically it'll work
		if(bytesread == 0 || bytesread == strlen(command)) {
			// We received an end of file, so exit
			exit(0);
		} else if(strlen(command) == 0) {
			// Ignore empty commands
			continue;
		} else if(strncmp(command, HISTORY_COMMAND, bytesread) == 0) {
			// Output the history and go back
			historyPrint(historyList);
		} else if(strstr(command, CD_COMMAND) == command) {
			// Are we switching into the filesystem?
			if(
				strstr(command, path) != NULL &&
				strlen(strstr(command, path)) == strlen(path)
			) {
				// Yep, switching into the filesystem
				inFS = true;
			} else {
				// Nope, switching out of the filesystem
				inFS = false;
				// Change the working directory. Kinda sketch.
				// BUT cd is not actually a command in /bin
				// So tokenize the string to get the path
				strtok(command, " ");
				char *ptr = strtok(NULL, " ");
				if(ptr != NULL && strtok(NULL, " ") == NULL) {
					// Changing directories
					chdir(ptr);
				} else {
					// Invalid destination
					printf("Error: Missing path operand\n");
					printf("Usage: cd path\n");
				}
			}
		} else if(strstr(command, PWD_COMMAND) == command) {
			// Print the working directory
			if(inFS) {
				// We're in the filesystem, so print that
				char path[100];
				strcpy(path, ROOT_DIR);
				strcat(path, argv[1]);
				printf("%s\n", path);
			} else {
				// We're not in the filesystem, so chain it
				execCommand(command);
			}
		} else if(strstr(command, TOUCH_COMMAND) == command) {
			// Verify that we're in the filesystem
			if(inFS) {
				touch(command);
			} else {
				execCommand(command);
			}
		} else if(strstr(command, RM_COMMAND) == command) {
			// Verify that we're in the filesystem
			if(inFS) {
				rm(command);
			} else {
				execCommand(command);
			}
		} else if(strstr(command, LS_COMMAND) == command) {
			// Verify we're in the filesystem
			if(inFS) {
				ls(command);
			} else {
				execCommand(command);
			}
		} else if(strstr(command, CAT_COMMAND) == command) {
			// Verify we're in the filesystem
			if(inFS) {
				cat(command);
			} else {
				execCommand(command);
			}
		} else if(strstr(command, CP_COMMAND) == command) {
			// Pass the call to the fancy cp function
			cp(command, inFS, path);
		} else if(strstr(command, MV_COMMAND) == command) {
			// Pass the call to the fancy mv function
			mv(command, inFS, path);
		} else if(strstr(command, DF_COMMAND) == command) {
			// Pass the call to the df function
			df();
		} else {
			// Now that we have a complete command, we'll exec it
			execCommand(command);
		}

		// Create a history entry for this command
		historyAdd(historyList, command);
	}

	// CLEAN UP ////////////////////////////////////////////////////////
	// Tear down the history
	historyDestroy(historyList);
}
