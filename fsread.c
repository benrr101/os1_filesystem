////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Reading
//
// @file	fsread.c
// @descrip	This implements the functions necessary for reading and
// 		gathering information about the filesystem. This includes
// 		getting indexes into the filesystem.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "fsread.h"
#include "os1shell.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////
/**
 * Calculates the offset into the file that the given file pointer corresponds
 * @param	FSPTR	The cluster to get the address for
 * @return	UINT	The offset into the file that the cluster corresponds to
 */
UINT calcOffset(FSPTR addr) {
	// Offset = address * clusterSize
	return addr * fsBootRecord.clusterSize;
}

/**
 * Calculates the cluster index based on the address of the fat entry
 * @param	UINT	fatAddress	The address of the fat entry
 * @return	FSPTR	The cluster index that the address corresponds to
 */
FSPTR getClusterFromFatAddress(UINT fatAddress) {
	// First subtract off the address of the fat table
	fatAddress -= fsBootRecord.fatTable * fsBootRecord.clusterSize;

	// Next, divide by the size of the fat entry to get the final address
	fatAddress /= sizeof(FatEntry);

	// return it
	return fatAddress;
}

/**
 * Lookups a file by its filename and returns the address of the directory
 * table entry. Seeks is done atomically.
 * @param	char*	name	The name of the file to lookup
 * @return	UINT	The address of the file's directory table entry
 *			0 if the file does not exist
 */
UINT getDirTableAddressByName(char name[112]) {
	// Get the current address for restoration purposes
	UINT curPos = ftell(fsFile);

	// Seek to the location of the directory table
	UINT dirTable    = calcOffset(fsBootRecord.rootDir);
	UINT dirCluster  = fsBootRecord.rootDir;
	UINT dirTableEnd = dirTable + fsBootRecord.clusterSize;
	UINT currentAddr = dirTable;
	fseek(fsFile, dirTable, SEEK_SET);

	// Loop until we find the entry
	while(dirCluster != FAT_EOC) {
		// Load the info about the directory cluster
		UINT dirTable    = calcOffset(dirCluster);
		UINT dirTableEnd = dirTable + fsBootRecord.clusterSize;
		UINT currentAddr = dirTable;
		fseek(fsFile, dirTable, SEEK_SET);

		// While we're not at the end of the cluster
		while(currentAddr < dirTableEnd) {
			// Read in the entry
			DirectoryEntry entry;
			fread(&entry, sizeof(DirectoryEntry), 1, fsFile);
			
			// Does it match?
			if(strcmp(name, entry.fileName) == 0) {
				// Yep, return the address
				fseek(fsFile, curPos, SEEK_SET);
				return currentAddr;
			}

			currentAddr += sizeof(DirectoryEntry);
		}

		// We're at the end of a directory cluster!
		// FIND THE NEXT ONE
		dirCluster = lookupFAT(dirCluster);
	}
	
	// We didn't find the file...
	fseek(fsFile, curPos, SEEK_SET);

	return 0;
}

/**
 * Returns the address of the first free directory entry. Creates new directory
 * table clusters if needed.
 * @return	UINT	The address of the first free directory entry
 */
UINT getFirstFreeDirEntry() {
	// Seek to the location of the directory table
	FSPTR dirCluster = fsBootRecord.rootDir;
	FSPTR lastCluster = fsBootRecord.rootDir;

	// Iterate until we get to the end of the directory table
	while(dirCluster != FAT_EOC) {
		// Get the info about the cluster
		UINT dirTable    = calcOffset(dirCluster);
		UINT currAddr    = dirTable;
		UINT dirTableEnd = dirTable + fsBootRecord.clusterSize;
		fseek(fsFile, currAddr, SEEK_SET);

		// Loop until we've read all the directory entries
		while(currAddr < dirTableEnd) {
			// Read in the directory entry
			DirectoryEntry entry;
			fread(&entry, sizeof(DirectoryEntry), 1, fsFile);
			
			if(entry.fileName[0] == DIR_ENTRY_AVAILABLE) {
				// This entry is free, so return it
				return currAddr;
			}

			// Advance the counter!
			currAddr += sizeof(DirectoryEntry);
		}

		// Load up the next dirCluster
		lastCluster = dirCluster;
		dirCluster = lookupFAT(dirCluster);
	}

	// If we made it this far, we need another directory table
	UINT fatEntryAddr = getFirstFreeFATEntry();
	FSPTR newCluster = (fatEntryAddr 
			- (fsBootRecord.fatTable * fsBootRecord.clusterSize))
			/ sizeof(FatEntry);
	UINT dirTableAddr = newCluster * fsBootRecord.clusterSize;
	initDirTableCluster(newCluster);
	writeToFAT(
		(lastCluster * sizeof(FatEntry)) 
		+ (fsBootRecord.fatTable * fsBootRecord.clusterSize)
		, newCluster);
	writeToFAT(fatEntryAddr, FAT_EOC);
	return dirTableAddr;
}

/**
 * Returns the address of the first free fat table entry. This cooresponds
 * with the first free cluster in the filesystem.
 * @return	UINT	The address of the first free FAT table entry
 */
UINT getFirstFreeFATEntry() {
	// Jump to the FAT table
	UINT currentAddress = calcOffset(fsBootRecord.fatTable);
	UINT endOfTable     = currentAddress + fsBootRecord.clusterSize;
	fseek(fsFile, currentAddress, SEEK_SET);

	// Loop until we find a free FAT entry
	UINT result = 0;
	do {
		// Load the FatEntry from the table
		FatEntry e;
		fread(&e, sizeof(FatEntry), 1, fsFile);

		// Is the cluster free?
		if(e == FAT_FREE_CLUSTER) {
			// Yep, we're good to go
			result = currentAddress;
		} else {
			// Nope, advance to the next cluster
			currentAddress += sizeof(FatEntry);
		}
	} while(result == 0);

	return result;
}

/**
 * Determines if the provided path is in the filesystem or the root filesystem.
 * If the path starts with /filesystem/, the path paramemter will be modified
 * such that the /filesystem/ is removed.
 * @param	char**	path	Pointer to the char* of the path to determine
 * @param	char*	fsPath	Constant string of the path of the filesystem
 *				like: /filesystem
 * @param	bool	inFS	Whether or not the shell is in the filesystem
 * @return	bool	True if the path points to a file in the filesystem
 * 			False if the path is not in the filesystem
 */
int isPathInFS(char **path, const char *fsPath, int inFS) {
	// Case 1: path starts with /fsPath
	if(strstr(*path, fsPath) == *path) {
		// Yep we're in the filesystem, trim off the /fspath
		strtok(*path, "/");
		*path = strtok(NULL, "/");
		
		return true;
	}

	// Case 2: path starts with /
	if(*(path)[0] == '/') {
		// Nope, we're not in the filesystem
		return false;
	}

	// Case 3: plain filename
	return inFS;
}		

/**
 * Seeks (atomically) to the FAT table and looks up the entry at the given
 * cluster index.
 * @param	FSPTR	curCluster	The cluster index to lookup
 * @return	FatEntry	The fat entry at the given index
 */
FatEntry lookupFAT(FSPTR curCluster) {
	// Get the current position in the file
	UINT curPos = ftell(fsFile);
	
	// Jump to the FAT plus the cluster we're looking at
	UINT fatAddr = fsBootRecord.fatTable * fsBootRecord.clusterSize;
	fatAddr += (curCluster * sizeof(FatEntry));
	fseek(fsFile, fatAddr, SEEK_SET);

	// Read from the FAT at the current address
	FatEntry result;
	fread(&result, sizeof(FatEntry), 1, fsFile);
	
	// Set the position to where we started
	fseek(fsFile, curPos, SEEK_SET);

	return result;
}
