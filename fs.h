////////////////////////////////////////////////////////////////////////////
// OS1 Final Project - Filesystem Header
//
// @file	fs.h
// @descrip	This header declares the structs and types required for this
//		FAT filesystem to be implemented. This includes constants for
//		max cluster/fs size AS WELL AS defining the globals for the MBR
//		temp storage and FAT tables.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef OS1FS_fs
#define OS1FS_fs

// CONSTANTS ///////////////////////////////////////////////////////////////
#define MAX_CLUSTER_SIZE 16384
#define MIN_CLUSTER_SIZE 8192
#define CLUSTER_SIZE_MOD 128

#define MAX_FS_SIZE 50
#define MIN_FS_SIZE 5
#define FS_SIZE_MULTIPLE 1024 * 1024

#define DIR_ENTRY_AVAILABLE 0x00
#define DIR_ENTRY_DELETED 0xFF
#define DIR_ENTRY_FILE 0x0000
#define DIR_ENTRY_FOLDER 0xFFFF

#define FAT_FREE_CLUSTER 0x0000
#define FAT_RESERVED 0xFFFE
#define FAT_EOC 0xFFFF

// GLOBALS /////////////////////////////////////////////////////////////////

// This will be a global pointer to the filesystem file
FILE *fsFile;

// This is where the boot record will be stored while we're working with the FS
struct mbr fsBootRecord;

// TYPEDEFS ////////////////////////////////////////////////////////////////
typedef unsigned int UINT;
typedef unsigned int FSPTR;
typedef struct mbr MBR;
typedef struct dirEntry DirectoryEntry;
typedef int FatEntry;

// STRUCTS /////////////////////////////////////////////////////////////////
/**
 * The MBR struct is the structure that the boot record follows. It is
 * necessary for defining the geometry of the filesystem.
 * I know it's not technically a MASTER boot record, but it sounds better than
 * some regular ol' boot record.
 */
struct mbr {
	UINT 	clusterSize;	// The size of clusters in this filesystem
	UINT	maxSize;	// The maximum size of the filesystem
	FSPTR	rootDir;	// Ptr to the root directory table
	FSPTR	fatTable;	// Ptr to the first file allocation table
};

/**
 * The dirEntry struct provides a predefined layout for the structure of a
 * directory structure. This includes all necessary attributes describing a
 * file/folder and a pointer to the location of the file/folder in the fs.
 */
struct dirEntry {
	char	fileName[111];	// The filename, max of 112 chars. Byte 0
				// denotes if the entry is available or has
				// been deleted
	UINT	index;		// The address of this file's first cluster
	UINT	size;		// The size of the file
	UINT	type;		// Whether the entry is a file or a folder
	UINT	creationDate;	// The creation date in UNIX timestamp
};

#endif
