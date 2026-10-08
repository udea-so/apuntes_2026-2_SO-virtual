#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
1. To compile:

gcc -o t2 t2.c -lpthread -Wall

2. To run

./t2
*/

typedef struct __myarg_t {
  int a;
  int b;
} myarg_t;

typedef struct __myret_t {
  int x;
  int y;
} myret_t;

void *mythread(void *arg) {
  myarg_t *m = (myarg_t *) arg;
  printf("%d %d\n", m->a, m->b);
  myret_t *r = malloc(sizeof(myret_t)); // allocated in heap (Dont forget to free it)
  r->x = 1;
  r->y = 2;
  return (void *) r;
}

int main(int argc, char *argv[]) {
  pthread_t p;
  myret_t *m;

  myarg_t args;
  args.a = 10;
  args.b = 20;
  pthread_create(&p, NULL, mythread, &args);
  pthread_join(p, (void **) &m); // this thread has been
                                 // waiting inside of the
                                 // pthread_join() routine.
  printf("returned %d %d\n", m->x, m->y);
  free(m); // free the allocated memory
  // printf("original %d %d\n", args.a, args.b);  
  return 0;
}

