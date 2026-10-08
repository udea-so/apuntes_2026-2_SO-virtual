![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Sistemas Operativos — Clase 14: Entorno de ejecución de los ejemplos

Los programas de esta clase ([`api-thread/`](api-thread/) y [`sum_vectores/`](sum_vectores/README.md)) miden cosas que dependen directamente del sistema operativo y del hardware: el orden en que el scheduler ejecuta los hilos, cuántas actualizaciones se pierden en una race condition y cuánto se acelera un programa al usar varios núcleos. Por eso los resultados **no** son iguales en todas las máquinas ni en todos los entornos. Este documento describe la máquina donde se obtuvieron las salidas de las guías, explica cómo influye cada una de sus características y qué cambia al ejecutar los ejemplos en WSL, en una máquina virtual, en Docker o en Linux nativo.

---

## Máquina de prueba

Todas las salidas y tablas de tiempos de las guías de esta clase se obtuvieron en esta máquina (octubre de 2026):

| Característica | Valor |
|---|---|
| Equipo | Portátil HP 245 G7 |
| CPU | AMD Ryzen 5 3500U (arquitectura Zen+), 2.1 GHz base / hasta 3.7 GHz en turbo |
| Núcleos | 4 físicos, 8 lógicos (SMT: 2 hilos por núcleo); `nproc` reporta 8 |
| Cachés | L2: 512 KiB por núcleo (2 MiB en total); L3: 4 MiB compartida |
| Memoria RAM | 20 GiB DDR4 en dos módulos **asimétricos** (4 GiB + 16 GiB), configurados a 2400 MT/s. Windows ve ~17.9 GiB porque los gráficos integrados (Radeon Vega) reservan parte de la RAM |
| Memoria libre en Linux al medir | ~7.6 GiB (`free -h`); el resto lo ocupaba Windows y sus aplicaciones |
| Sistema anfitrión | Windows 10 Pro, versión 1909 (build 18363) |
| Entorno Linux | **WSL1** con Ubuntu 20.04.6 LTS; `uname -r` reporta `4.4.0-18362-Microsoft` |
| Compilador | gcc 9.4.0 |
| Python | 3.8.10 |

Para obtener la misma ficha en tu máquina:

```bash
uname -a                                    # kernel; en WSL contiene "Microsoft" o "microsoft"
cat /etc/os-release | head -2               # distribución
lscpu | grep -E "Model name|Thread|Core|Socket|cache"
nproc                                       # núcleos lógicos que ve el proceso
free -h                                     # memoria total y disponible
gcc --version | head -1
python3 --version
```

---

## Cómo influye cada característica

### Núcleos físicos vs. lógicos (SMT)

`nproc` y `sysconf(_SC_NPROCESSORS_ONLN)` reportan **8**, por lo que `suma_p` crea 8 hilos por defecto. Pero solo hay 4 núcleos físicos: los dos hilos lógicos de cada núcleo comparten sus unidades de ejecución y sus cachés L1/L2. En las mediciones de la suma sin optimización (caso A de la [guía de C de la suma de vectores](sum_vectores/c/README.md#resultados-en-la-máquina-de-prueba)) el speedup subió con cada duplicación de hilos hasta 4 (≈ 3.4×) y pasar a 8 hilos solo lo llevó a ≈ 4.4×. En una máquina con 8 núcleos físicos la curva seguiría subiendo; en una con 2, se aplanaría antes.

### Ancho de banda de memoria

Con `-O2`, la suma de vectores dejó de escalar con más hilos (caso B, speedup ≈ 1.2× como máximo). El límite es cuántos bytes por segundo puede entregar la RAM, y en esta máquina ese límite es modesto: es un portátil con DDR4 a 2400 MT/s y con módulos de distinto tamaño, configuración en la que solo una parte de la memoria trabaja en doble canal. Además, los gráficos integrados usan esa misma memoria. Una máquina de escritorio o un servidor con más canales de memoria obtendría un speedup mayor en ese caso, pero el fenómeno (un programa limitado por memoria escala mal) es el mismo en cualquier máquina.

### Memoria libre

Con `TAMANO = 1000000000` los tres arreglos ocupan ~12 GB, más de los ~7.6 GiB libres que había. El programa no falla de inmediato: Linux permite reservar más memoria de la disponible y empieza a usar **swap** cuando se agota. Lo que se mediría entonces sería, sobre todo, la velocidad del disco (tema de la clase 12, *swapping*). Por eso las guías usan `TAMANO = 100000000` (~1.2 GB).

### Portátil: frecuencia variable y ruido

La frecuencia de la CPU cambia según la temperatura, el plan de energía de Windows y si el equipo está conectado al cargador. Además, el turbo suele ser más alto cuando trabaja un solo núcleo que cuando trabajan todos, lo que favorece a la versión serial y reduce el speedup medido. A esto se suman los programas de Windows en segundo plano. En las mediciones esto se notó como dispersión: por ejemplo, Ts osciló entre 0.49 y 0.65 s y una de las tres ejecuciones de `suma_p` con 4 hilos dio 0.40 s cuando las otras dos dieron 0.17–0.19 s. Por eso las guías reportan medianas de varias ejecuciones.

---

## Particularidades de WSL1 observadas en estas pruebas

WSL1 **no** ejecuta un kernel Linux. Es una capa de Windows que traduce cada llamada al sistema de Linux a su equivalente en el kernel de Windows (NT). El kernel `4.4.0-18362-Microsoft` que reporta `uname -r` no existe como tal: es un número de compatibilidad. Los hilos de Linux se convierten en hilos de Windows y los planifica el **scheduler de Windows**, no el de Linux. Esto tuvo tres efectos concretos en las pruebas:

1. **`taskset` y `sched_setaffinity` no tienen efecto.** Se aceptan sin error, pero no restringen los núcleos. Se comprobó así: `suma_p` con 8 hilos bajo `taskset -c 0` (que debería usar un solo núcleo) tardó 0.11–0.13 s, lo mismo que sin `taskset`, mientras que con 1 hilo tardaba 0.53–0.65 s. Igualmente, `taskset -c 0 ./t1 10000000` siguió perdiendo millones de actualizaciones, como si los dos hilos corrieran en paralelo. Las preguntas que dependen de fijar núcleos deben hacerse en otro entorno.
2. **Las llamadas al sistema son caras.** Con `Spin()` activado, cada elemento hace dos llamadas a `gettimeofday()`, y la versión serial tardó ~0.8 s para un millón de elementos: unos **0.4 µs por llamada**. En Linux nativo `gettimeofday()` se resuelve sin entrar al kernel (mecanismo vDSO) y típicamente cuesta decenas de nanosegundos, así que el mismo experimento tardaría mucho menos (habría que subir `TAMANO` para obtener tiempos medibles).
3. **Los fallos de página son caros.** Antes de agregar `resultado[i] = 0` a la inicialización, los primeros fallos de página de `resultado` caían dentro de la medición: `suma_p` con 8 hilos tardaba 0.41–0.43 s; después del cambio, 0.13–0.15 s. Es decir, más de la mitad del tiempo medido era manejo de fallos de página, no suma. En Linux nativo el efecto también existe, pero es de esperar que sea menor.

Dos notas prácticas adicionales: en WSL (1 y 2), compilar y ejecutar dentro de `/mnt/c/...` (el disco de Windows) es más lento que dentro del directorio personal de Linux (`~`), así que conviene copiar ahí los ejemplos. Y WSL1 es la versión antigua: si tu Windows lo permite, usa WSL2 (`wsl --set-version <distro> 2`), que se comporta mucho más parecido a Linux nativo.

---

## Comparativa de entornos

| Entorno | ¿Qué kernel ejecuta los hilos? | ¿Cuántos núcleos ve el programa? | `taskset` | Costo de syscalls y fallos de página | Ruido en las mediciones |
|---|---|---|---|---|---|
| **Linux nativo** | Kernel Linux, directamente sobre el hardware | Todos los de la máquina | Funciona | Referencia (el más bajo) | Bajo |
| **WSL1** | Kernel de Windows (traducción de syscalls) | Todos los lógicos del anfitrión | Se acepta pero no tiene efecto | Alto | Medio–alto: compite con todo Windows |
| **WSL2** | Kernel Linux real dentro de una máquina virtual ligera (Hyper-V) | Por defecto todos los lógicos; configurable en `%UserProfile%\.wslconfig` | Funciona (dentro de la VM) | Cercano a nativo | Medio: los núcleos virtuales los planifica Windows |
| **VirtualBox / VMware** | Kernel Linux del invitado sobre un hipervisor | Los **vCPU asignados** a la VM (VirtualBox suele crearla con 1) | Funciona sobre los vCPU, no sobre núcleos físicos | Algo mayor que nativo | Medio–alto: depende de la carga del anfitrión |
| **Docker sobre Linux** | El mismo kernel del anfitrión (el contenedor solo aísla) | Todos, salvo que se limite con `--cpuset-cpus` | Funciona | Igual que nativo | Bajo |
| **Docker Desktop (Windows / macOS)** | Kernel Linux de una VM (en Windows, la de WSL2) | Los que tenga esa VM | Funciona dentro de la VM | Como WSL2 o como una VM | Medio |

### Qué esperar en cada ejemplo

**`t0` (orden de los hilos).** En cualquier entorno el orden `A B` será el más frecuente, porque el primer hilo se crea antes y su trabajo es mínimo. El orden `B A` aparece más en máquinas con varios núcleos y con carga; en una máquina virtual con **1 vCPU** puede no aparecer nunca en miles de ejecuciones, aunque sigue siendo posible.

**`t1` (race condition).** Con varios núcleos (nativo, WSL, Docker, VM con 2+ vCPU) los dos hilos corren realmente en paralelo y se pierden muchas actualizaciones. Con **un solo núcleo** (VM con 1 vCPU, o `taskset -c 0` en un entorno donde funcione) la race condition sigue existiendo, pero solo ocurre cuando una interrupción del timer cae justo entre la lectura y la escritura de `counter`, así que las pérdidas son mucho más raras y se necesitan valores de `loopcount` más grandes para verlas. El `loopcount` mínimo para observar el error depende del entorno.

**`suma_s` / `suma_p` (speedup).**
- El speedup máximo está acotado por los núcleos que el entorno realmente entrega. En una VM de VirtualBox con 1 vCPU, `nproc` dice 1 y el speedup será ≈ 1 sin importar el código: hay que asignar más CPUs a la VM (con la VM apagada, en *Configuración → Sistema → Procesador*).
- En Docker, la opción `--cpus=2` limita el **tiempo** de CPU pero `nproc` sigue mostrando todos los núcleos: `suma_p` creará, por ejemplo, 8 hilos que se repartirán el equivalente a 2 CPUs, y el speedup no pasará de ~2. Con `--cpuset-cpus=0,1`, en cambio, `nproc` sí reporta 2.
- En máquinas virtuales y en WSL, los núcleos virtuales se ejecutan sobre núcleos lógicos del anfitrión que pueden ser hermanos de SMT, así que la distinción entre núcleos físicos y lógicos se vuelve difusa.
- El caso limitado por memoria (con `-O2`) escalará mal en cualquier entorno: virtualizar no cambia el ancho de banda de la RAM.

---

## Recomendaciones para medir

1. Repite cada medición al menos 3 veces y reporta la mediana, no un solo valor.
2. Cierra programas pesados y, en un portátil, conéctalo al cargador y usa el plan de energía de alto rendimiento.
3. Compara siempre la versión serial y la paralela en **la misma** máquina, entorno y opciones de compilación, y con el mismo `TAMANO`.
4. Antes de compartir resultados, adjunta la ficha de tu máquina (los comandos del inicio de este documento).
5. Para saber en qué entorno estás: `uname -r` contiene `Microsoft` en WSL1 y `microsoft-standard-WSL2` en WSL2; `systemd-detect-virt` (en distribuciones con systemd) indica el hipervisor de una VM; la existencia del archivo `/.dockerenv` indica un contenedor Docker.

---

## Referencias

- Microsoft. *Comparing WSL Versions* (diferencias entre WSL1 y WSL2): https://learn.microsoft.com/windows/wsl/compare-versions
- Microsoft. *Advanced settings configuration in WSL* (`.wslconfig`): https://learn.microsoft.com/windows/wsl/wsl-config
- Docker. *Resource constraints* (`--cpus`, `--cpuset-cpus`): https://docs.docker.com/engine/containers/resource_constraints/
- `nproc(1)`, `taskset(1)`, `sched_setaffinity(2)`, `vdso(7)` — páginas de manual de Linux.
- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*. Capítulos 26 (Concurrency: An Introduction) y 27 (Thread API): https://pages.cs.wisc.edu/~remzi/OSTEP/

> [!note]
> **Nota sobre IA:** este documento fue elaborado con asistencia de un modelo de inteligencia artificial. Las cifras de la máquina de prueba provienen de mediciones reales hechas en ella; las afirmaciones sobre otros entornos son generales y conviene verificarlas en cada caso.
