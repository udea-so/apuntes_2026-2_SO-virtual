#include "sloppy_counter.h"


// init: record threshold, init locks, init values
//       of all local counts and global count
void sloppy_init(sloppy_counter_t *c, int threshold) {
    int i;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    c->threshold = threshold;

    for (i = 0; i < NUMCPUS; i++) {
        // CAMBIO: Acceder a .value
        c->local[i].value = 0; 
        pthread_mutex_init(&c->llock[i].lock, NULL);
    }
}

// update: usually, just grab local lock and update local amount
//         once local count has risen by 'threshold', grab global
//         lock and transfer local values to it
void sloppy_update(sloppy_counter_t *c, int threadID, int amt) {
  // printf("%d\n", threadID);
  // printf("%d\n", c->threshold);

  pthread_mutex_lock(&c->llock[threadID].lock);
  // c->local[threadID] += amt;   // assumes amt > 0
  c->local[threadID].value += amt; // assumes amt > 0
  if (c->local[threadID].value >= c->threshold)
  { // transfer to global
    pthread_mutex_lock(&c->glock);
    c->global += c->local[threadID].value;
    pthread_mutex_unlock(&c->glock);
    c->local[threadID].value = 0;
  }
  pthread_mutex_unlock(&c->llock[threadID].lock);
}

// get: just return global amount (which may not be perfect)
int sloppy_get(sloppy_counter_t *c) {
  pthread_mutex_lock(&c->glock);
  int val = c->global;
  pthread_mutex_unlock(&c->glock);
  return val; 	// only approximate!
}
