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
```c
enum {
    OP_ADD      =  1,
    OP_ADDI     =  2,
    OP_ADDIU    =  3,
    OP_ADDU     =  4,
    OP_AND      =  5,
    OP_ANDI     =  6,
    OP_BEQ      =  7,
    OP_BGEZ     =  8,
    OP_BGEZAL   =  9,
    OP_BGTZ     = 10,
    OP_BLEZ     = 11,
    OP_BLTZ     = 12,
    OP_BLTZAL   = 13,
    OP_BNE      = 14,

    OP_DIV      = 16,
    OP_DIVU     = 17,
    OP_J        = 18,
    OP_JAL      = 19,
    OP_JALR     = 20,
    OP_JR       = 21,
    OP_LB       = 22,
    OP_LBU      = 23,
    OP_LH       = 24,
    OP_LHU      = 25,
    OP_LUI      = 26,
    OP_LW       = 27,
    OP_LWL      = 28,
    OP_LWR      = 29,

    OP_MFHI     = 31,
    OP_MFLO     = 32,

    OP_MTHI     = 34,
    OP_MTLO     = 35,
    OP_MULT     = 36,
    OP_MULTU    = 37,
    OP_NOR      = 38,
    OP_OR       = 39,
    OP_ORI      = 40,
    OP_RFE      = 41,
    OP_SB       = 42,
    OP_SH       = 43,
    OP_SLL      = 44,
    OP_SLLV     = 45,
    OP_SLT      = 46,
    OP_SLTI     = 47,
    OP_SLTIU    = 48,
    OP_SLTU     = 49,
    OP_SRA      = 50,
    OP_SRAV     = 51,
    OP_SRL      = 52,
    OP_SRLV     = 53,
    OP_SUB      = 54,
    OP_SUBU     = 55,
    OP_SW       = 56,
    OP_SWL      = 57,
    OP_SWR      = 58,
    OP_XOR      = 59,
    OP_XORI     = 60,
    OP_SYSCALL  = 61,

    OP_UNIMP    = 62,
    OP_RES      = 63,

    MAX_OPCODE  = 63
};
```
NachOS simula 60 instrucciones MIPS.

## Ejecticio 5:
```c
case OP_ADD:
            sum = registers[instr->rs] + registers[instr->rt];
            if (!((registers[instr->rs] ^ registers[instr->rt]) & SIGN_BIT)
                  && (registers[instr->rs] ^ sum) & SIGN_BIT) {
                RaiseException(OVERFLOW_EXCEPTION, 0);
                return;
            }
            registers[instr->rd] = sum;
            break;
```
Primero, realiza la suma de los dos registros y la guarda en una variable auxiliar. Luego:
```c
!((registers[instr->rs] ^ registers[instr->rt]) & SIGN_BIT)
```
chequea si los argumentos tienen el mismo signo. Y
```c
(registers[instr->rs] ^ sum) & SIGN_BIT
```
chequea si el resultado tiene un signo diferente.
Si ambas condiciones son verdaderas, esto indica que se produjo overflow, por lo que lanza una excepcion indicando esto.
En caso de que no haya overflow, guarda el resultado en el registro correspondiente.


## Ejecticio 6:
Primer Nivel:

En main:

1. Initialize: implementado en /code/threads/system.cc
2. DEBUG: definido en /code/lib/utility.hh
3. strcmp: implementado en string.h
4. PrintVersion: implementado en /code/threads/main.cc
5. ThreadTest: implementado en /code/threads/thread_test.cc
6. Thread::Finish: implementado en /code/threads/thread.cc


Segundo Nivel:

En Initialize:

1. ASSERT: definido en /code/lib/utility.hh
2. strcmp: implementado en string.h
3. RandomInit: implementado en /code/machine/system_dep.cc
4. atoi: implementado en stdlib.h
5. Debug::SetFlags: implementado en /code/lib/debug.cc
6. Timer::Timer: implementado en /code/machine/timer.cc
7. Thread::Thread: implementado en /code/threads/thread.cc
8. Thread::SetStatus: implementado en /code/threads/thread.cc
9. Interrupt::Enable: implementado en /code/machine/interrupt.cc
10. CallOnUserAbort: implementado en /code/machine/system_dep.cc
11. PreemptiveScheduler::PreemptiveScheduler: definido en /code/threads/preemptive.hh
12. PreemptiveScheduler::SetUp: implementado en /code/threads/preemptive.cc


En DEBUG: 

1. Debug::Print: implementado en /code/lib/debug.cc


En PrintVersion:

1. pirntf: implementado en stdio.h


En ThreadTest:

1. DEBUG: definido en /code/lib/utility.hh
2. strncpy: implementado en string.h
3. Thread::Thread: implementado en /code/threads/thread.cc
4. Thread::Fork: implementado en /code/threads/thread.cc
5. SimpleThread: implementado en /code/threads/thread_test.cc


En Thread::Finish:

1. Interrupt::SetLevel: implementado en /code/machine/interrupt.cc
2. ASSERT: definido en /code/lib/utility.hh
3. DEBUG: definido en /code/lib/utility.hh
4. Thread::GetName: implementado en /code/threads/thread.cc
5. Thread::Sleep: implementado en /code/threads/thread.cc

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
