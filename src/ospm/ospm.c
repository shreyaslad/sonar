#include <ospm/ospm.h>

void init_ospm(struct stivale2_struct_tag_rsdp* rsdp_tag,
               struct stivale2_struct_tag_smp* smp_tag) {

    uint64_t rsdp_addr = rsdp_tag->rsdp + HIGH_VMA;
    
    init_acpi(rsdp_addr);
    init_madt();
    init_apic();

    init_pci();

    //init_smp(smp);
}
