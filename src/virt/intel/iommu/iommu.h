#ifndef __INTEL__IOMMU_H__
#define __INTEL__IOMMU_H__

#include <stdint.h>
#include <trace.h>
#include <ospm/acpi/acpi.h>

struct dmar_t {
    struct sdt_t sdt;
    uint8_t h_addr_width;
    uint8_t flags;
    uint8_t reserved[10];
    uint64_t rmap_structs[];
} __attribute__((packed));

#define DMAR_TYPE_DRHD  0

struct dmar_sdt_t {
    uint16_t type;
    uint16_t len;
} __attribute((packed));

struct drhd_t {
    struct dmar_sdt_t sdt;
    uint8_t flags;
    uint8_t reserved;
    uint16_t seg_num;
    uint64_t reg_bar;
    uint64_t remap_structures[];
} __attribute__((packed));

void init_iommu();

#endif
