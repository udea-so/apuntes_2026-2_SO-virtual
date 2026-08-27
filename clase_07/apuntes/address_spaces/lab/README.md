# Laboratorio

El programa `use-memory` ya ha sido creado. Se puede revisar el [código fuente](use-memory.c) si así se desea. El programa asigna memoria basándose en la entrada proporcionada por el usuario. No obstante, no se recomienda llamar a este programa directamente. En su lugar, se debe utilizar la [utilidad](https://www.gnu.org/software/time/) `time` de GNU para monitorear el uso de memoria de `use-memory`.

El siguiente comando invoca un script que llama a la utilidad `time` y al programa `use-memory`, utilizando `200` como parámetro:

```
bash monitor-memory.sh 200
```

Se le solicitará ejecutar este script con diferentes valores de entrada y responder las preguntas que se presentan a continuación.

### Pregunta de laboratorio 1

Ejecute el siguiente comando en la terminal:

```
bash monitor-memory.sh 16
Memory Used: 17664 kilobytes
```

¿Qué sucede cuando se pasa al script un número menor a 200?

* [ ] Se asigna menos memoria
* [ ] Se asigna más memoria
* [ ] Se asigna la misma cantidad de memoria

### Pregunta de laboratorio 2

Ejecute el siguiente comando en la terminal:

```
bash monitor-memory.sh 400
Memory Used: 410880 kilobytes
```

¿Qué sucede cuando se pasa al script un número mayor a 200?

* [ ] Se asigna menos memoria
* [ ] Se asigna más memoria
* [ ] Se asigna la misma cantidad de memoria

### Pregunta de laboratorio 3

¿Qué representa el número (`200`, `16`, `400`) pasado al script monitor-memory?

* [ ] La cantidad de memoria a asignar en bytes.
* [ ] La cantidad de memoria a asignar en kilobytes.
* [ ] La cantidad de memoria a asignar en gigabytes.
* [ ] La cantidad de memoria a asignar en megabytes.
