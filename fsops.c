////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Operations
//
// @file	fsops.c
// @descrip	This file implements all the functions for handling complex
// 		operations to be performed on the filesystem. They can handle
// 		their own argument parsing and are basically like standalone
// 		programs.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "os1shell.h"
#include "fs.h"
#include "fswrite.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

void touch(char *command) {
	// Copy the command to a temp var for destruction by strtok
	char commandCpy[BUFFER_SIZE];
	strncpy(commandCpy, command, BUFFER_SIZE);

	// Grab the filename from the command
	char fileName[112];
	memset(fileName, 0x0, 112);
	
	// Do the first strtok
	strtok(commandCpy, " ");
	int tokens = 0;		// Starting at 0 b/c the touch doesn't count

	char *tok = strtok(NULL, " ");
	
	// Loop over the string to find all filenames
	while(tok != NULL) {
		// Increment the count of tokens
		++tokens;

		// Copy the filename into the filename buffer
		strncpy(fileName, tok, 112);

		// Grab the next token
		tok = strtok(NULL, " ");

		// Create the file
		UINT result = createFile(fileName);
		if(result == 0) {
			// There was a failure
			printf("Error: Could not create file %s\n", fileName);
		}
	}

	// If we still have no filenames, then print the usage info
	if(tokens == 0) {
		printf("Error: Missing filename operand\n");
		printf("Usage: touch filename\n");
	}
}
