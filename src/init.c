#include <stdint.h>
#include <io.h>
#include <mem.h>
#include <log.h>
#include <protos/stivale2.h>
#include <drivers/display/serial.h>
#include <drivers/display/vesa.h>
#include <ospm/ospm.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/cpu.h>

#undef __MODULE__
#define __MODULE__ "init"

__attribute__((noreturn))
void sonar_main(struct stivale2_struct* info) {
    struct stivale2_struct_tag_memmap*      memmap_tag;
    struct stivale2_struct_tag_framebuffer* fb_tag;
    struct stivale2_struct_tag_rsdp*        rsdp_tag;
    struct stivale2_struct_tag_smp*         smp_tag;
    struct stivale2_struct_tag_firmware*    firmware_tag;

    for (struct stivale2_tag* cur = (struct stivale2_tag *)(info->tags);
         cur;
         cur = (struct stivale2_tag *)(cur->next)) {
        switch (cur->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
                memmap_tag = (struct stivale2_struct_tag_memmap *)cur;
                break;
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
                fb_tag = (struct stivale2_struct_tag_framebuffer *)cur;
                break;
            case STIVALE2_STRUCT_TAG_RSDP_ID:
                rsdp_tag = (struct stivale2_struct_tag_rsdp *)cur;
                break;
            case STIVALE2_STRUCT_TAG_SMP_ID:
                smp_tag = (struct stivale2_struct_tag_smp *)cur;
                break;
            case STIVALE2_STRUCT_TAG_FIRMWARE_ID:
                firmware_tag = (struct stivale2_struct_tag_firmware *)cur;
                break;
        }
    }
    
    init_gdt();
    init_idt();

    init_serial();
    init_vesa(fb_tag);

    init_mem(memmap_tag);

    init_cpu();
    init_ospm(rsdp_tag, smp_tag);

    printf("\n");
    LOG("Sonar\n");
    LOG("-\tBuilt on %s at %s\n", __DATE__, __TIME__);
    LOG("-\tBooted with %s %s (%s)\n",
            info->bootloader_brand,
            info->bootloader_version,
            firmware_tag->flags & 1 ? "BIOS" : "UEFI");

    asm {
        cli
        hlt
    }
}

static uint8_t stack[STACK_SIZE];

struct stivale2_header_tag_smp stivale2_smp_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = 0
    },

    .flags = 0
};

struct stivale2_tag stivale2_mtrr_tag = {
    .identifier = STIVALE2_HEADER_TAG_FB_MTRR_ID,
    .next = (uintptr_t)&stivale2_smp_hdr_tag
};

struct stivale2_header_tag_framebuffer stivale2_fb_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&stivale2_mtrr_tag
    },

    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header stivale2_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + STACK_SIZE,
    .flags = 0,
    .tags = (uintptr_t)&stivale2_fb_tag
};
