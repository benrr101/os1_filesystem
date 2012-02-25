////////////////////////////////////////////////////////////////////////////
// OS1 Final Project - Filesystem Initialization Header
//
// @file	fsinit.h
// @descrip	This header declares the functions necessary for initialization
//		of the filesystem. This includes creating/loading the
//		filesystem as well as initailization of directory clusters that
//		can happen after the filesystem is init'd.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef OS1FS_init
#define OS1FS_init

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * Writes null bytes to the maximum size of the filesystem.
 * @param	UINT	size	Mamimum size of the filesystem in bytes
 */
void allocateFS(UINT size);

/**
 * Create the filesystem. Promps the user for information about the filesystem.
 * Creates the boot record, initializes the fat table, allocates the full size
 * of the fs, and initializes 1 cluster of directory table.
 * @param	char*	name	Name of the filesystem
 */
void createFileSystem(char *name);

/**
 * Writes the boot record to the filesystem file
 */
void flushBootRecord(void);

/**
 * Initializes a cluster for use as a directory table by creating a cluster's
 * worth of directory table entries.
 * @param	FSPTR	addr	The cluster to initialize as a directory table
 */
void initDirTableCluster(FSPTR addr);

/**
 * Initializes a cluster for use as the fat table. Starts all clusters off as
 * available with the exception of the fat table, directory table, and boot
 * record clusters.
 * @param	FSPTR	addr	Cluster to initialize as the fat table
 */
void initFAT(FSPTR addr);

/**
 * Loads the boot record information into the global boot record struct.
 * @param	char*	name	The name of the filesystem to load
 */
void loadFileSystem(char *name);

#endif
