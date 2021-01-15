#include <mem.h>
#include <protos/stivale2.h>
#include <sys/interrupts.h>
#include <virt/intel/vmx/vmx.h>
#include <ospm/ospm.h>

#define __MODULE__ "init"

__attribute__((noreturn))
void sonar_main(struct stivale2_struct* info) {
    struct stivale2_struct_tag_memmap*      memmap;
    struct stivale2_struct_tag_framebuffer* fb;
    struct stivale2_struct_tag_rsdp*        rsdp;
    struct stivale2_struct_tag_smp*         smp;
    struct stivale2_struct_tag_firmware*    firmware;

    for (struct stivale2_tag* cur = (struct stivale2_tag *)info->tags; cur;
            cur = (struct stivale2_tag *)(cur->next)) {
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
            case STIVALE2_STRUCT_TAG_FIRMWARE_ID:
                firmware = (struct stivale2_struct_tag_firmware *)cur;
                break;
        }
    }
    
    init_isrs();

    enable_early_log(fb);
    TRACE("Booted on \"%s\" by \"%s\" (version \"%s\")\n",
            firmware->flags & 1 ? "BIOS" : "UEFI", 
            info->bootloader_brand,
            info->bootloader_version);

    init_mem(memmap);
    init_ospm(rsdp, smp);
    init_vmx();
    
    asm volatile("cli\n\t"
                 "hlt\n\t");
}
