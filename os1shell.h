////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	os1shell.h
// @descrip	This file defines several constants for use across the
//		application including the text for the prompt, the maximum
//		size of a command and the maximum number of history items to
//		store.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef H_OS1_MAIN
#define H_OS1_MAIN

// CONSTANTS ///////////////////////////////////////////////////////////////
#define BUFFER_SIZE 65
#define MAX_HISTORY 20

#define ROOT_DIR "/"

#define CAT_COMMAND "cat"
#define CD_COMMAND "cd"
#define CP_COMMAND "cp"
#define DF_COMMAND "df"
#define HISTORY_COMMAND "history"
#define LS_COMMAND "ls"
#define MV_COMMAND "mv"
#define PWD_COMMAND "pwd"
#define RM_COMMAND "rm"
#define TOUCH_COMMAND "touch"
#define PROMPT "OS1Shell -> "

#define true 8
#define false 0

#endif
