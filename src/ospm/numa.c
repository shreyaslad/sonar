#include <ospm/numa.h>

struct acpi_srat {
    struct sdt_t sdt;
    uint32_t reserved[3];
} __attribute__((packed));

#define AFFIN_LAPIC     0
#define AFFIN_MEM       1
#define AFFIN_X2LAPIC   2

struct srat_affin_hdr {
    uint8_t type;
    uint8_t length;
} __attribute__((packed));

#define LAPIC_AFFIN_FLAGS_ENABLED (1 << 0)

struct srat_lapic_affin {
    struct srat_affin_hdr hdr;
    uint8_t prox_domain_lo;
    uint8_t apic_id;
    uint32_t flags;
    uint8_t lsapic_eid;
    uint8_t prox_domain_hi[3];
    uint32_t clk_domain;
} __attribute__((packed));

#define MEM_AFFIN_FLAGS_ENABLED (1 << 0)
#define MEM_AFFIN_FLAGS_HOTPLUG (1 << 1)
#define MEM_AFFIN_FLAGS_NV      (1 << 2)

struct srat_mem_affin {
    struct srat_affin_hdr hdr;
    uint32_t prox_domain;
    uint16_t reserved1;
    uint32_t addr_lo;
    uint32_t addr_hi;
    uint32_t length_lo;
    uint32_t length_hi;
    uint32_t reserved2;
    uint32_t flags;
    uint64_t reserved3;
} __attribute__((packed));

struct acpi_slit {
    uint64_t nlocalities;
    uint8_t matrix[];
} __attribute__((packed));

void init_numa() {
    uint64_t* sras = (uint64_t *)((uint64_t)find_sdt("SRAT", 0) + sizeof(struct acpi_srat));
    struct acpi_slit* slit = find_sdt("SLIT", 0);
    
}
