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

	//@DEBUG

	// SETUP PROCEDURES ////////////////////////////////////////////////
	// Initialize the history list
	historyList = historyInit(MAX_HISTORY);
	
	// Initialize the command buffer
	char command[BUFFER_SIZE];
	memset(command, 0, BUFFER_SIZE);

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
		} else if(strstr(command, TOUCH_COMMAND) != NULL) {
			// @TODO: verify that we're in the filesystem
			touch(command);
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
