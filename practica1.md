# Practica 1 Sistemas Operativos II
 
## Ejercicio 1:
En machine/mmu.hh

```c
MEMORY_SIZE = NUM_PHYS_PAGES * PAGE_SIZE;
PAGE_SIZE = SECTOR_SIZE;
NUM_PHYS_PAGES = 32;
```
Y en machine/disk.hh
```c
 SECTOR_SIZE = 128;
```
Luego 
```c 
32*128 = 4096
```
## Ejercicio 2:

 Aumentando la cantidad de paginas fisicas o el tamaño de las paginas.

## Ejercicio 3:

En *machine/disk.cc*
```c
MAGIC_SIZE = sizeof (int);
DISK_SIZE = MAGIC_SIZE + NUM_SECTORS * SECTOR_SIZE;
```

Y en *machine/disk.hh*

```c
NUM_SECTORS = SECTORS_PER_TRACK * NUM_TRACKS;
NUM_TRACKS = 32;
SECTOR_SIZE = 128;
SECTORS_PER_TRACK = 32;
```
Luego
```c
4 + (32*32) * 128 = 131072 + 4
```

## Ejecticio 4:
## Ejecticio 5:
## Ejecticio 6:
## Ejecticio 7:
## Ejecticio 8:
## Ejecticio 9:
alv
## Ejecticio 10:

Las constantes estan definidas en los distintos Makefile para incluir distintos modulos.
Marcamos con un *X* en los cuales se incluyen:

|   	    |  USER_PROGRAM 	| FILESYS_NEEDED  	|  FILESYS_STUB |  NETWORK 	|
|:-:	    |:-:	            |:-:	            |:-:        	|:-:    	|
|   filesys	|  *X* 	            |   *X*	            |           	|          	|
|   network	|  *X*	            |   *X*	            |             	|   *X*   	|
|   userprog|  *X*	            |   *X*          	|   *X*       	|   	    |
|   vmem	|  *X*	            |   *X* 	        |      *X*	    |          	|
## Ejecticio 11:
List es una implementacion de lista enlazada con prioridad, lo elementos de List son del tipo ListElement que se encarga de un solo item de la lista.

SynchList es una lista sincronizada, es decir una List que teine las siguientes restricciones:

    1- Un thread que intenta remover un item de List va a esperar hasta que la lista tenga un elemento en ella.
    2- Solo un thread a la vez puede acceder a la estructura de lista.
## Ejecticio 12:
Podemos encontrar definida la funcion *main* en:


- code/bin/coff2flat.c:

- code/bin/coff2noff.c:

- code/bin/disasm.c:

- code/bin/main.c:

- code/bin/out.c:

- code/bin/readnoff.c:

- code/bin/fuse/nachosfuse.c:

- code/userland/filetest.c:

- code/userland/halt.c:

- code/userland/matmult.c:

- code/userland/shell.c:

- code/userland/sort.c:

- code/userland/tiny_shell.c:
  
Podemos ver que el *main* del ejecutable *nachos* de *userprog* esta definida en:
*code/threads/main.cc*.

Al inspeccionar *code/userprog/Makefile.depends* vemos:
 ```{c}
  main.o: ../threads/main.cc 
  ```

REVISAR ESTA CONCLUSION !!!!!!!!!!!1

## Ejecticio 13:
Nachos soporta las siguentes lineas de comandos: 

 * `-d`: Imprime algunos mensajes de debugging.

 * `-p`: Habilita la multitarea preventiva para los threads del kernel.

 * `-rs`: Hace que ocurran `Yield` en lugares aleatorios.

 * `-z`: Imprime informacion sobre version y copyrights.

 * `-s`: Hace que los programas de usuarios se ejecuten en modo paso-simple.

 * `-x`: Ejecuta programa de usuario.

 * `-tc`: Testea la consola.

 * `-f`: Formatea el disco fisico.

 * `-cp`: Copia archivo de UNIX a Nachos.

 * `-pr`: Imprime un archivo de Nachos a la salida estandar.

 * `-rm`: ELimina un archivo de Nachos del sistema.

 * `-ls`: Lista el contenido de el directorio de Nachos.

 * `-D`: Imprime el contenido de todo el sistema de archivos.

 * `-tf`: Testea la performance del sistema de archivos de Nachos.

 * `-n`: Establece la fiabilidad de la red.

 * `-id`: Establece el host id de la maquina (necesaria para la red).

 * `-tn`: Ejecuta un test simple sobre el software de red de Nachos.

La opcion `-rs` produce que ocurran `Yield` en lugares aleatorios.

## Ejecticio 14:
## Ejecticio 15:
## Ejecticio 16: