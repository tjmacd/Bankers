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
// max amount of each resource, used to calculate random values
int max[NUM_RESOURCES];
pthread_mutex_t lock;
pthread_mutex_t console;

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

// banker's algorithm variables
int work[NUM_RESOURCES];
int finish[NUM_CUSTOMERS];

void print_resources () {
    pthread_mutex_lock(&console);
    printf("Available: ");

    for (int i = 0; i < NUM_RESOURCES; i++)
        printf("%d ", available[i]);

    printf("\n");
    pthread_mutex_unlock(&console);
}

void print_need (int customerNumber) {
    pthread_mutex_lock(&console);
    printf("Need for %d: ", customerNumber);

    for (int i = 0; i < NUM_RESOURCES; i++)
        printf("%d ", need[customerNumber][i]);

    printf("\n");
    pthread_mutex_unlock(&console);
}

void print_allocation (int customerNumber, int request[], bool isSafe) {
    pthread_mutex_lock(&console);
    printf("Customer %d requesting to allocate: ", customerNumber);
    for (int i = 0; i < NUM_RESOURCES; i++)
        printf("%d ", request[i]);
    if (!isSafe)
        printf("\nState unsafe: Request denied\n");
    else
        printf("\nState safe: Request granted\n");

    printf("\n");
    pthread_mutex_unlock(&console);
}

void print_deallocation (int customerNumber, int amount[]) {
    pthread_mutex_lock(&console);
    printf("Customer %d de-allocating: ", customerNumber);
    for (int i = 0; i < NUM_RESOURCES; i++)
        printf("%d ", amount[i]);

    printf("\n\n");
    pthread_mutex_unlock(&console);
}

void* requestReleaseRepeat (void* arg) {
    // local variables
    int customerNumber = (int) arg;
    bool resourcesAllocated = false;
    srand((unsigned)time(NULL));

    // generate the max amount of resources this thread requires in order to run
    for (int i = 0; i < NUM_RESOURCES; i++) {
        maximum[customerNumber][i] = rand() % (max[i] + 1);
        need[customerNumber][i] = maximum[customerNumber][i];
    }

    // continuously loop until the thread has allocated all the resources it needs
    while (!resourcesAllocated) {
        int request[NUM_RESOURCES]; // hold random amount of resources to allocate

        // generate a random amount of resources(bounded by need) to allocate to the thread   
        for (int i = 0; i < NUM_RESOURCES; i++) {
            if (need[customerNumber][i] == 0)
                request[i] = 0;
            else {
                request[i] = rand() % (need[customerNumber][i] + 1);
            }
        }

        while (true) {
            pthread_mutex_lock(&lock);
            if (request_Res(customerNumber, request)) {
                pthread_mutex_unlock(&lock);
                resourcesAllocated = true;
                // check if all resources have been allocated
                for (int i = 0; i < NUM_RESOURCES; i++) {
                    if (need[customerNumber][i] != 0)
                        resourcesAllocated = false;
                }
                break;
            } else {
                pthread_mutex_unlock(&lock);
            }
        }
    }
    // free all resources since the thread has allocated all it needs
    pthread_mutex_lock(&lock);
    release_Res(customerNumber, maximum[customerNumber]);
    pthread_mutex_unlock(&lock);

    return NULL; // exit thread
}

// Define functions declared in banker.h here
bool request_Res (int nCustomer, int request[NUM_RESOURCES]) {
    // check if there are enough resources to even allocate
    for (int j = 0; j < NUM_RESOURCES; j++) {
		if(request[j] > need[nCustomer][j] || request[j] > available[j])
			return false;
	}

    // make change to resources to check if the resulting state will be safe
	for (int j = 0; j < NUM_RESOURCES; j++) {
		available[j] -= request[j];
		allocation[nCustomer][j] += request[j];
        need[nCustomer][j] = maximum[nCustomer][j] - allocation[nCustomer][j];
	}

    if (is_safe()) {
        print_allocation(nCustomer, request, true);
        return true;
    }
    else { // resulting state unsafe, revert changes
        print_allocation(nCustomer, request, false);
        release_Res(nCustomer, request);
        return false;
    }
}

bool release_Res (int nCustomer, int amount[]) {
    for (int j = 0; j < NUM_RESOURCES; j++) {
        available[j] += amount[j];
        allocation[nCustomer][j] -= amount[j];
        need[nCustomer][j] = maximum[nCustomer][j] - allocation[nCustomer][j];
    }
    print_deallocation(nCustomer, amount);
    return true;
}

bool lessthan_equalto (int array1[], int array2[], int length) {
    for (int i = 0; i < length; i++) {
        if (array1[i] > array2[i])
            return false;
    }
    return true;
}

// Check if state is safe
bool is_safe () {
    // local variables
    int work[NUM_RESOURCES];
    bool finish[NUM_CUSTOMERS] = {false};
    bool found;

    // Step 1:
    for (int i = 0; i < NUM_RESOURCES; i++)
        work[i] = available[i];

    // Step 2:
    do {
        found = false;
        for (int i = 0; i < NUM_CUSTOMERS; i++) {
            if (finish[i] == false) {
                if (lessthan_equalto(need[i], work, NUM_RESOURCES)) {
                    found = true;
                    // Step 3:
                    for (int j = 0; j < NUM_RESOURCES; j++)
                        work[j] += allocation[i][j];

                    finish[i] = true;
                }
            }
        }
    } while(found);

    // Step 4:
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        if (finish[i] == false)
            return false;
    }
    return true;
}


int main (int argc, char *argv[]) {
    // define local variables
    pthread_t customer[NUM_CUSTOMERS];

    if (argc != NUM_RESOURCES+1) {
        printf("Error: Incorrect amount of arguments supplied. Requires: %d Received: %d\n", NUM_RESOURCES, argc-1);
        exit(0);
    }

    // initialise available and max resources
    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = atoi(argv[i+1]);
        max[i] = atoi(argv[i+1]);
    }

    // initialize other arrays
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            allocation[i][j] = 0;
            need[i][j] = 0;
        }
    }
    // initialise the mutex locks
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&console, NULL);
    
    // create customer threads and run them
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_create(&customer[i], 0, requestReleaseRepeat, (void*) i);

    // join the customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customer[i], NULL);

    // destroy the mutex locks
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&console);

    return EXIT_SUCCESS;
}
