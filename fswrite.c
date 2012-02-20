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
FSPTR createFile(char name[112]) {
	// Find the next free directory entry and FAT entry
	UINT freeDir = getFirstFreeDirEntry();
	UINT freeFAT = getFirstFreeFATEntry();

	// Calculate cluster index from FAT entry
	FSPTR clusterIndex = getClusterFromFatAddress(freeFAT);
	printf("First Free Cluster = %d\n", clusterIndex);
	
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

void removeFile(char name[112]) {
	// Find the file in the directory table
	
}

void writeToFAT(UINT index, FatEntry value) {
	// Seek to the index of the FAT
	fseek(fsFile, index, SEEK_SET);

	// Temp var
	FatEntry v = value;
	
	// Write that value
	fwrite(&v, sizeof(FatEntry), 1, fsFile);
}
