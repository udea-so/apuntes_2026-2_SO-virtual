#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/*
1. To compile:

gcc -o t1_lock t1_lock.c -lpthread -Wall

2. To run

./t1_lock <MAX_VALUE>
*/

int max;
volatile int counter = 0; // shared global variable

pthread_mutex_t lock; // lock variable (Mutex)

void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread)     
    for (i = 0; i < max; i++) {
        pthread_mutex_lock(&lock);      // -------------------------------------- Acquire the lock (adquire(L)) ----------------------------------------------
        counter = counter + 1;          // critical section: only one
        pthread_mutex_unlock(&lock);    // -------------------------------------- Release the lock (release(L)) ----------------------------------------------
    }
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[]) {                    
    if (argc != 2) {
        fprintf(stderr, "usage: main-first <loopcount>\n");
        exit(1);
    }
    max = atoi(argv[1]);

    pthread_t p1, p2;
    int rc = pthread_mutex_init(&lock, NULL); // Initialize the lock
    if (rc != 0) {
        fprintf(stderr, "main: mutex init failed\n");
        exit(1);
    }
    printf("main: begin [counter = %d]\n", counter);
    pthread_create(&p1, NULL, mythread, "A"); 
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    printf("main: done\n [counter: %d]\n [should: %d]\n", 
	   counter, max*2);
    return 0;
}

