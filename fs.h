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

// GLOBALS /////////////////////////////////////////////////////////////////

// This will be a global pointer to the filesystem file
FILE *fsFile;

// This is where the boot record will be stored while we're working with the FS
struct mbr fsBootRecord;

// TYPEDEFS ////////////////////////////////////////////////////////////////
typedef unsigned int UINT;
typedef unsigned int FSPTR;
typedef struct mbr MBR;

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

// FUNCTIONS ///////////////////////////////////////////////////////////////
void createFileSystem(char *name);
void loadFileSystem(char *name);

#endif
