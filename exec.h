////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - OS1Shell
//
// @file	exec.h
// @descrip	This file declares the functions that are used for creating
// 		child processes and executing the requested command. It also
// 		includes helper functions for acheiving this goal.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef H_OS1_EXEC
#define H_OS1_EXEC

// FUNCTIONS ///////////////////////////////////////////////////////////////
char **createArgumentList(char * command, int *background);
void execCommand(char *command);

#endif
