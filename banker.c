/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Adham, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "banker.h"

// Put any other macros or constants here using #define
// May be any values >= 0
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

// Put global environment variables here
// Available amount of each resource
int available[NUM_RESOURCES];

int max[NUM_RESOURCES];

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

void* requestReleaseRepeat (void* arg) {
    int customerNumber = (int) arg;

    for (int i = 0; i < 3; i++) {
        int randAmount[NUM_RESOURCES];

        for (int i = 0; i < NUM_RESOURCES; i++) {
            randAmount[i] = rand() % max[i];
            printf("%d\n", randAmount[i]);
        }

        //requestRes(customerNumber, randAmount);
        //releaseRes(customerNumber, randAmount);
    }
    return NULL;
}

// Define functions declared in banker.h here
bool request_res(int n_customer, int request[])
{
    for(int j = 0; j < NUM_RESOURCES; j++){
		if(request[j] > need[n_customer][j])
			return false;
	}
	for(int j = 0; j < NUM_RESOURCES; j++){
		if(request[j] > available[j])
			return false;
	}
	for(int j = 0; j < NUM_RESOURCES; j++){
		available[j] -= request[j];
		allocation[n_customer][j] += request[j];
		need[n_customer][j] -= request[j];
	}
	return true;
}

bool release_res (int n_customer, int amount[]) {
    return false;
}


int main(int argc, char *argv[]) {
    // define local variables
    pthread_t customerZero, customerOne, customerTwo, customerThree, customerFour;

    if (argc != NUM_RESOURCES+1) {
        printf("Error: Incorrect amount of arguments supplied. Requires: %d Received: %d\n", NUM_RESOURCES, argc-1);
        exit(0);
    }

    // initialize available resources
    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = atoi(argv[i+1]);
        max[i] = atoi(argv[i+1]);
    }

    pthread_create(&customerZero, 0, requestReleaseRepeat, (void*) 0);
    pthread_create(&customerOne, 0, requestReleaseRepeat, (void*) 1);
    pthread_create(&customerTwo, 0, requestReleaseRepeat, (void*) 2);
    pthread_create(&customerThree, 0, requestReleaseRepeat, (void*) 3);
    pthread_create(&customerFour, 0, requestReleaseRepeat, (void*) 4);

    // ==================== YOUR CODE HERE ==================== //

    // Initialize the pthreads, locks, mutexes, etc.

    // Run the threads and continually loop

    // The threads will request and then release random numbers of resources

    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly
    
    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

    pthread_join(customerZero, NULL);
    pthread_join(customerOne, NULL);
    pthread_join(customerTwo, NULL);
    pthread_join(customerThree, NULL);
    pthread_join(customerFour, NULL);

    return EXIT_SUCCESS;
}
