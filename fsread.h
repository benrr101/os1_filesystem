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

FSPTR getClusterFromFatAddress(UINT fatAddress);
UINT getDirTableAddressByName(char name[112]);
UINT getFirstFreeDirEntry();
UINT getFirstFreeFATEntry();
FatEntry lookupFAT(FSPTR curCluster);

#endif
