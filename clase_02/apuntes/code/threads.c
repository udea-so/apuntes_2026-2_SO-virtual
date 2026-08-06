#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>


double getTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void wait(int howlong) {
    double t = getTime();
    while ((getTime() - t) < (double) howlong)
      ; //wait...
}

volatile int counter = 0; 
int increments;

void *worker(void *arg) {
    int i;
    for (i = 0; i < increments; i++) {
      counter++;
    }
    return NULL;
}

/*
gcc threads.c -o threads

./threads 1000      (x4)

./threads 100000    (x4)

pkill cpu
*/

int main(int argc, char *argv[]) {
    increments = atoi(argv[1]);
    pthread_t p1, p2;
  
    //create two threads
    pthread_create(&p1, NULL, worker, NULL); 
    pthread_create(&p2, NULL, worker, NULL);
  
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("Final value   : %d\n", counter);
    return 0;
}