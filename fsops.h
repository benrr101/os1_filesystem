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

/**
 * Outputs all the characters of the requested file
 * @param	char* command	the command from the command line
 */
void cat(char *command);

/**
 * Copies a file from anywhere to anywhere
 * @param	char* 	command	the command from the command line
 * @param	bool	inFS	Whether or not the shell is in the fs
 * @param	char*	fsPath	The path to the filesystem like /filesystem
 */
void cp(char *command, int inFS, char *path);

/**
 * Copies a file from the virtual fs to the root fs
 * @param	char*	source	The source file location
 * @param	char*	dest	The destination file location
 */
void cpFromFStoRootFS(char *source, char *dest);

/**
 * Copies a file from the root fs to the virtual fs
 * @param	char*	source	The source file location
 * @param	char*	dest	The destination file location
 */
void cpFromRootFStoFS(char *source, char *dest);

/**
 * Copies a file from the virtualFS to the virtualFS
 * @param	char*	source	The source file location
 * @param	char*	dest	The destination file location
 */
void cpFromFStoFS(char *source, char *dest);

/**
 * Prints out the information about the filesystem.
 */
void df();

/**
 * Outputs all the files in the directory
 * @param	char* command	the command from the command line
 */
void ls(char *command);

/**
 * Moves a file from anywhere to anywhere using a combo of cp's and rm's
 * @param	char* 	command	the command from the command line
 * @param	bool	inFS	Whether or not the shell is in the fs
 * @param	char*	fsPath	The path to the filesystem like /filesystem
 */
void mv(char *command, int inFS, char *path); 

/**
 * Creates an empty file(s)
 * @param	char* command	the command from the command line
 */
void touch(char *command);

/**
 * Removes the requested file(s)
 * @param	char* command	the command from the command line
 */
void rm(char *command);

#endif
