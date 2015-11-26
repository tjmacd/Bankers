/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
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

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

pthread_mutex_t mutex;
pthread_mutex_t console;

void print_resources(){
	pthread_mutex_lock(&console);
	printf("Available: ");
	for(int i = 0; i < NUM_RESOURCES; i++){
		printf("%d ", available[i]);
	}
	printf("\n");
	pthread_mutex_unlock(&console);
}

void print_need(int customer){
	pthread_mutex_lock(&console);
	printf("Need for %d: ", customer);
	for(int i = 0; i < NUM_RESOURCES; i++){
		printf("%d ", need[customer][i]);
	}	
	printf("\n");
	pthread_mutex_unlock(&console);
}

void print_result(int num, int request[], bool safe){
	pthread_mutex_lock(&console);
	printf("Request from %d: ", num);
	for(int j = 0; j < NUM_RESOURCES; j++){
		printf("%d ", request[j]);
	}
	if(!safe){
		printf("State Unsafe; Request denied\n");
	} else {
		printf("State Safe; Request accepted\n");
	}
	pthread_mutex_unlock(&console);
}

void* requestReleaseRepeat (void* arg) {
    int customerNumber = (int) arg;
	//int count = 10;
	bool finished = false;
    while(!finished) {
        int request[NUM_RESOURCES];
		finished = true;
        for (int i = 0; i < NUM_RESOURCES; i++) {
			if(need[customerNumber][i] != 0){
				finished = false;
	            request[i] = rand() % (need[customerNumber][i]+1);
			} else {
				request[i] = 0;
			}
            //printf("%d ", request[i]);
        }
		//printf("\n");
		//print_resources();
		
		bool success;
		
		//do {
		//print_need(customerNumber);
		pthread_mutex_lock(&mutex);
		success = request_res(customerNumber, request);
		pthread_mutex_unlock(&mutex);
			//printf("success: %d\n", success);
		//} while(!success);
		
        //while(!request_res(customerNumber, request));
		
		//print_resources();
		if(success){
			pthread_mutex_lock(&mutex);
    	    release_res(customerNumber, request);
			pthread_mutex_unlock(&mutex);
		}
		//print_resources();
		//count--;
		sleep(1);
    }
	printf("Process %d completed\n", customerNumber);
    return NULL;
}

// Define functions declared in banker.h here
bool request_res(int n_customer, int request[])
{
    for(int j = 0; j < NUM_RESOURCES; j++){
		if(request[j] > need[n_customer][j]){
			puts("Error: requesting more than you need");
			return false;
		}
	}
	for(int j = 0; j < NUM_RESOURCES; j++){
		if(request[j] > available[j]){
			puts("ERROR: resources not available");
			return false;
		}
	}
	for(int j = 0; j < NUM_RESOURCES; j++){
		available[j] -= request[j];
		allocation[n_customer][j] += request[j];
		
	}
	bool safe = is_safe();
	print_result(n_customer, request, safe);
	if(!safe){
		release_res(n_customer, request);
	} else {
		for(int j = 0; j < NUM_RESOURCES; j++){
			need[n_customer][j] -= request[j];
		}
	}
	return safe;
}

// Release resources, returns true if successful
bool release_res (int n_customer, int amount[]) {
	pthread_mutex_lock(&console);
	printf("Released ");
    for(int j = 0; j < NUM_RESOURCES; j++){
		available[j] += amount[j];
		allocation[n_customer][j] -= amount[j];
		//need[n_customer][j] += amount[j];
		printf("%d ", amount[j]);
	}
	printf("from %d\n", n_customer);
	pthread_mutex_unlock(&console);
	return true;
}

bool lessthan_equalto(int array1[], int array2[], int length){
	for(int i = 0; i < length; i++){
		if(array1[i] > array2[i]){
			return false;
		}
	}
	return true;
}

// Check if state is safe
bool is_safe() {
	int work[NUM_RESOURCES];
	for(int i = 0; i < NUM_RESOURCES; i++){
		work[i] = available[i];
	}
	bool finish[NUM_CUSTOMERS] = {false};
	bool found;
	do {
		found = false;
		for (int i = 0; i < NUM_CUSTOMERS; i++){
			if(finish[i] == false){	
				//for(int j = 0; j < NUM_RESOURCES; j++){
				//	need[i][j] = maximum[i][j] - allocation[i][j];
				//}
				if(lessthan_equalto(need[i], work, NUM_RESOURCES)){
					found = true;
					for(int j = 0; j < NUM_RESOURCES; j++){
						work[j] += allocation[i][j];
					}
					finish[i] = true;
				}
			}
		}
	} while(found);

	for(int i = 0; i < NUM_CUSTOMERS; i++){
		if(finish[i] == false){
			return false;
		}
	}
	return true;
}


int main(int argc, char *argv[]) {
    // define local variables
    pthread_t customer[NUM_CUSTOMERS];

    if (argc != NUM_RESOURCES+1) {
        printf("Error: Incorrect amount of arguments supplied. Requires: %d Received: %d\n", NUM_RESOURCES, argc-1);
        exit(0);
    }

    // initialize available resources
    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = atoi(argv[i+1]);
		for(int j = 0; j < NUM_CUSTOMERS; j++){
	        maximum[j][i] = rand() % available[i];
			need[j][i] = maximum[j][i];
		}
    }

	for(int i = 0; i < NUM_CUSTOMERS; i++){
	    pthread_create(&customer[i], 0, requestReleaseRepeat, (void*) i);
	}

    // ==================== YOUR CODE HERE ==================== //

    // Initialize the pthreads, locks, mutexes, etc.

    // Run the threads and continually loop

    // The threads will request and then release random numbers of resources

    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly
    
    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

	for(int i = 0; i < NUM_CUSTOMERS; i++){
	    pthread_join(customer[i], NULL);
	}

    return EXIT_SUCCESS;
}
