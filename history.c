////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	history.C
// @descrip	This file defines the structs used for the history of commands
//		as well as funtions for manipulating the history.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "history.h"

// FUNCTIONS ///////////////////////////////////////////////////////////////

/**
 * This function takes in the history list and the last command ran and creates
 * a new history item and adds it to the end of the list.
 * @param  HistoryList*   historyList   The linked list of history items
 * @param  char*          command       The command string last executed
 */  
void historyAdd(HistoryList *historyList, char *command) {
	// Create a new history item for this command
	HistoryItem *newItem = malloc(sizeof(HistoryItem));
	// If malloc failed, we're 'effed
	if(newItem == NULL) {
		fprintf(stderr, "Could not allocate more memory!\n");
		exit(errno);
	}
	
	// Copy the command to the heap (should be null terminated)
	int length = strlen(command) + 1;
	char *commandMem = malloc(length);
	strncpy(commandMem, command, length);
	
	// Set up the new item
	newItem->command = commandMem;
	newItem->nextItem = NULL;

	if(historyList->items == 0) {
		// Case 1: The history list is empty
		// Store new item at the head and foot
		historyList->items = 1;
		historyList->firstItem = newItem;
		historyList->lastItem = newItem;
	} else if (historyList->items == historyList->maxItems) {
		// Case 2: The history list is full
		// Remove the top of the list, insert new item at the foot
		HistoryItem *oldFirst = historyList->firstItem;
		historyList->firstItem = oldFirst->nextItem;
		historyList->lastItem->nextItem = newItem;
		historyList->lastItem = newItem;
		
		// Free the old first element and it's command
		free(oldFirst->command);
		free(oldFirst);
	} else {
		// Case 3: The history list has items
		// Insert the new item at the foot
		historyList->lastItem->nextItem = newItem;
		historyList->lastItem = newItem;
		
		// Increment the number of elements
		historyList->items++;
	}
}

void historyDestroy(HistoryList *historyList) {
	// Iterate through the linked list and delete each command string
	HistoryItem *ptr = historyList->firstItem;
	while(ptr != NULL) {
		// Free the string
		free(ptr->command);
		
		// Go to the next history item
		HistoryItem *oldPtr = ptr;
		ptr = ptr->nextItem;

		// Free the history item
		free(oldPtr);
	}

	// Now free the head of the list
	free(historyList);
}

/**
 * Creates a blank HistoryList struct and returns it. It is initialized with
 * 0 elements and the max number of history items provided
 * @param  int   maxHistory   The maximum number of history elements in the
 *                            linked list
 * @return HistoryList*       A pointer to the newly created history list
 */
HistoryList *historyInit(int maxHistory) {
	// Create a blank history list and initialize it for use
	HistoryList *newList = malloc(sizeof(HistoryList));
	
	// If malloc failed, we're 'effed
	if(newList == NULL) {
		fprintf(stderr, "Could not allocate more memory!\n");
		exit(errno);
	}
	
	// Initialize the new list
	newList->items = 0;
	newList->maxItems = maxHistory;
	newList->firstItem = NULL;
	newList->lastItem = NULL;
}

/**
 * Iterates over the linked list of history items and prints them
 * @param  HistoryList*   historyList   The linked list of history items to
 *                                      print out
 */
void historyPrint(HistoryList *historyList) {
	// If there are no elements in the history list then print that out
	if(historyList->items == 0) {
		printf("There are no commands in the command history\n");
		return;
	}

	// Iterate over the list of history items and print them
	HistoryItem *ptr = historyList->firstItem;
	while(ptr != NULL) {
		printf("%s\n", ptr->command);
		ptr = ptr->nextItem;
	}
}
