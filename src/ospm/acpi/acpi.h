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

struct generic_addr {
    uint8_t id;
    uint8_t width;
    uint8_t offset;
    uint8_t access_size;
    uint64_t address;
} __attribute__((packed));

struct fadt_t {
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t pref_pm_profile;
    uint16_t sci_int;
    uint16_t sci_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_blk_len;
    uint8_t gpe1_blk_len;
    uint8_t gpe1_base;
    uint8_t cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t reserved0;
    uint32_t flags;
    struct generic_addr reset_reg;
    uint8_t reset_val;
    uint16_t arm_boot_arch;
    uint8_t minor_ver;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    struct generic_addr x_pm1a_evt_blk;
    struct generic_addr x_pm1b_evt_blk;
    struct generic_addr x_pm1a_cnt_blk;
    struct generic_addr x_pm1b_cnt_blk;
    struct generic_addr x_pm2_cnt_blk;
    struct generic_addr x_pm_tmr_blk;
    struct generic_addr x_gpe0_blk;
    struct generic_addr x_gpe1_blk;
    struct generic_addr sleep_ctrl_reg;
    struct generic_addr sleep_status_reg;
    uint64_t hypervisor_id;
} __attribute__((packed));

void* find_sdt(const char* signature, int idx);
void init_acpi(uint64_t rsdp_addr);

#endif
