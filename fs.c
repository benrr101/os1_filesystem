////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Functions
//
// @file	fs.c
// @descrip	This file implements the functions that perform manipulations
//		and lookups on the virtual filesystem.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "fs.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

void createFS(char *name) {
	// Prompt the user for the maximum FS size
	printf("Enter the maximum size for this filesystem in MB [10]: ");
	
	// Get the input from user
	char buf[20];
	gets(buf);
	// Process the input
	UINT maxSize;
	int replacements = sscanf(buf, "%u", &maxSize);
	if(replacements == 0 || replacements == EOF) {
		// User didn't provide a filesystem size
		maxSize = 10;
	}

	// Verify the FS size is within the bounds
	if(maxSize > MAX_FS_SIZE || maxSize < MIN_FS_SIZE) {
		printf("Invalid filesystem size. Size must be between %dMB and %dMB\n",
			MIN_FS_SIZE, MAX_FS_SIZE);
		exit(1);
	}
	maxSize = maxSize * FS_SIZE_MULTIPLE;

	// Prompt the user for the cluster size
	printf("Enter the cluster size for this filesystem in KB [8]: ");
	gets(buf);
	UINT clusterSize;
	replacements = sscanf(buf, "%u\n", &clusterSize);
	if(replacements == 0 || replacements == EOF) {
		// User did not provide a cluster size
		clusterSize = 8;
	}
	clusterSize = clusterSize * 1024;

	// Verify the cluster size is within the bounds AND divisible by 128
	if(clusterSize > MAX_CLUSTER_SIZE ||
	   clusterSize < MIN_CLUSTER_SIZE ||
	   clusterSize % CLUSTER_SIZE_MOD != 0) {
		printf("Invalid cluster size. Size must be between %dKB and %dKB and\n",
			MIN_CLUSTER_SIZE, MAX_CLUSTER_SIZE);
		printf("must be divisible %d bytes\n", CLUSTER_SIZE_MOD);
		exit(1);
	}

	// Verify the filesystem is physically possible
	UINT trueMax = (clusterSize / sizeof(FatEntry)) * clusterSize;
	if(trueMax < maxSize) {
		printf("Filesystem geometry is impossible. Please select new parameters\n");
		exit(1);
	}

	// Everything checks out so we can create a new filesystem file
	fsFile = fopen(name, "w+");
	if(fsFile == NULL) {
		fprintf(stderr, "Could not create a new filesystem\n");
		perror("fopen");
		exit(errno);
	}

	// Store the MBR data in the MBR global
	fsBootRecord.clusterSize = clusterSize;
	fsBootRecord.maxSize     = maxSize;
	fsBootRecord.rootDir     = 1;
	fsBootRecord.fatTable    = 2;

	// @TODO: Create a root directory table
	
	// @TODO: Create a FAT
	
	// Flush the boot record to the FS file
	flushBootRecord();
}

void flushBootRecord(void) {
	// Seek to the beginning of the filesystem file
	fseek(fsFile, 0, SEEK_SET);
	
	// Write out the bytes we know we have (eg stuff in the struct)
	fwrite(&fsBootRecord, sizeof(MBR), 1, fsFile);

	// Until we reach the size of a cluster write zeros
	UINT bytesWritten = sizeof(MBR);
	char zero = 0x0;
	while(bytesWritten < fsBootRecord.clusterSize) {
		fwrite(&zero, sizeof(char), 1, fsFile);
		++bytesWritten;
	}
}

void loadFS(char *name) {
	// Load the file from the disk
	fsFile = fopen(name, "r+");
	
	// Load the MBR into the mbr global
	fseek(fsFile, 0, SEEK_SET);
	fread(&fsBootRecord, sizeof(MBR), 1, fsFile);

	printf("Cluster Size: %d\n", fsBootRecord.clusterSize);
	printf("MaxFS   Size: %d\n", fsBootRecord.maxSize);
	printf("rootDir Addr: %d\n", fsBootRecord.rootDir);
	printf("fatTableAddr: %d\n", fsBootRecord.fatTable);
}
