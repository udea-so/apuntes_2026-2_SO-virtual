/*
 * my_cv.h - Señalización entre hilos con un semáforo.
 *
 * Se llama my_cv porque cumple el papel que tuvo la variable de condición
 * en el join de la clase 17, pero no es una variable de condición: no
 * recibe un mutex y, a diferencia de pthread_cond_signal(), una señal
 * enviada cuando nadie espera NO se pierde, porque queda contada en el
 * valor del semáforo.
 */
#ifndef __MY_CV_H__
#define __MY_CV_H__

#include <semaphore.h>

// Estructura simplificada: solo contiene el semáforo para la señalización
typedef struct {
    sem_t sem;
} my_cv;

// Inicializa el semáforo en 0
void my_cv_init(my_cv *cv);

// Espera la señal (sem_wait)
void my_cv_wait(my_cv *cv);

// Envía la señal (sem_post)
void my_cv_signal(my_cv *cv);

#endif // __MY_CV_H__
