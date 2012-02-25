////////////////////////////////////////////////////////////////////////////
// OS1 Final Project - Filesystem Initialization Header
//
// @file	fsinit.h
// @descrip	This file implements the functions necessary for initialization
//		of the filesystem. This includes creating/loading the
//		filesystem as well as initailization of directory clusters that
//		can happen after the filesystem is init'd.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "fs.h"
#include "fsinit.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * Writes null bytes to the maximum size of the filesystem.
 * @param	UINT	size	Mamimum size of the filesystem in bytes
 */
void allocateFS(UINT size) {
	// Seek to the beginning of the filesystem
	fseek(fsFile, 0, SEEK_SET);

	// Write out the number of bytes to the filesystem
	UINT bytesWritten = 0;
	char zero = 0x0;
	while(bytesWritten < size) {
		fwrite(&zero, sizeof(char), 1, fsFile);
		++bytesWritten;
        }
}

/**
 * Create the filesystem. Promps the user for information about the filesystem.
 * Creates the boot record, initializes the fat table, allocates the full size
 * of the fs, and initializes 1 cluster of directory table.
 * @param	char*	name	Name of the filesystem
 */
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

	// Allocate all the space for the filesystem
	allocateFS(fsBootRecord.maxSize);

	// Initialize the FAT
	initFAT(fsBootRecord.fatTable);

	// Initialize the first cluster of directory entries
	initDirTableCluster(fsBootRecord.rootDir);
	
	// Flush the boot record to the FS file
	flushBootRecord();
}

/**
 * Writes the boot record to the filesystem file
 */
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

/**
 * Initializes a cluster for use as a directory table by creating a cluster's
 * worth of directory table entries.
 * @param	FSPTR	addr	The cluster to initialize as a directory table
 */
void initDirTableCluster(FSPTR addr) {
	// Calculate the correct offset into the filesystem and seek to it
	UINT offset = calcOffset(addr);
	fseek(fsFile, offset, SEEK_SET);

	// Create a blank directory entry
	DirectoryEntry blank;
	blank.fileName[0]   = DIR_ENTRY_AVAILABLE;
	memset(&blank.fileName[1], 0x0, 111);
	blank.index         = 0x0;
	blank.size          = 0x0;
	blank.type          = DIR_ENTRY_FILE;
	blank.creationDate  = 0x0;

	// Now allocate the cluster at the address with these directory entries
	UINT entriesWritten = 0;
	while(entriesWritten * sizeof(DirectoryEntry) 
		< fsBootRecord.clusterSize) {
		fwrite(&blank, sizeof(DirectoryEntry), 1, fsFile);
		++entriesWritten;
	}
}

/**
 * Initializes a cluster for use as the fat table. Starts all clusters off as
 * available with the exception of the fat table, directory table, and boot
 * record clusters.
 * @param	FSPTR	addr	Cluster to initialize as the fat table
 */
void initFAT(FSPTR addr) {
	// Calculate the correct offset into the filesystem and seek to it
	UINT offset = calcOffset(addr);
	fseek(fsFile, offset, SEEK_SET);

	// Create blank FAT entry for every cluster
	FatEntry clusterStatus;
	UINT cluster = 0;
	while(cluster < fsBootRecord.clusterSize / sizeof(FatEntry)) {
		if(cluster == 0 || cluster == fsBootRecord.fatTable) {
			// This cluster is reserved.
			clusterStatus = FAT_RESERVED;
		} else if(cluster == fsBootRecord.rootDir) {
			// This is the end of the dir table FOR NOW
			clusterStatus = FAT_EOC;
		} else {
			// This cluster is unallocated
			clusterStatus = FAT_FREE_CLUSTER;
		}

		// Write this cluster to the FAT
		fwrite(&clusterStatus, sizeof(FatEntry), 1, fsFile);
		++cluster;
	}
}

/**
 * Loads the boot record information into the global boot record struct.
 * @param	char*	name	The name of the filesystem to load
 */
void loadFS(char *name) {
	// Load the file from the disk
	fsFile = fopen(name, "r+");
	
	// Load the MBR into the mbr global
	fseek(fsFile, 0, SEEK_SET);
	fread(&fsBootRecord, sizeof(MBR), 1, fsFile);
}
