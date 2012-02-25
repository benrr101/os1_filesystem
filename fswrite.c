////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Writing
// 
// @file	fswrite.c
// @descrip	This file implements functions needed for writing data to the
// 		filesystem.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "fs.h"
#include "fswrite.h"
#include "fsread.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////
/**
 * Allocates an additional cluster to add to the chain of the current file.
 * Also writes the update to the FAT.
 * @param	FSPTR	currentCluster	The current end cluster of the file chain
 * @return	FSPTR	The cluster index of the newly allocated cluster
 */
FSPTR allocateCluster(FSPTR currentCluster) {
	// Store the current location in the FS file
	UINT curPos = ftell(fsFile);

	// Find the next available cluster in the FAT table
	UINT curClusterAddr = (fsBootRecord.fatTable * fsBootRecord.clusterSize)
		+ (currentCluster * sizeof(FatEntry));
	UINT fatAddr = getFirstFreeFATEntry();
	
	// Verify that we got a result
	if(fatAddr == 0) {
		return 0;
	}

	// Calculate the cluster that this cooresponds to
	FSPTR cluster = (fatAddr 
		- (fsBootRecord.fatTable * fsBootRecord.clusterSize))
		/ sizeof(FatEntry);
	
	// Write to the current cluster and point to the next cluster
	writeToFAT(curClusterAddr, cluster);
	writeToFAT(fatAddr, FAT_EOC);

	// Return the cluster
	return cluster;
}

/**
 * Creates a file with the given name. Initializes the directory table entry
 * for the file and allocates 1 cluster for the file.
 * @param	char[112]	name	The name of the file
 * @return	FSPTR		The index of the first cluster of the file
 */
FSPTR createFile(char name[112]) {
	// Find the next free directory entry and FAT entry
	UINT freeDir = getFirstFreeDirEntry();
	UINT freeFAT = getFirstFreeFATEntry();

	// Calculate cluster index from FAT entry
	FSPTR clusterIndex = getClusterFromFatAddress(freeFAT);
	
	// Write to the fat to allocate the cluster
	writeToFAT(freeFAT, FAT_EOC);
	
	// Create a directory table entry and write it out to the table
	DirectoryEntry entry;
	memcpy(entry.fileName, name, 112);
	entry.index        = clusterIndex;
	entry.size         = 0;
	entry.type         = DIR_ENTRY_FILE;
	entry.creationDate = time(NULL);
	fseek(fsFile, freeDir, SEEK_SET);
	fwrite(&entry, sizeof(DirectoryEntry), 1, fsFile);

	// Return the index of the cluster
	return clusterIndex;
}

/**
 * Removes the specified file from the filesystem. It also unallocates the
 * chain of clusters.
 * @param	char[112]	name	The name of the file to delete
 */
void removeFile(char name[112]) {
	// Find the file in the directory table
	UINT dirAddr = getDirTableAddressByName(name);
	if(dirAddr == 0) {
		// File doesn't exist, bomb out
		fprintf(stderr, "Error: File does not exist\n");
		return;
	}
	fseek(fsFile, dirAddr, SEEK_SET);
	DirectoryEntry dir;
	fread(&dir, sizeof(DirectoryEntry), 1, fsFile);

	// Store the cluster index for use soon
	FSPTR index = dir.index;
	
	// Change the directory entry to denote the availability and write it
	dir.fileName[0] = DIR_ENTRY_AVAILABLE;
	fseek(fsFile, -1 * sizeof(DirectoryEntry), SEEK_CUR);
	fwrite(&dir, sizeof(DirectoryEntry), 1, fsFile);

	// Goto the FAT entry for the file
	FatEntry entry = index;
	do {
		// Get the address for the fat table entry
		UINT fatAddress = fsBootRecord.fatTable;
		fatAddress *= fsBootRecord.clusterSize;
		fatAddress += entry * sizeof(FatEntry);
	
		// Store the FAT value so we can safely rewrite it	
		entry = lookupFAT(entry);
		
		// Rewrite to show that the cluster is free
		writeToFAT(fatAddress, FAT_FREE_CLUSTER);

	} while(entry != FAT_EOC);
}

/**
 * Writes the specified value to the specified ADDRESS of an entry in the FAT.
 * @param	UINT	index	The address of the FAT table entry to write to
 * @param	FatEntry	value	The value to write to the FAT entry.
 */
void writeToFAT(UINT index, FatEntry value) {
	// Seek to the index of the FAT
	fseek(fsFile, index, SEEK_SET);

	// Temp var
	FatEntry v = value;
	
	// Write that value
	fwrite(&v, sizeof(FatEntry), 1, fsFile);
}
