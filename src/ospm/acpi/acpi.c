#include <ospm/acpi/acpi.h>

#undef __MODULE__
#define __MODULE__ "acpi"

static struct rsdp_t* rsdp;
static struct rsdt_t* rsdt;
static struct xsdt_t* xsdt;

void* find_sdt(const char* signature, int idx) {
    int cnt = 0;

    if (xsdt) {
        for (size_t i = 0; i < (xsdt->sdt.len - sizeof(struct sdt_t)) / 8; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)xsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->signature, signature, 4)) {
                if (cnt++ == idx) {
                    return (void *)ptr;
                }
            }
        }
    } else {
        for (size_t i = 0; i < (rsdt->sdt.len - sizeof(struct sdt_t)) / 4; i++) {
            struct sdt_t* ptr = (struct sdt_t *)((size_t)rsdt->sdt_ptr[i] + HIGH_VMA);

            if (!strncmp(ptr->signature, signature, 4)) {
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

    TRACE("REV: %d\n", rsdp->rev);

    TRACE("Found the \"%s\"\n", (rsdp->rev > 0) ? "XSDT" : "RSDT");

    /*TRACE("Detected %d ACPI %s tables\n", n_entries, (rsdp->rev > 1) ? "2": "1");
    TRACE("%-s %-s %-6s %-16s\n", "Signature", "Rev", "OEMID", "Address");

    for (size_t i = 0; i < n_entries; i++) {
        uint64_t table_paddr = (rsdp->rev > 0) ? xsdt->sdt_ptr[i] : rsdt->sdt_ptr[i];
        vmm_map(table_paddr + HIGH_VMA, table_paddr, get_pml4(), TABLEPRESENT);

        TRACE("%-s %-d %-s %-#lx\n",
                sdt.signature,
                sdt.rev,
                sdt.oem_id,
                table_paddr
        );
    }*/
}
