## Introducción a LDR

La virtualización ocurre cuando el sistema operativo comparte el tiempo de la CPU entre múltiples trabajos que se ejecutan al mismo tiempo. 

El objetivo es ejecutar los procesos uno tras otro. Esto puede causar algunos problemas relacionados con el rendimiento y el control de la CPU.

El SO debe mantener el control sobre el sistema mientras virtualiza la CPU de manera eficiente. 

Este módulo aborda cómo responder a estas preguntas:
* ¿Cómo se puede virtualizar sin ralentizar el sistema?
* ¿Cómo se puede mantener el control mientras se ejecutan los procesos de manera eficaz?

Para lograr esto de manera efectiva, se necesita el apoyo del hardware y del sistema operativo. 


## Técnica Básica: Ejecución Directa Limitada

Los desarrolladores de sistemas operativos crearon la ejecución directa limitada para ayudar a que los programas se ejecuten lo más rápido posible.

La **ejecución directa** significa ejecutar el programa directamente en la CPU. Al iniciar un programa, el SO realiza los siguientes pasos:
* Agrega el programa a la lista de procesos.
* Asigna memoria para el programa.
* Carga el programa del disco a la memoria.
* Encuentra el punto de entrada del programa (como la función `main()`).
* Comienza a ejecutar el programa.

La figura a continuación demuestra el protocolo básico de ejecución directa. Observa cómo el SO se prepara para lanzar el programa, lo ejecuta y luego regresa al kernel.

<p align="center">
  <img src="protocolo_de_ejecucion_directa.gif" alt="Creacion de un proceso" width="400">
</p>

La ejecución directa puede causar algunos problemas al virtualizar la CPU.
* ¿Cómo nos aseguramos de que el programa no realice acciones no permitidas mientras se ejecuta de manera eficiente?
* ¿Cómo conmuta el SO de un proceso a otro al hacer uso del tiempo compartido (time-sharing)?

Limitar la ejecución tiene mucho sentido. Sin límites, el SO sería simplemente una biblioteca que invoca al siguiente programa a ejecutar.

## Operaciones Restringidas: Modo Usuario y Modo Kernel
Ejecutar un programa directamente en la CPU es rápido. Pero, ¿qué sucede si el proceso necesita realizar una operación restringida? Los procesos deberían poder realizar operaciones de E/S (Entrada/Salida) y otras operaciones restringidas sin tener el control total del sistema.

### ¿Cómo pueden el SO y el hardware hacer que esto suceda?

Permitir que cada proceso realice sus propias tareas de E/S y de otro tipo reduciría la protección. No se podrían validar los permisos antes de escribir en el disco. Para solucionar esto, el procesador tiene dos modos: un **modo usuario** restringido y un **modo kernel** privilegiado.

* **Modo usuario** - Este modo limita lo que el código puede hacer. Por ejemplo, no se permiten solicitudes de E/S directas. Si un proceso lo intentara, el SO probablemente lo terminaría.
* **Modo kernel** - En este modo es donde se ejecuta el SO (también llamado el kernel). El código que se ejecuta en este modo puede realizar libremente operaciones privilegiadas.

## Llamadas al Sistema (System Calls)

Sin embargo, los dos modos no resuelven el caso en que un proceso de usuario quiere realizar una operación privilegiada. Los programas que se ejecutan en modo usuario utilizan **llamadas al sistema** (**system calls**) para solicitar operaciones privilegiadas al kernel.

Las llamadas al sistema exponen las funcionalidades importantes del kernel:
* Trabajar con sistemas de archivos.
* Iniciar y terminar procesos.
* Comunicarse con otros procesos.
* Asignar más memoria.

Si un programa quiere realizar una operación privilegiada, debe ejecutar una instrucción especial llamada **trap**. La instrucción **trap** salta al kernel y cambia a modo kernel.

Una vez en el kernel, el sistema puede realizar las actividades requeridas y permitidas para el proceso. Al finalizar, el sistema operativo emite una instrucción de **retorno del trap** (**return-from-trap**). Esta instrucción devuelve el control al programa y restablece el modo a modo usuario.

Ejecutar instrucciones **trap** implica que el hardware debe guardar el estado de los registros del proceso llamador, para que el SO pueda reanudar la ejecución donde la dejó después de la instrucción de retorno.

El SO debe controlar cuidadosamente qué código se ejecuta cuando se invoca una instrucción **trap**. No queremos que los programas ejecuten código arbitrario en el kernel. Eso nos lleva a nuestro siguiente tema, la tabla de traps (**trap table**).

## Tablas de Traps (Trap Tables)

Una **tabla de traps** se crea durante el proceso de arranque (**boot**). Las máquinas inician en modo kernel para poder realizar tareas sensibles. El kernel le informa al hardware la ubicación de los manejadores de traps (trap handlers). De esta manera, el hardware sabe qué hacer cuando recibe solicitudes de un programa.

A cada llamada al sistema se le asigna un **número de llamada al sistema** (**system-call number**). El programa de usuario guarda este número en un registro o en la pila (stack) antes de hacer la llamada. Si este número corresponde a una llamada al sistema válida, el kernel ejecuta la acción solicitada. Las tablas de traps son muy poderosas, ya que impiden que los programas de usuario accedan directamente a partes restringidas del kernel.

La siguiente figura muestra cómo es este proceso. Asume que los registros de cada proceso son guardados (y restaurados) por el hardware.

<p align="center">
  <img src="LDEProtocol.gif" alt="Ejecucion directa limitada" width="400">
</p>

Asumimos que cada proceso tiene una pila de kernel (kernel stack) donde los registros son guardados y restaurados.

El protocolo de Ejecución Directa Limitada (LDE) se puede resumir en dos fases:
1. En el arranque, el kernel inicializa las tablas de traps y la CPU recuerda su ubicación. El kernel hace esto ejecutando código privilegiado.
2. Durante la ejecución, cuando un proceso necesita un servicio del SO (como crear otro proceso o salir), realiza una llamada al sistema. Esto causa un trap al SO. El kernel maneja la solicitud y devuelve el control con una instrucción de retorno del trap (return-from-trap).

Cuando un programa termina, lo hace con una llamada al sistema `exit()`, que transfiere el control de nuevo al SO mediante un trap. Finalmente, el SO se encarga de limpiar el proceso, eliminándolo de la lista de procesos y liberando su memoria.

## Problema 2: Conmutación Entre Procesos

La conmutación de procesos (**Process switching**) es la otra área de preocupación con la ejecución directa. Aparentemente, es un problema fácil de resolver: simplemente, dejar que el SO se encargue. Sin embargo, el SO no se está ejecutando cuando otro proceso está corriendo en la CPU.

¿Cómo puede el SO hacer su trabajo si no se está ejecutando? En resumen, no puede. Esto nos lleva a la siguiente pregunta: ¿cómo puede el SO recuperar el control de la CPU para poder cambiar de proceso?

## Esperando Llamadas al Sistema: Un Enfoque Cooperativo

Un enfoque cooperativo significa confiar en que los procesos del sistema cederán ocasionalmente el control de la CPU para permitir que se realicen otras tareas.

Cuando las aplicaciones intentan realizar actividades no autorizadas, **le devuelven el control al SO**. El kernel genera un trap si un programa de usuario intenta acceder a una parte restringida del sistema operativo. Esto le da el control de la CPU al SO, y el sistema a menudo termina al proceso infractor.

La planificación cooperativa implica que el SO recupera el control de la CPU a través de una llamada al sistema o de una actividad no autorizada. Pero, ¿qué sucede si hay un bucle infinito, sin llamadas al sistema ni actividades no autorizadas?

## El SO toma el control: un enfoque no cooperativo

El sistema operativo confía en el hardware para recuperar el control de la CPU cuando un proceso no realiza una llamada al sistema ni comete un error. Una **interrupción de temporizador** (**timer interrupt**) es un temporizador integrado que genera una interrupción después de un número específico de milisegundos. Esta señal detendrá al proceso actual. El intervalo de tiempo suele ser lo suficientemente largo para que las tareas normales se completen, pero evita que procesos como un bucle infinito monopolicen la CPU.

Durante el arranque (una actividad privilegiada), el kernel le indica al hardware qué código ejecutar cuando se reciba una señal de interrupción. En caso de una interrupción de temporizador, el SO recupera el control de la CPU. El hardware también debe guardar el estado del programa (registros, etc.) para que el sistema pueda reanudarlo más tarde, después de la instrucción de retorno del trap.

## Guardando y Restaurando el Contexto

### Context Switch
 
Cuando un SO conmuta procesos, realiza un cambio de contexto (**context switch**). Un cambio de contexto hace uso de la pila del kernel para almacenar los valores de los registros. El SO guarda los valores de los registros del proceso actual y restaura los valores del siguiente proceso. Los valores de los registros son guardados en la pila del kernel. Al guardar el estado de los procesos, el kernel puede conmutar fácilmente entre ellos. La conmutación parece fluida para el usuario.

Guardar el contexto del proceso actual requiere que el SO:
* Guarde los registros de propósito general, el PC y el puntero de pila del kernel del proceso actual.
* Luego restaure todos esos y cambie a la pila del kernel para que el siguiente proceso se ejecute.

Cambiar de pila significa que el kernel entra a la llamada al código de conmutación como el proceso interrumpido y regresa como el proceso que pronto se ejecutará. Luego, cuando el SO ejecuta una instrucción de retorno del trap, el proceso que pronto se ejecutará se convierte en el proceso en ejecución.

<p align="center">
  <img src="ldeTimer.gif" alt="Cambio de contexto" width="400">
</p>

La animación a la anterior ilustra la conmutación de contexto:
* El temporizador interrumpe al Proceso A
* Los registros se guardan en la pila del kernel
* El SO conmuta al Proceso B
* El sistema luego llama a la rutina `switch()` que hace tres cosas:
  * Guarda los valores de los registros entrantes (en la estructura de proceso del Proceso A)
  * Restaura los registros del Proceso B (desde su estructura de proceso)
  * Conmuta el contexto al cambiar entre las pilas de B y A
* El SO sale del trap y restaura los registros del Proceso B y lanza el proceso

Durante todo esto, hay dos tipos de guardado y restauración de registros que suceden:
* Con una interrupción de temporizador, el hardware guarda los registros del proceso actual en la pila del kernel.
* Cuando el SO conmuta de A a B, el sistema guarda los registros del kernel en la memoria en la estructura del proceso.

Esto cambia el comportamiento del sistema del Proceso A al Proceso B de una manera similar a hacer un trap al kernel desde el Proceso A.

## Problemas de Concurrencia

¿Cómo maneja el sistema una interrupción de temporizador cuando el programa se está ejecutando? ¿Qué sucede cuando una interrupción de temporizador ocurre cuando el kernel ya está tratando con otra interrupción de temporizador?

La concurrencia es la habilidad del sistema operativo de manejar varias cosas a la vez. Hablaremos más sobre este tema en un curso posterior. Pero por ahora, veamos cómo el SO puede manejar las preguntas anteriores.

Deshabilitar las interrupciones mientras se intenta procesar otra interrupción es una posible solución. Sin embargo, el sistema operativo puede perder interrupciones si están deshabilitadas por demasiado tiempo.

Los métodos de bloqueo evitan que los procesos intenten acceder a los mismos recursos al mismo tiempo. De esta manera se pueden tener varias actividades del kernel ejecutándose al mismo tiempo. Sin embargo, bloquear procesos puede conducir a errores que son difíciles de rastrear en su código.

# Resumen

Hemos discutido maneras en las que implementar la virtualización de la CPU, llamada ejecución directa limitada. La idea general es establecer límites sobre lo que un proceso puede hacer durante la ejecución.

Estos límites incluyen:
* Crear manejadores de trap en el momento del arranque. 
  * Los programas no pueden alterar la tabla de traps.
  * Usar una llamada al sistema para solicitar servicios del kernel mediante un trap.
  * Las llamadas de trap guardan el estado de los registros mientras conmutan procesos.
  * Se emite una instrucción de retorno del trap después de que se complete el servicio del kernel.
* Implementar un temporizador de interrupción. 
  * Un temporizador de hardware que puede interrumpir un proceso después de un tiempo específico.
  * Esta es una forma no cooperativa de compartir la CPU.
  * La conmutación de contexto es el resultado de una interrupción.
* Ejecutar tareas sensibles en un modo restringido. 
  * El modo usuario está restringido en cuanto a lo que un proceso puede hacer.
  * El modo kernel le da al proceso acceso limitado a tareas sensibles.
  * Los modos se cambian durante el manejo de traps.

La información anterior asegura que los procesos se ejecuten eficientemente, de forma segura y no monopolicen la CPU.
