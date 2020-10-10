#include <sys/smp.h>

void init_smp(struct stivale2_struct_tag_smp* smp) {
    TRACE("parsing smp information\n");
    TRACE("\t%-10s %-10s %-18s %-18s\n",
            "cpu id",
            "lapic id",
            "stack",
            "addr");

    for (uint64_t i = 0; i < smp->cpu_count; i++) {
        struct stivale2_smp_info* smp_info = &(smp->smp_info[i]);

        smp_info->target_stack = (uint64_t)kmalloc(0x1000) + HIGH_VMA;

        TRACE("\t%-10lu %-10lu %#-18lx %#-18lx\n",
                smp_info->processor_id,
                smp_info->lapic_id,
                smp_info->target_stack,
                smp_info->goto_address);
    }
}
