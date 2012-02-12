////////////////////////////////////////////////////////////////////////////
// OS1 Project 1 - os1shell
//
// @file	history.h
// @descrip	This file declares the structs used for the history of commands
//		as well as funtions for manipulating the history.
// @author	Benjamin Russell (brr1922)
////////////////////////////////////////////////////////////////////////////

#ifndef H_OS1_HIST
#define H_OS1_HIST

// TYPEDEFS ////////////////////////////////////////////////////////////////
typedef struct history_list HistoryList;
typedef struct history_item HistoryItem;

// GLOBALS /////////////////////////////////////////////////////////////////
HistoryList *historyList;

// STRUCTS /////////////////////////////////////////////////////////////////
/**
 * The history_list is the header for the list of history items. It stores the
 * number of items, the maximum number of items and a pointer to the head of
 * the linked list of history items
 */
struct history_list {
	int         items;	// The current number of items in the list
	int         maxItems;	// The maximum number of items in the list
	HistoryItem *firstItem; // Pointer to the first element in the linked
				// list of history items
	HistoryItem *lastItem;	// Pointer to the last element in the linked
				// list of history items. Useful for adding a
				// final item to the list
};

/**
 * The history_item struct is the meat and potatoes of the history linked list.
 * Each element contains a command string as well as a pointer to the next
 * element in the linked list.
 */
struct history_item {
	char        *command;	// The command that was executed
	HistoryItem *nextItem;	// The next command in the linked list
};

// FUNCTIONS ///////////////////////////////////////////////////////////////
void historyAdd(HistoryList *historyList, char *command);
void historyDestroy(HistoryList *historyList);
HistoryList *historyInit(int maxHistory);
void historyPrint(HistoryList *historyList);

#endif
