#ifndef __ACPI__ACPI_H__
#define __ACPI__ACPI_H__

#include <stdint.h>
#include <stddef.h>
#include <io.h>
#include <str.h>
#include <mem.h>

#define ACPI_MAX_TBL_CNT 256

struct sdt_t {
    char signature[4];
    uint32_t len;
    uint8_t rev;
    uint8_t csum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
} __attribute__((packed));

struct rsdp_t {
    char signature[8];
    uint8_t csum;
    char oemid[6];
    uint8_t rev;
    uint32_t rsdt_paddr;

    /* ACPI Version 2.0 */
    uint32_t len;
    uint64_t xsdt_paddr;
    uint8_t ext_csum;
    uint8_t reserved[3];
} __attribute__((packed));

struct rsdt_t {
    struct sdt_t sdt;
    uint32_t sdt_ptr[];
} __attribute__((packed));

struct xsdt_t {
    struct sdt_t sdt;
    uint64_t sdt_ptr[];
} __attribute__((packed));

void* find_sdt(const char* signature, int idx);
void init_acpi(uint64_t rsdp_addr);

#endif
