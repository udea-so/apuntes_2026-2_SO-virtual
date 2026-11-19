![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Señalización padre-hijo con la interfaz `my_cv`

Este ejemplo es el mismo *join* de [`diapositivas/ordering_sem/`](../diapositivas/ordering_sem/README.md) (diapositivas p. 21–26), reorganizado: el semáforo queda oculto detrás de una pequeña interfaz, `my_cv`, con tres funciones (`my_cv_init`, `my_cv_wait`, `my_cv_signal`), y se le pasa al hijo como argumento en lugar de usar una variable global. **No aparece en las diapositivas**; sirve para ver el semáforo usado como herramienta de señalización dentro de un módulo propio y para discutir en qué se diferencia de una variable de condición.

Volver al [índice de la parte 1](../README.md).

## Archivos

| Archivo | Contenido |
|---|---|
| [`my_cv.h`](my_cv.h), [`my_cv.c`](my_cv.c) | La interfaz `my_cv`: un semáforo inicializado en 0 |
| [`sem_as_CV.c`](sem_as_CV.c) | El padre crea al hijo y espera su señal con `my_cv_wait()` |

## Compilación

```bash
make          # genera sem_as_CV.out (a partir de sem_as_CV.c y my_cv.c)
make clean    # borra los ejecutables
```

## El código

```c
typedef struct {
    sem_t sem;
} my_cv;

void my_cv_init(my_cv *cv)   { Sem_init(&cv->sem, 0); }   // semáforo en 0
void my_cv_wait(my_cv *cv)   { Sem_wait(&cv->sem); }
void my_cv_signal(my_cv *cv) { Sem_post(&cv->sem); }
```

```c
void *child(void *arg) {
    my_cv *cv = (my_cv *)arg;     // la "variable de condición" llega como argumento
    printf("child\n");
    my_cv_signal(cv);
    return NULL;
}

int main(int argc, char *argv[]) {
    my_cv cond;
    pthread_t c;
    my_cv_init(&cond);
    printf("parent: begin\n");
    Pthread_create(&c, NULL, child, &cond);
    my_cv_wait(&cond);
    printf("parent: end\n");
    return 0;
}
```

Pasar `&cond` como argumento funciona porque `cond` vive en el *stack* de `main`, y `main` no retorna hasta después de `my_cv_wait()`: la variable sigue existiendo mientras el hijo la usa.

## Ejecución

Acepta los mismos argumentos opcionales que `ordering_sem` (segundos que se demora el hijo y segundos que se demora el padre):

```bash
./sem_as_CV.out [espera_hijo] [espera_padre]
```

Salida de referencia:

```text
$ ./sem_as_CV.out
parent: begin
child
parent: end

$ time ./sem_as_CV.out 1 0
parent: begin
child
parent: end

real	0m1.017s
user	0m0.016s
sys	0m0.000s
```

| Ejecución | Repeticiones | Orden correcto |
|---|---|---|
| `./sem_as_CV.out` | 500 | 500 |
| `./sem_as_CV.out 1 0` (el padre espera primero) | 1 | Sí |
| `./sem_as_CV.out 0 1` (el hijo señala primero) | 1 | Sí |

## `my_cv` no es una variable de condición

El nombre viene del papel que cumple: en el *join* de la clase 17 ese papel lo hacía una variable de condición. Pero hay dos diferencias importantes:

| | Variable de condición (`pthread_cond_t`) | `my_cv` (semáforo en 0) |
|---|---|---|
| Señal cuando nadie espera | Se pierde: `pthread_cond_signal()` no tiene efecto | Se recuerda: el valor sube a 1 y el siguiente `wait` no se bloquea |
| Mutex | `wait` recibe un mutex, lo libera al dormir y lo readquiere al despertar | No usa mutex |
| Estado | Necesita una variable de estado aparte (`done`) | El propio valor del semáforo es el estado |

Por la primera diferencia, el *join* con semáforo no necesita la variable `done`. Por la segunda, `my_cv` no sirve como reemplazo general de una variable de condición: cuando un hilo debe revisar una condición sobre datos compartidos (por ejemplo, "el buffer no está vacío") y dormirse si no se cumple, esa revisión y el dormirse tienen que ocurrir con un lock adquirido, que es justamente lo que `pthread_cond_wait()` hace de forma atómica. Construir una variable de condición completa a partir de semáforos es mucho más difícil de lo que parece: OSTEP (§31.8) cuenta que programadores con mucha experiencia lo intentaron en Windows y cometieron varios errores, documentados por Andrew Birrell en *Implementing Condition Variables with Semaphores* (2004).

## Para probar por cuenta propia

1. Inicializar el semáforo de `my_cv_init()` en 1. ¿El padre espera al hijo?
2. Crear tres hijos que compartan el mismo `my_cv` y cambiar el padre para que espere a los tres. ¿Cuántas veces debe llamar a `my_cv_wait()`?
3. Si el hijo terminara de forma anormal antes de llamar a `my_cv_signal()`, ¿qué le pasaría al padre?

## Referencias

- Diapositivas de la clase 18, parte 1, p. 21–26.
- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf), §31.3 y §31.8.

> [!IMPORTANT]
> **Sobre esta guía.** El código y la guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP. Las salidas son de una ejecución de referencia del 19/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente.
