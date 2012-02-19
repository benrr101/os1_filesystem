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

// FUNCTIONS ///////////////////////////////////////////////////////////////

void touch(char *command) {
	// Grab the filename from the command
	
	char fileName[112];
	printf("Replacements made: %d\n", sscanf(command, "%*s %s", fileName));
}
