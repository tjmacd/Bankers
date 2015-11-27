/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Adham, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#ifndef BANKER_H_
#define BANKER_H_

#include <stdbool.h>

// print resources available
extern void print_resources ();

// print resources needed by a thread
extern void print_need (int customerNumber);

// print the result of a call of the allocation function
extern void print_allocation (int customerNumber, int request[], bool safe);

// print the result of a call of the de-allocation function
extern void print_deallocation (int customerNumber, int request[]);

// Request resources, returns true if successful
extern bool request_Res (int nCustomer, int request[]);

// Release resources, returns true if successful
extern bool release_Res (int nCustomer, int release[]);

// checks if the system is in a safe state
extern bool is_safe ();
// helper function for is_safe()
extern bool lessthan_equalto (int array1[], int array2[], int length);

#endif /* BANKER_H_ */