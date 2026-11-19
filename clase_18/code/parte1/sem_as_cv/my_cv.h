#ifndef __MY_CV_H__
#define __MY_CV_H__

#include <semaphore.h>

// Estructura simplificada: solo contiene el semáforo para la señalización
typedef struct {
    sem_t sem; 
} my_cv;

// Inicializa el semáforo en 0
void my_cv_init(my_cv *cv);

// Espera la señal (wrapper de sem_wait)
void my_cv_wait(my_cv *cv);

// Envía la señal (wrapper de sem_post)
void my_cv_signal(my_cv *cv);

#endif // __MY_CV_H__
