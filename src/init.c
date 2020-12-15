#include <mem.h>
#include <protos/stivale2.h>
#include <sys/interrupts.h>
#include <drivers/vesa.h>
#include <virt/intel/vmx/vmx.h>
#include <ospm/ospm.h>

__attribute__((noreturn))
void sonar_main(struct stivale2_struct* info) {
    struct stivale2_struct_tag_memmap*      memmap;
    struct stivale2_struct_tag_framebuffer* fb;
    struct stivale2_struct_tag_rsdp*        rsdp;
    struct stivale2_struct_tag_smp*         smp;

    for (struct stivale2_tag* cur = (struct stivale2_tag *)info->tags; cur; cur = cur->next) {
        switch (cur->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
                memmap = (struct stivale2_struct_tag_memmap *)cur;
                break;
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
                fb = (struct stivale2_struct_tag_framebuffer *)cur;
                break;
            case STIVALE2_STRUCT_TAG_RSDP_ID:
                rsdp = (struct stivale2_struct_tag_rsdp *)cur;
                break;
            case STIVALE2_STRUCT_TAG_SMP_ID:
                smp = (struct stivale2_struct_tag_smp *)cur;
                break;
        }
    }
    
    init_isrs();
    init_vesa(fb);
    init_mem(memmap);

    init_ospm();
    init_vmx();

    asm volatile("cli\n\t"
                 "hlt\n\t");
}
