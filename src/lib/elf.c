#include <elf.h>

#define PT_LOAD     0x00000001
#define PT_INTERP   0x00000003
#define PT_PHDR     0x00000006

#define ABI_SYSV            0x00
#define ARCH_X86_64         0x3e
#define BITS_LE             0x01
#define ET_DYN              0x0003
#define SHT_RELA            0x00000004
#define R_X86_64_RELATIVE   0x00000008

#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
#define EI_OSABI    7

struct elf64_hdr {
    uint8_t  ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t hdr_size;
    uint16_t phdr_size;
    uint16_t ph_num;
    uint16_t shdr_size;
    uint16_t sh_num;
    uint16_t shstrndx;
} __attribute__((packed));

struct elf64_phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} __attribute__((packed));

struct elf64_shdr {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} __attribute__((packed));

struct elf64_rela {
    uint64_t r_addr;
    uint32_t r_info;
    uint32_t r_symbol;
    uint64_t r_addend;
} __attribute__((packed));

uint64_t* _module;

static void* elf_read(void* buf, size_t size) {
    void* ret = _module;
    _module = (uint64_t *)((uint64_t)_module + (uint64_t)size);

    memcpy(buf, ret, size);
    return ret;
}

int elf_load(uint64_t* module, uint64_t* entry) {
    _module = module;

    struct elf64_hdr hdr;
    elf_read(&hdr, sizeof(struct elf64_hdr));

    if (strncmp((char *)hdr.ident, "\177ELF", 4)) {
        panic("Not a valid ELF file!\n");
    }

    if (hdr.ident[EI_DATA] != BITS_LE) {
        panic("Not a little-endian ELF file!\n");
    }

    if (hdr.machine != ARCH_X86_64) {
        panic("Not a 64 bit ELF file!\n");
    }

    for (uint64_t i = 0; i < hdr.ph_num; i++) {
        struct elf64_phdr phdr;
        elf_read(&phdr, sizeof(struct elf64_phdr));

        if (phdr.p_type != PT_LOAD) {
            continue;
        }

        // TODO: reserve regions for elf loading
    }

    *entry = hdr.entry;
    return 0;
}
