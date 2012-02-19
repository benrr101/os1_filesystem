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

#include "fs.h"
#include "fswrite.h"
#include "fsread.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////
FSPTR createFile(char name[112]) {
	// Find the next free directory entry and FAT entry
	UINT freeDir = getFirstFreeDirEntry();
	UINT freeFAT = getFirstFreeFATEntry();

	// @DEBUG
	printf("First Free Dir Entry = 0x%x\n", freeDir);
	printf("First Free FAT Entry = 0x%x\n", freeFAT);
	exit(1);

	// Calculate cluster index from FAT entry
	//FSPTR clusterIndex = calcClusterFromFat(freeFAT);
	
	// Write to the fat to allocate the cluster
	//writeToFAT(freeFAT, FAT_EOC);
	
	// Write to the directory table indicating that there is a file in it!
	// Step 1) seek to the dir table address
	fseek(fsFile, freeDir, SEEK_SET);

	// Step 2) Write the filename
	fwrite(&name, 112, 1, fsFile);

	// Step 3) Write the index of the first cluster
	//fwrite(&clusterIndex, sizeof(FSPTR), 1, fsFile);

	// Step 4) Write the type of the file
	UINT type = DIR_ENTRY_FILE;
	fwrite(&type, sizeof(UINT), 1, fsFile);

	// Step 5) Write the creation data
	UINT seconds = time(NULL);
	printf("CURRENT TIME: %d\n", seconds); // @DEBUG
	fwrite(&type, sizeof(UINT), 1, fsFile);

	// Return the index of the cluster
	return /*clusterIndex*/NULL;
}

void writeToFAT(UINT index, FatEntry value) {
	// Seek to the index of the FAT
	fseek(
		fsFile, 
		index + (fsBootRecord.fatTable * fsBootRecord.clusterSize),
		SEEK_SET);
	
	// Write that value
	fwrite(&value, sizeof(FatEntry), 1, fsFile);
}
