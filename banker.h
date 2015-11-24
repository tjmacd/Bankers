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

// Add any additional data structures or functions you want here
// perhaps make a clean bank structure instead of several arrays...

// Request resources, returns true if successful
extern bool request_Res(int nCustomer, int request[]);

// Release resources, returns true if successful
extern bool release_Res(int nCustomer, int release[]);

// checks if the system is in a safe state
extern bool isSafe ();

#endif /* BANKER_H_ */