#include <acpi/acpi.h>

#undef __MODULE__
#define __MODULE__ "acpi"

static struct rsdp_t* rsdp;
static struct rsdt_t* rsdt;
static struct xsdt_t* xsdt;

void* find_sdt(const char* signature, int idx) {
    int cnt = 0;

    if (xsdt != NULL) {
        for (size_t i = 0; i < (xsdt->sdt.len - sizeof(struct sdt_t)) / 8; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)xsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->signature, signature, 4)) {
                if (cnt++ == idx) {
                    //printf(KPRN_INFO, "acpi:\tFound \"%s\" at %X\n", signature, (size_t)ptr);
                    return (void *)ptr;
                }
            }
        }
    } else {
        for (size_t i = 0; i < (rsdt->sdt.len - sizeof(struct sdt_t)) / 4; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)rsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->signature, signature, 4)) {
                if (cnt++ == idx) {
                    //printf(KPRN_INFO, "acpi:\tFound \"%s\" at %X\n", signature, (size_t)ptr);
                    return (void *)ptr;
                }
            }
        }
    }

    ERR("\t\"%s\" not found\n", signature);
    return NULL;
}

void init_acpi(uint64_t rsdp_addr) {
    if (rsdp_addr) {
        rsdp = (struct rsdp_t *)rsdp_addr;
        goto found;
    } else {
        ERR("\tNon-compliant system!\n");
    }

    return;

found:
    if (rsdp->rev >= 2 && rsdp->xsdt_paddr) {
        //printf(KPRN_INFO, "acpi:\tXSDT at %X\n", ((size_t)rsdp->xsdt_paddr + HIGH_VMA));
        xsdt = (struct xsdt_t *)((size_t)rsdp->xsdt_paddr + HIGH_VMA);
    } else {
        //printf(KPRN_INFO, "acpi:\tRSDT at %X\n", ((size_t)rsdp->rsdt_paddr + HIGH_VMA));
        rsdt = (struct rsdt_t *)((size_t)rsdp->rsdt_paddr + HIGH_VMA);
    }

    init_madt();
}
