# Tarea (Simulación)

Este programa, `x86.py`, le permite observar cómo diferentes entrelazados (interleavings) de hilos pueden causar o evitar condiciones de carrera. Es la tarea del capítulo 26 de OSTEP (*Concurrency: An Introduction*) y complementa al ejemplo `t1` de [`../ejemplos/api-thread/`](../ejemplos/api-thread/README.md): allí la race condition aparece cuando el sistema operativo quiere; aquí **usted decide** dónde caen las interrupciones y puede seguir el efecto instrucción por instrucción. La sección siguiente explica lo necesario para usarlo; el [README original de OSTEP](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/threads-intro) tiene más detalles.

## Cómo funciona `x86.py`

### Qué simula

Una CPU muy simple, parecida a x86, que ejecuta uno o varios **hilos** del mismo programa. Los hilos comparten la **memoria** (por ejemplo, la dirección `2000`), pero cada uno tiene **sus propios registros** (`%ax`, `%bx`, `%cx`, `%dx`) y su propio contador de programa: es exactamente la información que el sistema operativo guarda en el TCB de cada hilo. Cada cierto número de instrucciones ocurre una **interrupción** del timer y el simulador cambia al otro hilo, guardando los registros del que sale y restaurando los del que entra.

Los programas son archivos `.s` en un ensamblador reducido:

| Archivo | Qué hace | Preguntas |
|---|---|---|
| `loop.s` | Decrementa `%dx` en un ciclo hasta que se vuelve negativo; no usa memoria compartida | 1–3 |
| `looping-race-nolock.s` | En un ciclo de `%bx` vueltas, incrementa el valor de la dirección `2000` con `mov`/`add`/`mov`, igual que `counter = counter + 1` en `t1` | 4–8 |
| `wait-for-me.s` | Un hilo (con `%ax = 1`) escribe `1` en la dirección `2000`; el otro (con `%ax = 0`) se queda en un ciclo leyendo `2000` hasta ver ese `1` | 9–10 |
| `simple-race.s` | Un solo incremento de la dirección `2000` (sin ciclo); se usa en el ejemplo de abajo | — |

### Banderas

| Bandera | Significado | Ejemplo |
|---|---|---|
| `-p` | Programa `.s` a ejecutar | `-p loop.s` |
| `-t` | Número de hilos | `-t 2` |
| `-i` | Interrupción cada cuántas instrucciones (por defecto, 50) | `-i 3` |
| `-r` | Hace aleatorio el intervalo de interrupción (de 1 hasta el valor de `-i`) | `-i 4 -r` |
| `-s` | Semilla aleatoria (otra semilla, otro entrelazado) | `-s 1` |
| `-a` | Valor inicial de registros por hilo: hilos separados por coma, registros de un mismo hilo por dos puntos | `-a dx=3,dx=3` |
| `-R` | Registros que se muestran en la traza | `-R ax,bx` |
| `-M` | Direcciones de memoria que se muestran en la traza | `-M 2000` |
| `-c` | Calcula y muestra los valores (sin `-c` aparecen `?` para que usted los prediga) | `-c` |

El programa se ejecuta con `python3 x86.py ...` o, si tiene permiso de ejecución, con `./x86.py ...`.

### Cómo leer la traza

Ejemplo con `simple-race.s`, dos hilos e interrupciones cada 2 instrucciones:

```bash
python3 x86.py -p simple-race.s -t 2 -M 2000 -R ax -i 2 -c
```

```
 2000      ax          Thread 0                Thread 1
    0       0
    0       0   1000 mov 2000(%bx), %ax
    0       1   1001 add $1, %ax
    0       0   ------ Interrupt ------  ------ Interrupt ------
    0       0                            1000 mov 2000(%bx), %ax
    0       1                            1001 add $1, %ax
    0       1   ------ Interrupt ------  ------ Interrupt ------
    1       1   1002 mov %ax, 2000(%bx)
    1       1   1003 halt
    1       1   ----- Halt;Switch -----  ----- Halt;Switch -----
    1       1   ------ Interrupt ------  ------ Interrupt ------
    1       1                            1002 mov %ax, 2000(%bx)
    1       1                            1003 halt
```

- Las columnas de la izquierda son las direcciones de memoria (`-M`) y los registros (`-R`) pedidos; las de la derecha, una por hilo, muestran la instrucción que ejecuta cada uno (con su dirección: `1000`, `1001`, ...).
- Cada fila muestra los valores **después** de ejecutar la instrucción de esa fila.
- Los registros que se ven son los **del hilo que está corriendo**. Por eso, en la primera interrupción, `ax` "vuelve" a 0: no es que cambie, es que ahora se ve el `%ax` del hilo 1, que aún no ha cargado nada. El `%ax = 1` del hilo 0 quedó guardado y reaparece al volver a él.
- `Interrupt` marca un cambio de hilo por el timer; `Halt;Switch` marca que un hilo terminó y se pasa al otro.

En esta traza los dos hilos leen `2000` cuando todavía vale 0, ambos calculan 1 y ambos guardan 1: el valor final es **1** aunque se hicieron dos incrementos. Es la misma situación que analizan OSTEP (§26.4, Fig. 26.7) y el diagrama de `t1`, ahora reproducible a voluntad. Pruebe el mismo comando con `-i 3`: ¿qué cambia?

### Relación con el resto de la clase

- **Preguntas 1–8** → §26.3 y §26.4 de OSTEP: datos compartidos, *uncontrolled scheduling*, sección crítica. Compare `looping-race-nolock.s` con las tres instrucciones de `counter = counter + 1` que se obtienen con `objdump` en la sección "Del C al ensamblador" de `t1`.
- **Preguntas 9–10** → §26.6 (*One More Problem: Waiting For Another*): un hilo que espera a que otro haga algo. Aquí la espera se hace revisando la memoria en un ciclo; compárelo con `pthread_join` en `t0`, que también espera a otro hilo pero bloqueando al que espera.

## Preguntas

1. Examine un programa simple, "loop.s". Primero, léalo y compréndalo. Luego, ejecútelo con los siguientes argumentos (`./x86.py -t 1 -p loop.s -i 100 -R dx`). Esto especifica un único hilo, una interrupción cada 100 instrucciones, y el rastreo (tracing) del registro %dx. ¿Cuál será el valor de %dx durante la ejecución? Emplee la bandera `-c` para verificar sus respuestas; las respuestas, a la izquierda, muestran el valor del registro (o el valor de memoria) después de que se ha ejecutado la instrucción de la derecha.

2. Mismo código, banderas diferentes: (`./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx`). Esto especifica dos hilos, e inicializa cada %dx en 3. ¿Qué valores verá %dx? Ejecute con `-c` para verificar. ¿La presencia de múltiples hilos afecta sus cálculos? ¿Hay una condición de carrera en este código?

3. Ejecute esto: `./x86.py -p loop.s -t 2 -i 3 -r -R dx -a dx=3,dx=3`. Esto hace que el intervalo de interrupción sea pequeño/aleatorio; emplee diferentes semillas (`-s`) para ver diferentes entrelazados. ¿La frecuencia de interrupción cambia algo?

4. Ahora, un programa diferente, `looping-race-nolock.s`, que accede a una variable compartida ubicada en la dirección 2000, denominada `value`. Ejecútelo con un único hilo para confirmar su comprensión: `./x86.py -p looping-race-nolock.s -t 1 -M 2000`. ¿Cuál es el valor de `value` (es decir, en la dirección de memoria 2000) a lo largo de la ejecución? Emplee `-c` para verificar.

5. Ejecute con múltiples iteraciones/hilos: `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000`. ¿Por qué cada hilo ejecuta el loop tres veces? ¿Cuál es el valor final de `value`?

6. Ejecute con intervalos de interrupción aleatorios: `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0` con diferentes semillas (`-s 1`, `-s 2`, etc.). ¿Puede determinar, observando el entrelazado de hilos, cuál será el valor final de `value`? ¿Importa el momento (timing) de la interrupción? ¿Dónde puede ocurrir de forma segura? ¿Dónde no? Es decir, ¿dónde está exactamente la sección crítica?

7. Ahora examine intervalos de interrupción fijos: `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1`. ¿Cuál será el valor final de la variable compartida `value`? ¿Qué sucede cuando cambia a `-i 2`, `-i 3`, etc.? ¿Para cuáles intervalos de interrupción el programa da la respuesta "correcta"?

8. Ejecute lo mismo con más loops (por ejemplo, establezca `-a bx=100`). ¿Qué intervalos de interrupción (`-i`) llevan a un resultado correcto? ¿Cuáles intervalos son sorprendentes?

9. Un último programa: `wait-for-me.s`. Ejecute: `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000`. Esto establece el registro %ax en 1 para el hilo 0, y en 0 para el hilo 1, y observa %ax y la posición de memoria 2000. ¿Cómo debería comportarse el código? ¿Cómo está siendo empleado el valor en la posición 2000 por los hilos? ¿Cuál será su valor final?

10. Ahora invierta las entradas: `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000`. ¿Cómo se comportan los hilos? ¿Qué está haciendo el hilo 0? ¿Cómo cambiaría el resultado de la traza si cambia el intervalo de interrupción (por ejemplo, `-i 1000`, o quizás empleando intervalos aleatorios)? ¿Está el programa empleando la CPU de forma eficiente?