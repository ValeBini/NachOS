#include "core_map.hh"
#include "machine/mmu.hh"
#include "threads/system.hh"


CoreMap::CoreMap(){
    pageMap = new Bitmap (NUM_PHYS_PAGES);

    addrSpTable = new AddressSpace * [NUM_PHYS_PAGES];
    
    vpnTable = new unsigned int [NUM_PHYS_PAGES];

    pagesOrder = new List<int>;
}

CoreMap::~CoreMap(){
    delete pageMap;
    delete [] addrSpTable;
    delete [] vpnTable;
    delete pagesOrder;
}

int CoreMap::FindAPage(unsigned int vpn, AddressSpace * space){
    

    DEBUG('p',"PageMap finding a page\n");
    int phyPage = pageMap->Find();

    if(phyPage == -1){ 
        //ASSERT(false);
        DEBUG('p',"PageMap did not find a page, picking a page to swap\n");
        phyPage = PickAPage();
        DEBUG('p',"PickAPage choose the %d page\n",phyPage);
   
        int n = addrSpTable[phyPage]->WriteSwap(vpnTable[phyPage], phyPage * PAGE_SIZE);
        ASSERT(n == PAGE_SIZE);
        
    }else{
        DEBUG('p',"PageMap found the %d page free\n",phyPage);
    }

    DEBUG('l',"Appending %d\n",phyPage);

#ifndef LRU
    pagesOrder->Append(phyPage);
#endif
    addrSpTable[phyPage] = space;
    vpnTable[phyPage] = vpn;

    return phyPage;
}

//LOGICA PARA AGARRAR UNA PAGINA
int CoreMap::PickAPage(){
    // return 5;
    int pageIndex = pagesOrder->Pop();
    DEBUG('l',"Popping %d\n",pageIndex);
    return pageIndex;
}


void CoreMap::FreePages(AddressSpace *as){
    // debug('M',"%s\n",currentThread->GetName(),currentThread->)
    for (unsigned i = 0; i < NUM_PHYS_PAGES; i++) {
        if(addrSpTable[i] == as) {
          pageMap->Clear(i); 
          pagesOrder->Remove(i);
          addrSpTable[i] = nullptr;
        }
    }
}
        
unsigned int CoreMap::CountClear(){
    return pageMap->CountClear();
}

#ifdef LRU

void CoreMap::usePage(int page){
    pagesOrder->Remove(page);
    pagesOrder->Append(page);
}

#endif