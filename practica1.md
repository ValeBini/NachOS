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
## Ejecticio 10:
## Ejecticio 11:
## Ejecticio 12:
## Ejecticio 13:
## Ejecticio 14:
## Ejecticio 15:
## Ejecticio 16: