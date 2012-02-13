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
	UINT maxSize;
	int replacements = scanf("%u", maxSize);
	if(replacements == 0 || replacements == EOF) {
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
	UINT clusterSize;
	replacements = scanf("%u", clusterSize);
	if(replacements == 0 || replacements == EOF) {
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
	// @TODO: Flush the MBR
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
