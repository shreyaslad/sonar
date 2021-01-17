#include <ospm/smp.h>

void init_smp(struct stivale2_struct_tag_smp* smp) {
    TRACE("Found %d CPUs, using the %s:\n",
            smp->cpu_count,
            smp->flags & 1 ? "x2APIC" : "xAPIC");

    TRACE("\t %-s %-s %-18s %-18s %-s\n",
            "CPU ID",
            "LAPIC ID",
            "Stack",
            "Address",
            "Type");

    for (uint64_t i = 0; i < smp->cpu_count; i++) {
        struct stivale2_smp_info smp_info = smp->smp_info[i];

        TRACE("-\t%-6lu %-8lu %-#18lx %-#18lx %-4s\n",
                smp_info.processor_id,
                smp_info.lapic_id,
                smp_info.target_stack,
                smp_info.goto_address,
                (smp->bsp_lapic_id == smp_info.processor_id)
                        ? "BSP" : "AP");
    }
}
