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
#include <time.h>

#include "os1shell.h"
#include "fs.h"
#include "fsread.h"
#include "fswrite.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

void ls(char *command) {
	// Goto the directory table and get an entry
	FSPTR dirCluster = fsBootRecord.rootDir;

	// Iterate until we get to the end of the directory table
	while(dirCluster != FAT_EOC) {
		// Get the address of the cluster and go to it
		UINT currentAddr = dirCluster * fsBootRecord.clusterSize;
		UINT endOfTable  = currentAddr + fsBootRecord.clusterSize;
		fseek(fsFile, currentAddr, SEEK_SET);

		// Iterate until we get to the end of the cluster
		while(currentAddr < endOfTable) {
			// Read in a directory entry
			DirectoryEntry e;
			fread(&e, sizeof(DirectoryEntry), 1, fsFile);
			
			// Only output a line if the file is not available
			// Calculate the creation date... damn.
			struct tm * timeTM;
			time_t time = e.creationDate;
			timeTM = localtime(&time);
			char date[13];
			strftime(&date, 13, "%b %d %H:%M", timeTM);
			
			if(e.fileName[0] != DIR_ENTRY_AVAILABLE &&
				e.fileName[0] != DIR_ENTRY_DELETED) {
				printf("%s %d\t%s\n", date, e.size, e.fileName);
			}
		
			// Advance the internal pointer
			currentAddr += sizeof(DirectoryEntry);
		}

		// Get the next cluster from FAT
		dirCluster = lookupFAT(dirCluster);
	}
}
		

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

void rm(char *command) {
	// Copy the command to a temp var for destruction by strtok
	char commandCpy[BUFFER_SIZE];
	strncpy(commandCpy, command, BUFFER_SIZE);
	char fileName[112];
	memset(fileName,0x0, 112);
	
	// Do the first strtok and start counting tokens
	strtok(commandCpy, " ");
	int tokens = 0;
	char *tok = strtok(NULL, " ");
	while(tok != NULL) {
		// Increment the count of tokens
		++tokens;

		// Copy the filename into the buffer and get next token
		strncpy(fileName, tok, 112);
		tok = strtok(NULL, " ");
		
		// Delete it up
		removeFile(fileName);
	}

	// If we still have no filenames, then print usage info
	if(tokens == 0) {
		printf("Error: Missing filename operand\n");
		printf("Usage: rm filename\n");
	}
}
