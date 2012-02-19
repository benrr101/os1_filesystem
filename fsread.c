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

#include "fs.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

UINT getFirstFreeDirEntry() {
	// Seek to the location of the directory table
	UINT dirTable    = calcOffset(fsBootRecord.rootDir);
	UINT dirCluster  = fsBootRecord.rootDir;
	UINT dirTableEnd = dirTable + fsBootRecord.clusterSize;
	UINT currentAddr = dirTable;
	fseek(fsFile, dirTable, SEEK_SET);

	// Loop until we find a available entry
	UINT entryAddress = 0;
	do {
		// Read in the directory table entry at this address
		DirectoryEntry entry;
		fread(&entry, sizeof(DirectoryEntry), 1, fsFile);

		// Is the current directory table entry 
		if(entry.fileName[0] == DIR_ENTRY_AVAILABLE) {
			// This entry is free, so return it
			entryAddress = currentAddr;
		} else {
			// Entry isn't free.
			// Are we at the end of the directory table?
			if(currentAddr+sizeof(DirectoryEntry) >= dirTableEnd){
				// Is there another table we can jump to?
				if(lookupFAT(dirCluster) == FAT_EOC) {
					// NOPE. Initialize a new directory table clust
					// @TODO: ^^^
					printf("FUCKSTICKS.\n");
					exit(1);
				} else {
					// Yep, jump to that location
					dirCluster = lookupFAT(dirCluster);
					dirTable = calcOffset(dirCluster);
					dirTableEnd = dirTable + fsBootRecord.clusterSize;
					currentAddr = dirTable;
					fseek(fsFile, dirTable, SEEK_SET);
				}
			} else {
				// Nope, advance the current address and loop
				currentAddr += sizeof(DirectoryEntry);
			}
		}

	} while(entryAddress == 0);

	return entryAddress;
}

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

FSPTR getClusterFromFatAddress(UINT fatAddress) {
	// First subtract off the address of the fat table
	fatAddress -= fsBootRecord.fatTable * fsBootRecord.clusterSize;

	// Next, divide by the size of the fat entry to get the final address
	fatAddress /= sizeof(FatEntry);

	// return it
	return fatAddress;
}

FatEntry lookupFAT(FSPTR curCluster) {
	// Get the current position in the file
	UINT curPos = ftell(fsFile);
	
	// Jump to the FAT plus the cluster we're looking at
	UINT fatAddr = fsBootRecord.fatTable;
	fseek(fsFile, fatAddr + (curCluster * sizeof(FatEntry)), SEEK_SET);

	// Read from the FAT at the current address
	FatEntry result;
	fread(&result, sizeof(FatEntry), 1, fsFile);
	
	// Set the position to where we started
	fseek(fsFile, curPos, SEEK_SET);

	return result;
}