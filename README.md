## 1) 
_// mmu.hh
<p>
_MEMORY_SIZE = NUM_PHYS_PAGES * PAGE_SIZE;
PAGE_SIZE = SECTOR_SIZE;
_NUM_PHYS_PAGES = 32;
</p>
// disk.hh

SECTOR_SIZE = 128;

32*128 = 4096




2) Aumentando la cantidad de paginas fisicas o el tamaño de las paginas.


3)
// disk.cc

MAGIC_SIZE = sizeof (int);

DISK_SIZE = MAGIC_SIZE + NUM_SECTORS * SECTOR_SIZE;


// disk.hh

NUM_SECTORS = SECTORS_PER_TRACK * NUM_TRACKS;

NUM_TRACKS = 32;

SECTOR_SIZE = 128;

SECTORS_PER_TRACK = 32;


4 + (32*32) * 128 = 131072 + 4
