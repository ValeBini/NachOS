#ifndef NACHOS_VMEM_COREMAP_HH
#define NACHOS_VMEM_COREMAP_HH

#include "lib/bitmap.hh"
#include "lib/list.hh"
#include "threads/thread.hh"

class CoreMap{
    public:
        CoreMap();

        ~CoreMap();

        int FindAPage(unsigned int vpn);

        void FreePages(AddressSpace *as);

        unsigned int CountClear();
        
    private:   
        int PickAPage();
        
        Bitmap * pageMap;
        
        AddressSpace **addrSpTable;

        unsigned int * vpnTable;

        List<int> * pagesOrder;
};

#endif