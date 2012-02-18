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
UINT calcOffset(FSPTR addr);
void createFileSystem(char *name);
void flushBootRecord(void);
void initDirTableCluster(FSPTR addr);
void initFAT(FSPTR addr);
void loadFileSystem(char *name);

#endif
