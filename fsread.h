////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Reading Header
//
// @file	fsread.h
// @descrip	This file declares the functions necessary for reading and
// 		gathering information about the filesystem. This includes
// 		getting indexes into the filesystem.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef OS1FS_read
#define OS1FS_read

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * Calculates the offset into the file that the given file pointer corresponds
 * @param	FSPTR	The cluster to get the address for
 * @return	UINT	The offset into the file that the cluster corresponds to
 */
UINT calcOffset(FSPTR addr);

/**
 * Calculates the cluster index based on the address of the fat entry
 * @param	UINT	fatAddress	The address of the fat entry
 * @return	FSPTR	The cluster index that the address corresponds to
 */
FSPTR getClusterFromFatAddress(UINT fatAddress);

/**
 * Lookups a file by its filename and returns the address of the directory
 * table entry. Seeks is done atomically.
 * @param	char*	name	The name of the file to lookup
 * @return	UINT	The address of the file's directory table entry
 *			0 if the file does not exist
 */
UINT getDirTableAddressByName(char name[112]);

/**
 * Returns the address of the first free directory entry. Creates new directory
 * table clusters if needed.
 * @return	UINT	The address of the first free directory entry
 */
UINT getFirstFreeDirEntry();

/**
 * Returns the address of the first free fat table entry. This cooresponds
 * with the first free cluster in the filesystem.
 * @return	UINT	The address of the first free FAT table entry
 */
UINT getFirstFreeFATEntry();

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
int isPathInFS(char **path, const char *fsPath, int inFS);

/**
 * Seeks (atomically) to the FAT table and looks up the entry at the given
 * cluster index.
 * @param	FSPTR	curCluster	The cluster index to lookup
 * @return	FatEntry	The fat entry at the given index
 */
FatEntry lookupFAT(FSPTR curCluster);

#endif
