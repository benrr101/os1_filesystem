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
#include "fsops.h"
#include "fsread.h"
#include "fswrite.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

void cat(char *command) {
	//Copy the command to a temp var for destruction by strtok
	char commandCpy[BUFFER_SIZE];
	strncpy(commandCpy, command, BUFFER_SIZE);
	char fileName[112];
	memset(fileName,0x0, 112);

	// Do the first strtok and start counting tokens
	strtok(commandCpy, " ");
	int tokens = 0;
	char *tok = strtok(NULL, " ");
	if(tok != NULL) {
		// We have a valid filename so look it up
		// Step 0) Copy the token into the fileName buffer
		strncpy(fileName, tok, 112);

		// Step 1) Lookup the directory entry
		UINT dirAddr = getDirTableAddressByName(fileName);
		fseek(fsFile, dirAddr, SEEK_SET);
		DirectoryEntry dir;
		fread(&dir, sizeof(DirectoryEntry), 1, fsFile);

		// Step 2) Setup for the loop
		FSPTR cluster  = dir.index;
		UINT bytesRead = 0;

		// STEP 3) LOOP TIL THE END OF THE CHAIN
		while(cluster != FAT_EOC) {
			// Get the info about the cluster
			UINT clusterStart = cluster * fsBootRecord.clusterSize;
			UINT currentAddr  = clusterStart;
			UINT clusterEnd= clusterStart+fsBootRecord.clusterSize;
			fseek(fsFile, clusterStart, SEEK_SET);

			// LOOP UNTIL AT END OF CLUSTER OR EOF
			while(currentAddr < clusterEnd && bytesRead<dir.size) {
				// Read a byte, print a byte
				char c;
				fread(&c, sizeof(char), 1, fsFile);
				printf("%c", c);

				// Increment byte count and address
				++currentAddr;
				++bytesRead;
			}

			// Load up the FAT entry for this cluster
			cluster = lookupFAT(cluster);
		} 

		// Newline for clarity sake
		printf("\n");
	} else {
		// We don't have a valid filename so print an error
		printf("Error: Missing file operand\n");
		printf("Usage: cat filename\n");
	}
}

void cp(char *command, int inFS, char *fsPath) {
	// Determine what mode we're going to use to do the copying
	// Step 0) Duplicate the command so we don't destroy it
	char commandCpy[BUFFER_SIZE];
	strncpy(commandCpy, command, BUFFER_SIZE);	

	// Step 1) Tokenize the command into the cp and 2 operands
	strtok(command, " ");
	char *source = strtok(NULL, " ");
	char *dest   = strtok(NULL, " ");

	// Verify that we have no more and no less tokens
	if(source == NULL || dest == NULL || strtok(NULL, " ") != NULL) {
		printf("Error: Missing file operand\n");
		printf("Usage: cp source_file destination_file\n");
	}

	// Step 2) Is the source in the filesystem?
	int fromFS = false;
	if(
		strstr(source, fsPath) == source		// Eg: /fsName/file
		|| (inFS && getDirTableAddressByName(source)) 	// Eg: fileinfs
	) {
		// Source is in the filesystem
		fromFS = true;

		// Remove the fsPath from the source path if needed
		if(strstr(source, fsPath) == source) {
			strtok(source, "/");
			source = strtok(NULL, "/");
		}
	}

	// Step 3) Is the destination in the filesystem?
	int toFS = false;
	if(
		strstr(dest, fsPath) == dest		// Eg: /fsName/file
		|| inFS && strstr(dest, "/") != dest	// Doesn't start with /
	) {
		// Destination is in the filesystem
		toFS = true;
		
		// Strip off the /fs
		if(strstr(dest, fsPath) == dest) {
			strtok(dest, "/");
			dest = strtok(NULL, "/");
		}
	}
	//@DEBUG:
	printf("FROM FS %d -> TO FS%d\n", fromFS, toFS); 
	
	// CASES ///////////////////////////////////////////////////////////
	if(!fromFS && !toFS) {
		// Case 1: Copying from root fs to root fs
		// Tell the OS to handle this
		printf("%s\n", commandCpy);
		execCommand(commandCpy);
	} else if(fromFS && !toFS) {
		// Case 2: Copying from the filesystem into the root fs
		printf("Copying from FS to root FS\n");
		cpFromFStoRootFS(source, dest);
	} else if(toFS && !fromFS) {
		// Case 3: Copying from root fs to the filesystem
		printf("Copying from root FS to FS\n");
	} else {
		printf("Copying from fs to fs\n");
	}
}

void cpFromFStoRootFS(char *source, char *dest) {
	// Does the file exist?
	UINT dirAddr = getDirTableAddressByName(source);
	if(dirAddr == 0) {
		printf("Error: Source file does not exist\n");
		return;
	}

	// Open the destination file
	FILE *destFile = fopen(dest, "w");
	if(destFile == NULL) {
		// Destination file could not be opened
		fprintf(stderr, "Could not open destination file\n");
		perror("fopen");
		return;
	}

	// Open the directory entry of the source file
	DirectoryEntry dir;
	fseek(fsFile, dirAddr, SEEK_SET);
	fread(&dir, sizeof(DirectoryEntry), 1, fsFile);

	// Goto the start of the source file
	FSPTR cluster = dir.index;
	UINT bytesRead = 0;

	// Loop until the end of the chain of clusters for the file
	while(cluster != FAT_EOC) {
		// Get info about the cluster
		UINT clusterStart = cluster * fsBootRecord.clusterSize;
		UINT currentAddr  = clusterStart;
		UINT clusterEnd   = clusterStart + fsBootRecord.clusterSize;
		fseek(fsFile, currentAddr, SEEK_SET);

		// Loop until end of the cluster or file
		while(currentAddr < clusterEnd && bytesRead < dir.size) {
			// Read a byte, write a byte
			char c;
			fread(&c, sizeof(char), 1, fsFile);
			fwrite(&c, sizeof(char), 1, destFile);

			// Increment the counter
			++bytesRead;
		}

		// Get the next cluster of the file
		cluster = lookupFAT(cluster);
	}

	// Close up the destination file
	fclose(destFile);
}


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
