#include <ospm/acpi/acpi.h>

static struct rsdp_t* rsdp;
static struct rsdt_t* rsdt;
static struct xsdt_t* xsdt;

void* find_sdt(const char* signature, int idx) {
    int cnt = 0;

    if (xsdt) {
        for (size_t i = 0; i < (xsdt->sdt.len - sizeof(struct sdt_t)) / 8; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)xsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->sig, signature, 4)) {
                if (cnt++ == idx) {
                    return (void *)ptr;
                }
            }
        }
    } else {
        for (size_t i = 0; i < (rsdt->sdt.len - sizeof(struct sdt_t)) / 4; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)rsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->sig, signature, 4)) {
                if (cnt++ == idx) {
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

found: ;
    struct sdt_t sdt;

    if (rsdp->rev > 0 && rsdp->xsdt_paddr) {
        xsdt = (struct xsdt_t *)((size_t)rsdp->xsdt_paddr + HIGH_VMA);
        sdt = xsdt->sdt;
    } else {
        rsdt = (struct rsdt_t *)((size_t)rsdp->rsdt_paddr + HIGH_VMA);
        sdt = rsdt->sdt;
    }
    
    LOG("Found the \"%s\" at %#lx\n",
            (rsdp->rev > 0) ? "XSDT" : "RSDT",
            (rsdp->rev > 0) ? 
                rsdp->rsdt_paddr + HIGH_VMA : rsdp->xsdt_paddr + HIGH_VMA);

    size_t n_entries = (sdt.len - sizeof(struct sdt_t)) / ((rsdp->rev > 0) ? 8 : 4);

    LOG("Detected %d ACPI rev %s tables:\n", n_entries, (rsdp->rev > 1) ? "2": "1");
    LOG("\t %-8s %-s %-6s %-11s\n", "Signature", "Rev", "OEMID", "Address");

    for (size_t i = 0; i < n_entries; i++) {
        uint64_t table_paddr = (rsdp->rev > 0) ? xsdt->sdt_ptr[i] : rsdt->sdt_ptr[i];
        vmm_map(table_paddr + HIGH_VMA, table_paddr, get_pml4(), MAP_TABLEPRESENT);

        struct sdt_t* c = (struct sdt_t *)table_paddr;

        LOG("-\t%-c%c%c%c %6d %3c%c%c%c%c%c %#0lx\n",
                c->sig[0], c->sig[1], c->sig[2], c->sig[3],
                c->rev,
                c->oid[0], c->oid[1], c->oid[2], c->oid[3], c->oid[4], c->oid[5],
                (uint64_t)table_paddr + HIGH_VMA
        );
    }
}
