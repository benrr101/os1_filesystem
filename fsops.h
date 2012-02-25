////////////////////////////////////////////////////////////////////////////
// OS1 FINAL PROJECT - Filesystem Operations Header
//
// @file	fsops.h
// @descrip	This file declares all the functions for handling complex
// 		operations to be performed on the filesystem. They can handle
// 		their own argument parsing and are basically like standalone
// 		programs. Actually that's not a bad idea, make it a DLL?
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef OS1FS_ops
#define OS1FS_ops

// FUNCTIONS ///////////////////////////////////////////////////////////////

void cat(char *command);
void cp(char *command, int inFS, char *path);
void cpFromFStoRootFS(char *source, char *dest);
void cpFromRootFStoFS(char *source, char *dest);
void cpFromFStoFS(char *source, char *dest);
void ls(char *command);
void touch(char *command);
void rm(char *command);

#endif
