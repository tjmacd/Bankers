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

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

// banker's algorithm variables
int work[NUM_RESOURCES];
int finish[NUM_CUSTOMERS];

void* requestReleaseRepeat (void* arg) {
    int customerNumber = (int) arg;

    for (int run = 0; run < 3; run++) {
        int randAmount[NUM_RESOURCES];

        pthread_mutex_lock(&lock);
        //printf("\nCustomer: %d\nNeed: ", customerNumber);
        
        for (int i = 0; i < NUM_RESOURCES; i++) {
            randAmount[i] = rand() % max[i]; // generate random amount of resources to allocate
            if (need[customerNumber][i]+randAmount[i] > max[i]) {
                randAmount[i] = max[i] - need[customerNumber][i];
                need[customerNumber][i] = max[i];
            } else {
                need[customerNumber][i] += randAmount[i]; // add to need array
            }

            //printf("%d ", need[customerNumber][i]);
        }
        //printf("\n");
        pthread_mutex_unlock(&lock);

        while (true) {
            pthread_mutex_lock(&lock);
            if (request_Res(customerNumber, randAmount)) {
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        printf("Freeing: ");
        /*for (int i = 0; i < NUM_RESOURCES; i++) {
            randAmount[i] = rand() % allocation[customerNumber][i]; // generate random amount of resources to release
            printf("%d ", randAmount[i]);
        }*/
        printf("\n");
        pthread_mutex_unlock(&lock);

        while (true) {
            pthread_mutex_lock(&lock);
            if (release_Res(customerNumber, randAmount)) {
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

// Define functions declared in banker.h here
bool request_Res(int nCustomer, int request[]) {
    // check if there are enough resources to even allocate
    for(int j = 0; j < NUM_RESOURCES; j++) {
		if(request[j] > need[nCustomer][j]) {
            printf("Exceeded need customer %d\n", nCustomer);
			return false;
        }
        if(request[j] > available[j]) {
            printf("Exceeded available customer %d\nAvailable: ", nCustomer);
            for (int i = 0; i < NUM_RESOURCES; i++)
                printf("%d ", available[i]);
            printf("\n");
            printf("Requested: ");
            for (int i = 0; i < NUM_RESOURCES; i++)
                printf("%d ", request[i]);
            printf("\n");
            return false;
        }
	}

    printf("\nAvailable: ");
    for(int j = 0; j < NUM_RESOURCES; j++)
        printf("%d ", available[j]);
    printf("\nRequested: ");
    for (int i = 0; i < NUM_RESOURCES; i++)
        printf("%d ", request[i]);
    printf("\nAllocation[%d]: ", nCustomer);
    for(int j = 0; j < NUM_RESOURCES; j++)
        printf("%d ", allocation[nCustomer][j]);
    printf("\nneed[%d]:", nCustomer);
    for(int j = 0; j < NUM_RESOURCES; j++)
        printf("%d ", need[nCustomer][j]);
    printf("\n");

    // make change to resources to check if the resulting state will be safe
	for(int j = 0; j < NUM_RESOURCES; j++) {
		available[j] -= request[j];
		allocation[nCustomer][j] += request[j];
		need[nCustomer][j] -= request[j];
	}

    if (isSafe()) {
        return true;
    }
    else {
        // resulting state unsafe, revert changes
        for (int j = 0; j < NUM_RESOURCES; j++) {
            available[j] += request[j];
            allocation[nCustomer][j] -= request[j];
            need[nCustomer][j] += request[j];
        }
        return false;
    }
}

bool release_Res (int nCustomer, int amount[]) {
    for (int j = 0; j < NUM_RESOURCES; j++) {
            available[j] += amount[j];
            allocation[nCustomer][j] -= amount[j];
            need[nCustomer][j] += amount[j];
    }
    return true;
}

bool isSafe () {
    bool violation = false;

    // Step 1:
    // initialise work array
    for (int i = 0; i < NUM_RESOURCES; i++)
        work[i] = available[i];
    // initialise finish array
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        finish[i] = false;

    // Step 2:
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        if (finish[i] == false) {
            for (int j = 0; j < NUM_RESOURCES; j++) {
                if (need[i][j] > work[i]) {
                    // violation, i is not the value we are looking for
                    violation = true;
                    break;
                }
            }
            if (!violation) {
                // Step 3:
                for (int j = 0; j < NUM_RESOURCES; j++)
                    work[j] = work[j] + allocation[i][j];
                finish[i] = true;
                i = 0;
            }
            continue;
        }
    }
    // Step 4:
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        if (!finish[i])
            return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    // define local variables
    pthread_t customerZero, customerOne, customerTwo, customerThree, customerFour;

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
    // create customer threadss and run them
    pthread_mutex_init(&lock, NULL);
    pthread_create(&customerZero, 0, requestReleaseRepeat, (void*) 0);
    //pthread_create(&customerOne, 0, requestReleaseRepeat, (void*) 1);
    //pthread_create(&customerTwo, 0, requestReleaseRepeat, (void*) 2);
    //pthread_create(&customerThree, 0, requestReleaseRepeat, (void*) 3);
    //pthread_create(&customerFour, 0, requestReleaseRepeat, (void*) 4);

    // ==================== YOUR CODE HERE ==================== //

    // Initialize the pthreads, locks, mutexes, etc.

    // Run the threads and continually loop

    // The threads will request and then release random numbers of resources

    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly
    
    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

    pthread_join(customerZero, NULL);
   // pthread_join(customerOne, NULL);
 /* pthread_join(customerTwo, NULL);
    pthread_join(customerThree, NULL);
    pthread_join(customerFour, NULL);*/
    pthread_mutex_destroy(&lock);

    return EXIT_SUCCESS;
}
