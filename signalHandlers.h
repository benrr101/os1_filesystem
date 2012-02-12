////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	history.h
// @descrip	This file declares the handlers for the signals to be handled
//		in this shell application.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef H_OS1_SIGNAL
#define H_OS1_SIGNAL

// FUNCTIONS ///////////////////////////////////////////////////////////////
void terminateHandler(int sig);
void ignoreHandler(int sig);
void handleSIGINT(int sig);
void handleSIGCHLD(int sig);

#endif
