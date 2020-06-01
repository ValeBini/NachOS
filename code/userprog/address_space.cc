/// Routines to manage address spaces (memory for executing user programs).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "address_space.hh"
#include "threads/system.hh"

#include <stdio.h>
#include <string.h>
#include <string>


/// First, set up the translation from program memory to physical memory.
/// For now, this is really simple (1:1), since we are only uniprogramming,
/// and we have a single unsegmented page table.
AddressSpace::AddressSpace(OpenFile *exe_file)
{

    ASSERT(exe_file != nullptr);

    exe = new Executable (exe_file);

    ASSERT(exe->CheckMagic());

    // How big is address spa

    unsigned size = exe->GetSize() + USER_STACK_SIZE;
      // We need to increase the size to leave room for the stack.
    numPages = DivRoundUp(size, PAGE_SIZE);
    size = numPages * PAGE_SIZE;
 
#ifdef VMEM
    fileName = new char[100];
    char * id = new char[100];

    //itoa(currentThread->threadId,id,10);
    strncpy(id, std::to_string((long int)(this)).c_str(), 100);
    strcpy(fileName,"SWAP.");
    strcat(fileName,id);
    fileSystem->Create(fileName,size);
    DEBUG('p', "--------------------\n");
    DEBUG('p', "Created swap file: %s\n", fileName);
    DEBUG('p', "--------------------\n");
    swap_file = fileSystem->Open(fileName);
#else
    ASSERT(numPages <= pageMap->CountClear());
#endif

      // Check we are not trying to run anything too big -- at least until we
      // have virtual memory.

    DEBUG('a', "Initializing address space, num pages %u, size %u\n",
          numPages, size);

    // First, set up the translation.

    pageTable = new TranslationEntry[numPages];
    for (unsigned i = 0; i < numPages; i++) {
        pageTable[i].virtualPage  = i;
          // For now, virtual page number = physical page number.
#ifdef VMEM
        pageTable[i].physicalPage = -1;
        pageTable[i].valid        = false;
#else
        int newPage = pageMap->Find();
        ASSERT(newPage != -1);
        pageTable[i].physicalPage = (unsigned int)newPage;
        pageTable[i].valid        = true;
#endif
        pageTable[i].use          = false;
        pageTable[i].dirty        = false;
        pageTable[i].readOnly     = false;
          // If the code segment was entirely on a separate page, we could
          // set its pages to be read-only.
    }

#ifndef VMEM

    char *mainMemory = machine->GetMMU()->mainMemory;
    

    for (unsigned i = 0; i < numPages; i++) {
      memset(&mainMemory[pageTable[i].physicalPage*PAGE_SIZE],0,PAGE_SIZE);
    }

    uint32_t codeSize = exe->GetCodeSize();
    uint32_t initDataSize = exe->GetInitDataSize();

    if (codeSize > 0) {
        uint32_t virtualAddr = exe->GetCodeAddr();
        DEBUG('z', "Initializing code segment, at 0x%X, size %u\n",
              virtualAddr, codeSize);
        uint32_t virtualPage    = virtualAddr/PAGE_SIZE;
        uint32_t offset        = virtualAddr%PAGE_SIZE;
        uint32_t currentCodeSize = codeSize;
        uint32_t sizeToRead = minn(PAGE_SIZE-offset,currentCodeSize);

        exe->ReadCodeBlock(&mainMemory[(pageTable[virtualPage].physicalPage*PAGE_SIZE)+offset], sizeToRead, 0);
        currentCodeSize -= sizeToRead;
        virtualPage++;
        for(;currentCodeSize > 0; virtualPage++){
          sizeToRead = minn(PAGE_SIZE,currentCodeSize);
          exe->ReadCodeBlock(&mainMemory[pageTable[virtualPage].physicalPage*PAGE_SIZE], sizeToRead, codeSize-currentCodeSize);
          currentCodeSize -= sizeToRead;
        }
    }
    if (initDataSize > 0) {
        uint32_t virtualAddr = exe->GetInitDataAddr();
        DEBUG('z', "Initializing data segment, at 0x%X, size %u\n",
              virtualAddr, initDataSize);

        uint32_t virtualPage    = virtualAddr/PAGE_SIZE;
        uint32_t offset        = virtualAddr%PAGE_SIZE;
        uint32_t currentIDataSize = initDataSize;
        uint32_t sizeToRead = minn(PAGE_SIZE-offset, currentIDataSize);

        exe->ReadDataBlock(&mainMemory[(pageTable[virtualPage].physicalPage*PAGE_SIZE)+offset], sizeToRead, 0);
        currentIDataSize -= sizeToRead;
        virtualPage++;
        for(;currentIDataSize > 0; virtualPage++){
          sizeToRead = minn(PAGE_SIZE,currentIDataSize);
          exe->ReadDataBlock(&mainMemory[pageTable[virtualPage].physicalPage*PAGE_SIZE], sizeToRead, initDataSize-currentIDataSize);
          currentIDataSize -= sizeToRead;
        }
    }
#endif
    
}

#ifdef VMEM

uint32_t  LoadCode(uint32_t physicalAddr, Executable *exe, char* mainMemory, uint32_t vpn, uint32_t vaddr, uint32_t init, uint32_t size){
    
    int leftToRead;

    if(vaddr  > size + init)
      return 0;
    
    leftToRead = size + init - vaddr;
    
    int sizeToRead = minn((int)PAGE_SIZE, maxx(0, leftToRead));
    if(sizeToRead <= 0) return sizeToRead;
    exe->ReadCodeBlock(&mainMemory[physicalAddr], sizeToRead, vaddr - init);
    return sizeToRead;
}

uint32_t  LoadData(uint32_t physicalAddr,Executable *exe, char* mainMemory, uint32_t vpn, uint32_t vaddr, uint32_t init, uint32_t size, uint32_t alreadyRead){
    if (size == 0) return alreadyRead;

    int leftToRead;

    if(vaddr + alreadyRead > size + init)
      return alreadyRead;
    
    leftToRead = size + init - (vaddr + alreadyRead);

    int sizeToRead = minn((int)(PAGE_SIZE - alreadyRead), leftToRead);
    if(sizeToRead <= 0) return alreadyRead;
    exe->ReadDataBlock(&mainMemory[physicalAddr + alreadyRead], sizeToRead, (vaddr + alreadyRead) - init);
    return alreadyRead + sizeToRead;
}

uint32_t LoadStack(uint32_t physicalAddr, char* mainMemory, uint32_t vpn, uint32_t alreadyRead){
  memset(&mainMemory[physicalAddr + alreadyRead],0, PAGE_SIZE - alreadyRead);
  return 0;
}


TranslationEntry AddressSpace::LoadPage(int vpn){

  
  uint32_t codeSize = exe->GetCodeSize();
  uint32_t dataSize = exe->GetInitDataSize();
  uint32_t initAddrData = exe->GetInitDataAddr();
  uint32_t initAddrCode = exe->GetCodeAddr();
  uint32_t vaddr = vpn * PAGE_SIZE;
  char *mainMemory = machine->GetMMU()->mainMemory;

  int newPage = coreMap->FindAPage(vpn, this);
  //ASSERT(newPage >= 0);
  pageTable[vpn].physicalPage = newPage;  
  int physicalAddr = pageTable[vpn].physicalPage*PAGE_SIZE;
    
  if(!pageTable[vpn].inSwap){
    // Binary
    DEBUG('p',"Loading from binary page %u.\n", vpn);
    int alreadyRead = LoadCode(physicalAddr, exe, mainMemory, vpn, vaddr, initAddrCode, codeSize);
    alreadyRead = LoadData(physicalAddr, exe, mainMemory, vpn, vaddr, initAddrData, dataSize, alreadyRead);
    LoadStack(physicalAddr, mainMemory, vpn, alreadyRead);

  } else {
    //ASSERT(false);
    //Swap
    DEBUG('p',"Loading from swap file page %u.\n", vpn);
    int n = ReadSwap(vpn, physicalAddr);
    ASSERT(n == PAGE_SIZE);
  } 
  
  pageTable[vpn].valid = true;
  pageTable[vpn].readOnly = false;
  pageTable[vpn].dirty = false;
  pageTable[vpn].use = false;

  return pageTable[vpn];
}

int AddressSpace::ReadSwap(int vpn, uint32_t physicalAddr){
  char *mainMemory = machine->GetMMU()->mainMemory;
  DEBUG('p',"Reading from swap file. From: %u. To: %u. Virtual Page: %u.\n",vpn * PAGE_SIZE, physicalAddr,vpn);
  return swap_file->ReadAt(&mainMemory[physicalAddr], PAGE_SIZE, vpn * PAGE_SIZE);
}

int AddressSpace::WriteSwap(int vpn, uint32_t physicalAddr){
  char *mainMemory = machine->GetMMU()->mainMemory;
  DEBUG('p',"Writing at swap file. Position: %u. Virtual Page: %u.\n",vpn * PAGE_SIZE,vpn);
  pageTable[vpn].valid = false;
  int n;
  //ASSERT(pageTable[vpn].dirty==false);
  if(pageTable[vpn].dirty==false) {
    n = PAGE_SIZE;
  }else{
    n = swap_file->WriteAt(&mainMemory[physicalAddr],PAGE_SIZE, vpn * PAGE_SIZE);
    pageTable[vpn].inSwap = true;
  }
  memset(&mainMemory[physicalAddr], 0, PAGE_SIZE);
  for(unsigned i=0; i<TLB_SIZE; i++){
    if(machine->GetMMU()->tlb[i].virtualPage == (unsigned)vpn)
      machine->GetMMU()->tlb[i].valid = false;
  }
  return n;
}
#endif

/// Deallocate an address space.
///
/// Nothing for now!
AddressSpace::~AddressSpace()
{
  #ifdef VMEM
    coreMap->FreePages(this);
    delete swap_file;
    fileSystem->Remove(fileName);
  #else
    for (unsigned i = 0; i < numPages; i++) {
        if(pageTable[i].physicalPage!=-1)
          pageMap->Clear(pageTable[i].physicalPage);
    }
  #endif
    
    delete [] pageTable;
}

/// Set the initial values for the user-level register set.
///
/// We write these directly into the “machine” registers, so that we can
/// immediately jump to user code.  Note that these will be saved/restored
/// into the `currentThread->userRegisters` when this thread is context
/// switched out.
void
AddressSpace::InitRegisters()
{
    for (unsigned i = 0; i < NUM_TOTAL_REGS; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of `Start`.
    machine->WriteRegister(PC_REG, 0);

    // Need to also tell MIPS where next instruction is, because of branch
    // delay possibility.
    machine->WriteRegister(NEXT_PC_REG, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we do not
    // accidentally reference off the end!
    machine->WriteRegister(STACK_REG, numPages * PAGE_SIZE - 16);
    DEBUG('a', "Initializing stack register to %u\n",
          numPages * PAGE_SIZE - 16);
}

/// On a context switch, save any machine state, specific to this address
/// space, that needs saving.
///
/// For now, nothing!
void
AddressSpace::SaveState()
{
    #ifdef VMEM
    TranslationEntry *tlb = machine -> GetMMU() -> tlb;
    for(unsigned i = 0; i < TLB_SIZE; i++){
          if(tlb[i].valid){
              unsigned pageIndex = tlb[i].virtualPage;
              pageTable[pageIndex].use = tlb[i].use;
              pageTable[pageIndex].dirty = tlb[i].dirty;
          }
      }
    #endif
}

/// On a context switch, restore the machine state so that this address space
/// can run.
///
/// For now, tell the machine where 0 find the page table.
void
AddressSpace::RestoreState()
{
    #ifdef VMEM
    for(int i = 0 ; i < (int)TLB_SIZE; i++){
      machine->GetMMU()->tlb[i].valid = false;
    }    
    #else 
    machine->GetMMU()->pageTable     = pageTable;
    machine->GetMMU()->pageTableSize = numPages;
    #endif
}

