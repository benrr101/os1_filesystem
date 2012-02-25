////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Writing Header
//
// @file	fswrite.h
// @descrip	This declares the functions that enable the project to write to
// 		the filesystem.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef OS1FS_write
#define OS1FS_write

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * Allocates an additional cluster to add to the chain of the current file.
 * Also writes the update to the FAT.
 * @param	FSPTR	currentCluster	The current end cluster of the file chain
 * @return	FSPTR	The cluster index of the newly allocated cluster
 */
FSPTR allocateCluster(FSPTR currentCluster);

/**
 * Creates a file with the given name. Initializes the directory table entry
 * for the file and allocates 1 cluster for the file.
 * @param	char[112]	name	The name of the file
 * @return	FSPTR		The index of the first cluster of the file
 */
FSPTR createFile(char name[112]);

/**
 * Removes the specified file from the filesystem. It also unallocates the
 * chain of clusters.
 * @param	char[112]	name	The name of the file to delete
 */
void removeFile(char name[112]);

/**
 * Writes the specified value to the specified ADDRESS of an entry in the FAT.
 * @param	UINT	index	The address of the FAT table entry to write to
 * @param	FatEntry	value	The value to write to the FAT entry.
 */
void writeToFAT(UINT index, FatEntry value);

#endif
