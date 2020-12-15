#include <ospm/ospm.h>

void init_ospm(struct stivale2_struct_tag_rsdp* rsdp_tag,
               struct stivale2_struct_tag_smp* smp_tag) {

    uint64_t rsdp_addr = rsdp_tag->rsdp + HIGH_VMA;
    
    init_acpi(rsdp_addr);
    init_madt();
    init_apic();

    struct rsdp_t* rsdp = (struct rsdp_t *)rsdp_addr;

    lai_set_acpi_revision(rsdp->rev);
    lai_create_namespace();
    lai_enable_acpi(1);

    //init_smp(smp);
}
