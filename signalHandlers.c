////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	signalHandlers.c
// @descrip	This file defines the handlers for the signals to be handled
//		in this shell application. It relies on global variables to be
//		defined in order to properly function.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "history.h"
#include "os1shell.h"
#include "signalHandlers.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * This function will register all the signal handlers for almost all of the
 * signals in the POSIX and non-POSIX signal list. The ones that aren't being
 * handled have good reasons.
 */
void registerHandlers(void) {
	// Register all signals to something meaningful
	
	// SIGABRT - It should terminate
	signal(SIGABRT, terminateHandler);
	// SIGALRM - It should terminate
	signal(SIGALRM, terminateHandler);
	// SIGBUS - Memory got effed, going to terminate
	signal(SIGBUS, terminateHandler);
	// SIGCHLD - To be determined at execution time
	// SIGCONT - We're going to print that
	signal(SIGCONT, ignoreHandler);
	// SIGFPE - We're not using floating points here, so print it
	signal(SIGFPE, ignoreHandler);
	// SIGHUP - If a hangup happens, terminate
	signal(SIGHUP, terminateHandler);
	// SIGILL - Illegal operations should probably terminate
	signal(SIGILL, terminateHandler);
	// SIGINT - Ctrl-C will show the history
	signal(SIGINT, handleSIGINT);
	// SIGIO - Not sure about this, so print
	signal(SIGIO, ignoreHandler);
	// SIGKILL - This is the killswitch to beat all killswitches, therefore
	// imma just let the system handle this one
	// SIGPIPE - Broken pipe! We'll terminate.
	signal(SIGPIPE, terminateHandler);
	// SIGPROF - Not sure about this, default is to terminate
	signal(SIGPROF, terminateHandler);
	// SIGPWR - Power is failing! Terminate!
	signal(SIGPWR, terminateHandler);
	// SIGQUIT - Ctrl-\ we want to terminate on this
	signal(SIGQUIT, terminateHandler);
	// SIGSEGV - Segfault, Imma going to terminate
	signal(SIGSEGV, terminateHandler);
	// SIGSTOP - I trust the system with handling this one
	// SIGSYS - Invalid system call, I'll just print that
	signal(SIGSYS, ignoreHandler);
	// SIGTERM - Terminate, of course!
	signal(SIGTERM, terminateHandler);
	// SIGTRAP - Terminate
	signal(SIGTRAP, terminateHandler);
	// SIGTSTP - Print it
	signal(SIGTSTP, ignoreHandler);
	// SIGTTIN - Print it
	signal(SIGTTIN, ignoreHandler);
	// SIGTTOU - Print it
	signal(SIGTTOU, ignoreHandler);
	// SIGURG - Print it
	signal(SIGURG, ignoreHandler);
	// SIGUSR1/2 - Print it
	signal(SIGUSR1, ignoreHandler);
	signal(SIGUSR2, ignoreHandler);
	// SIGWINCH - Terminal size changed, so print it
	signal(SIGWINCH, ignoreHandler);
	// SIGXCPU - Terminate hard
	signal(SIGXCPU, terminateHandler);
	// SIGXFSZ - Terminate for file size too big!
	signal(SIGXFSZ, terminateHandler);
}

/**
 * This handler will print the history list when the ctrl-c is pressed (ie
 * when the SIGINT signal is received). Once the history is printed, a prompt
 * is outputted.
 * @param  int   sig   The signal that was raised
 */
void handleSIGINT(int sig) {
	printf("\n");
	historyPrint(historyList);
	printf(PROMPT);
	fflush(stdout);
}

/**
 * This handler will output a notification that a specific child has exited.
 * The PID of the child will be printed.
 * @param  int   sig   The signal that was raised
 */
void handleSIGCHLD(int sig) {
	// Grab which PID exited
	int pid = wait(NULL);

	// Output that the child exited
	printf("\nBackground process finished: %d\n", pid);

	// Reregister a do nothing handler
	signal(SIGCHLD, ignoreHandler);
}	

/**
 * This handler will print out the caught signal and then continue
 * @param  int   sig   The signal that is being handled
 */
void ignoreHandler(int sig) {
	printf("\nCaught signal: %d\n", sig);
}

/**
 * This handler will print out the caught signal and then terminate with the
 * given signal as the exit code.
 * @param  int   sig   The signal that is being handled
 */
void terminateHandler(int sig) {
	printf("\nCaught signal: %d\n TERMINATING.\n", sig);
	exit(sig);
}
