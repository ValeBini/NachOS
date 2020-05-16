#include "core_map.hh"
#include "machine/mmu.hh"
#include "threads/system.hh"


CoreMap::CoreMap(){
    pageMap = new Bitmap (NUM_PHYS_PAGES);

    addrSpTable = new AddressSpace * [NUM_PHYS_PAGES];
    
    vpnTable = new unsigned int [NUM_PHYS_PAGES];
}

CoreMap::~CoreMap(){
    delete pageMap;
    delete [] addrSpTable;
    delete [] vpnTable;
}

int CoreMap::FindAPage(unsigned int vpn){
    
    int phyPage = pageMap->Find();
    
    if(phyPage == -1){ 
        phyPage = PickAPage();
        addrSpTable[phyPage]->WriteSwap(vpnTable[phyPage], phyPage * PAGE_SIZE);
    }

    pagesOrder->Append(phyPage);
    addrSpTable[phyPage] = currentThread->space;
    vpnTable[phyPage] = vpn;

    return phyPage;
}

//LOGICA PARA AGARRAR UNA PAGINA
int CoreMap::PickAPage(){
    int pageIndex = pagesOrder->Pop();
    return pageIndex;
}


void CoreMap::FreePages(){
    AddressSpace *as = currentThread->space;
    for (unsigned i = 0; i < NUM_PHYS_PAGES; i++) {
        if(addrSpTable[i] == as) {
          pageMap->Clear(i); 
          pagesOrder->Remove(i);
        }
    }
}
        
unsigned int CoreMap::CountClear(){
    return pageMap->CountClear();
}