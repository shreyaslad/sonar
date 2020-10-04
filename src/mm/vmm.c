#include <mm/vmm.h>

struct idx_t{
    size_t pml1idx;
    size_t pml2idx;
    size_t pml3idx;
    size_t pml4idx;
};

size_t* get_pml4() {
    size_t* addr;
    asm volatile("movq %%cr3, %0;" : "=r"(addr));
    return (size_t*)addr;
}

void set_pml4(size_t PML4) {
    asm volatile("movq %0, %%cr3;" ::"r"(PML4) : "memory");
}

void invlpg(size_t* vaddr) {
    asm volatile("invlpg (%0);" ::"r"(vaddr) : "memory");
}

void tlbflush() {
    set_pml4(get_pml4());
}

void vtoidx(struct idx_t* ret, size_t* vaddr) {
    size_t addr = (size_t)vaddr;

    ret->pml4idx = (addr & ((size_t)0x1ff << 39)) >> 39;
    ret->pml3idx = (addr & ((size_t)0x1ff << 30)) >> 30;
    ret->pml2idx = (addr & ((size_t)0x1ff << 30)) >> 30;
    ret->pml1idx = (addr & ((size_t)0x1ff << 12)) >> 12;

    return;
}

void vmm_map(size_t* vaddr, size_t* paddr, size_t* pml4ptr, size_t flags) {
    struct idx_t indicies;
    vtoidx(&indicies, vaddr);

    size_t  pml3phys = NULL;
    size_t* pml3virt = NULL;
    size_t  pml2phys = NULL;
    size_t* pml2virt = NULL;
    size_t  pml1phys = NULL;
    size_t* pml1virt = NULL;

    if (pml4ptr[indicies.pml4idx] & TABLEPRESENT) {
        pml3phys = (size_t)(pml4ptr[indicies.pml4idx] & RMFLAGS);
        pml3virt = (size_t)(pml3phys + HIGH_VMA);
    } else {
        pml3phys = pmm_alloc(1);
        memset(pml3phys + HIGH_VMA, 0, PAGESIZE);
        pml3virt = (size_t)(pml3phys + HIGH_VMA);
        pml4ptr[indicies.pml4idx] = (size_t)pml3phys | flags;
    }

    if (pml3virt[indicies.pml3idx] & TABLEPRESENT) {
        pml2phys = (size_t)(pml3virt[indicies.pml3idx] & RMFLAGS);
        pml2virt = (size_t)(pml2phys + HIGH_VMA);
    } else {
        pml2phys = pmm_alloc(1);
        memset(pml2phys + HIGH_VMA, 0, PAGESIZE);
        pml2virt = (size_t)(pml2phys + HIGH_VMA);
        pml3virt[indicies.pml3idx] = (size_t)pml2phys | flags;
    }

    if (pml2virt[indicies.pml2idx] & TABLEPRESENT) {
        pml1phys = (size_t)(pml2virt[indicies.pml2idx] & RMFLAGS);
        pml1virt = (size_t)(pml1phys + HIGH_VMA);
    } else {
        pml1phys = pmm_alloc(1);
        memset(pml1phys + HIGH_VMA, 0, PAGESIZE);
        pml1virt = (size_t)(pml1phys + HIGH_VMA);
        pml2virt[indicies.pml2idx] = (size_t)pml1phys | flags;
    }

    pml1virt[indicies.pml1idx] = (size_t)paddr | flags;

    invlpg(vaddr);

    return;
}

void vmm_unmap(size_t* vaddr, size_t pages) {
    struct idx_t indicies;
    vtoidx(&indicies, vaddr);
    size_t* pml4ptr = get_pml4();
    size_t* pml3ptr = (size_t*)(pml4ptr[indicies.pml4idx] & RMFLAGS);
    size_t* pml2ptr = (size_t*)(pml3ptr[indicies.pml3idx] & RMFLAGS);
    
    for (size_t i = indicies.pml2idx; i < pages + 1; i++) {
        pml2ptr[i] = 0; // TODO: free page table if necessary
    }
    
    tlbflush();

    return;
}
