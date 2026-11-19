# La Cena de los Filósofos (The Dining Philosophers)

Este directorio contiene la implementación del clásico problema de sincronización **"Dining Philosophers"**, basado en la Sección 31.6 del libro *Operating Systems: Three Easy Pieces* (Remzi).

El objetivo es demostrar problemas complejos de concurrencia como el **Deadlock** (Bloqueo Mutuo) y la **Inanición** (Starvation), y cómo resolverlos utilizando semáforos y ordenamiento de recursos.

## Descripción del Escenario

* **5 Filósofos** se sientan alrededor de una mesa redonda.
* Pasan su vida alternando entre dos estados: **Pensar** y **Comer**.
* Para comer, un filósofo necesita **dos tenedores**: el que está a su izquierda y el que está a su derecha.
* **Restricción:** Hay solo 5 tenedores en total (uno entre cada par de filósofos). Si un filósofo toma un tenedor, su vecino no puede usarlo.

### Estructura Lógica

Cada "tenedor" se representa como un **Semáforo Binario** (o Mutex).

```mermaid
graph TD
    P1((P1)) --- F1[Tenedor 1]
    F1 --- P2((P2))
    P2 --- F2[Tenedor 2]
    F2 --- P3((P3))
    P3 --- F3[Tenedor 3]
    F3 --- P4((P4))
    P4 --- F4[Tenedor 4]
    F4 --- P5((P5))
    P5 --- F5[Tenedor 5]
    F5 --- P1
    
    style P1 fill:#e1f5fe,stroke:#01579b
    style P2 fill:#e1f5fe,stroke:#01579b
    style P3 fill:#e1f5fe,stroke:#01579b
    style P4 fill:#e1f5fe,stroke:#01579b
    style P5 fill:#e1f5fe,stroke:#01579b
    style F1 fill:#fff9c4,stroke:#fbc02d
    style F2 fill:#fff9c4,stroke:#fbc02d
    style F3 fill:#fff9c4,stroke:#fbc02d
    style F4 fill:#fff9c4,stroke:#fbc02d
    style F5 fill:#fff9c4,stroke:#fbc02d
```


## El Problema: Deadlock (Abrazo Mortal)

Si implementamos una lógica ingenua donde todos los filósofos actúan igual:

1.  Tomar tenedor izquierdo.
2.  Tomar tenedor derecho.
3.  Comer.

...y todos intentan comer al mismo tiempo, ocurre lo siguiente:

  * Todos toman su tenedor izquierdo simultáneamente.
  * Todos esperan que se libere el tenedor derecho.
  * **Resultado:** Nadie come, nadie suelta el tenedor. El sistema se detiene para siempre.

## La Solución (The Broken Dependency)

La solución implementada (sugerida por Remzi) consiste en **romper el ciclo de dependencia** cambiando el orden de adquisición de recursos para al menos uno de los filósofos.

  * **Filósofos 0 al 3:** Toman Izquierda (`Left`) y luego Derecha (`Right`).
  * **Filósofo 4 (El último):** Toma Derecha (`Right`) y luego Izquierda (`Left`).

### Pseudocódigo de la Solución

```c
void *philosopher(void *arg) {
    int p = (int) arg; // ID del filósofo
    
    if (p == 4) { // El último rompe el ciclo
        sem_wait(&forks[right(p)]);
        sem_wait(&forks[left(p)]);
    } else {      // Los demás siguen el orden estándar
        sem_wait(&forks[left(p)]);
        sem_wait(&forks[right(p)]);
    }
    
    eat();
    
    sem_post(&forks[left(p)]);
    sem_post(&forks[right(p)]);
}
```

### Diagrama de Flujo de la Solución

```mermaid
graph TD
    Start([Filósofo quiere comer]) --> Check{¿Soy el último?}
    
    Check -->|NO| L1[Tomar Izquierda]
    L1 --> R1[Tomar Derecha]
    
    Check -->|SÍ| R2[Tomar Derecha]
    R2 --> L2[Tomar Izquierda]
    
    R1 --> Comer([COMER])
    L2 --> Comer
    
    Comer --> Soltar[Soltar ambos tenedores]
    
    style Check fill:#ffcc00,stroke:#333
    style Comer fill:#ccffcc,stroke:#00aa00
```

## Compilación y Ejecución

Para facilitar la compilación de los múltiples archivos involucrados, se incluye un archivo `Makefile`.

### 1\. Compilar

Simplemente ejecuta el comando `make` en la terminal dentro de esta carpeta. El `Makefile` se encargará de enlazar las librerías necesarias (`-pthread`).

```bash
make
```

### 2\. Ejecutar

Una vez compilado, se generará el ejecutable (generalmente llamado `dining` o `main`).

```bash
./dining_philosophers
```

*(Verifica el nombre exacto del ejecutable generado por el Makefile haciendo un `ls`).*

### 3\. Limpiar

Para borrar los archivos binarios y objetos generados:

```bash
make clean
```

## Qué observar durante la ejecución

Al correr el programa, deberías ver mensajes en la consola indicando qué filósofo está comiendo. Gracias a la solución implementada:

1.  El programa **no se congela** (No hay Deadlock).
2.  Eventualmente, todos los filósofos logran comer (puedes verificar que no haya Starvation si la ejecución es larga).

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Puede contener errores.