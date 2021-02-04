#ifndef __STORAGE__AHCI_H__
#define __STORAGE__AHCI_H__

#include <stdint.h>
#include <log.h>
#include <ospm/pci.h>

#undef __MODULE__
#define __MODULE__ "ahci"

#define BYTES_PER_SECT      512
#define SECTORS_PER_BLOCK   128
#define BYTES_PER_BLOCK     (SECTORS_PER_BLOCK * BYTES_PER_SECT)

#define MAX_CACHED_BLOCKS   8192
#define MAX_AHCI_DEVICES    32

#define CACHE_NOT_READY 0
#define CACHE_READY     1
#define CACHE_DIRTY     2

#define SATA_SIG_ATA    0x00000101 /* SATA drive */
#define SATA_SIG_ATAPI  0xeb140101 /* ATAPI drive */
#define SATA_SIG_SEMB   0xc33c0101 /* Enclosure management bridge */
#define SATA_SIG_PM     0x96690101 /* Port multiplier */

#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4

#define AHCI_CLASS      0x01
#define AHCI_SUBCLASS   0x06
#define AHCI_PROG_IF    0x01

#define ATA_DEV_BUSY            0x80
#define ATA_DEV_DRQ             0x08
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_DMA_EXT   0x35

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

enum fis_type_t {
    FIS_TYPE_REG_H2D    = 0x27,
    FIS_TYPE_REG_D2H    = 0x34,
    FIS_TYPE_DMA_ACT    = 0x39,
    FIS_TYPE_DMA_SETUP  = 0x41,
    FIS_TYPE_DATA       = 0x46,
    FIS_TYPE_BIST       = 0x58,
    FIS_TYPE_PIO_SETUP  = 0x5F,
    FIS_TYPE_DEV_BITS   = 0xA1,
};

struct fis_reg_h2d_t {
    uint8_t fis_type;

    uint8_t pmport: 4;
    uint8_t rsv0: 3;
    uint8_t c: 1;

    uint8_t command;
    uint8_t featurel;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureh;

    uint8_t countl;
    uint8_t counth;
    uint8_t iccc;
    uint8_t ctrl;

    uint8_t rsv1[4];
} __attribute__((packed));

struct fis_reg_d2h_t {
    uint8_t fis_type;
    
    uint8_t pmport: 4;
    uint8_t rsv0: 2;
    uint8_t i: 1;
    uint8_t rsv1: 1;

    uint8_t status;
    uint8_t error;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t rsv2;

    uint8_t countl;
    uint8_t counth;
    uint8_t rsv3[2];

    uint8_t rsv4[4];
} __attribute__((packed));

struct fis_data_t {
    uint8_t fis_type;

    uint8_t pmport: 4;
    uint8_t rsv0: 4;

    uint8_t rsv1[2];

    uint32_t data[1];
} __attribute__((packed));

struct fis_pio_setup_t {
    uint8_t fis_type;

    uint8_t pmport: 4;
    uint8_t rsv0: 1;
    uint8_t d: 1;
    uint8_t i: 1;
    uint8_t rsv1: 1;

    uint8_t status;
    uint8_t error;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t rsv2;

    uint8_t countl;
    uint8_t counth;
    uint8_t rsv3;
    uint8_t e_status;

    uint16_t tc;
    uint8_t rsv4[2];
} __attribute__((packed));

struct fis_dma_setup_t {
    uint8_t fis_type;
    
    uint8_t pmport: 4;
    uint8_t rsv0: 1;
    uint8_t d: 1;
    uint8_t i: 1;
    uint8_t a: 1;
    
    uint8_t rsv1[2];

    uint64_t dma_buffer_id;

    uint32_t rsv2;

    uint32_t dma_buffer_offset;

    uint32_t transfer_count;
    
    uint32_t rsv3;
} __attribute__((packed));

struct hba_fis_t {
    struct fis_dma_setup_t dsfis;
    uint8_t pad0[4];

    struct fis_pio_setup_t psfis;
    uint8_t pad1[12];

    struct fis_reg_d2h_t rfis;
    uint8_t pad2[4];

    uint8_t subfis[8];

    uint8_t ufis[64];
    uint8_t rsv[0x60];
} __attribute__((packed));

struct hba_cmd_hdr_t {
    uint8_t cfl: 5;
    uint8_t a: 1;
    uint8_t w: 1;
    uint8_t p: 1;
    uint8_t r: 1;
    uint8_t b: 1;
    uint8_t c: 1;
    uint8_t rsv0: 1;
    uint8_t pmp: 4;
    uint16_t prdtl;
    volatile uint32_t prdbc;
    uint32_t ctba;
    uint32_t ctbau;
    uint32_t rsv1[4];
} __attribute__((packed));

struct hba_prdtl_t {
    uint32_t dba;
    uint32_t dbau;
    uint32_t rsv0;
    uint32_t dbc: 22;
    uint32_t rsv1: 9;
    uint32_t i: 1;
} __attribute__((packed));

struct hba_cmd_tbl_t {
    uint8_t cfis[64];
    uint8_t acmd[16];
    uint8_t rsv[48];
    struct hba_prdtl_t prdt_entry[1];
};

struct hba_port_t {
    uint32_t clb;
    uint32_t clbu;
    uint32_t fb;
    uint32_t fbu;
    uint32_t is;
    uint32_t ie;
    uint32_t cmd;
    uint32_t rsv0;
    uint32_t tfd;
    uint32_t sig;
    uint32_t ssts;
    uint32_t sctl;
    uint32_t serr;
    uint32_t sact;
    uint32_t ci;
    uint32_t sntf;
    uint32_t fbs;
    uint32_t rsv1[11];
    uint32_t vendor[4];
} __attribute__((packed));

struct hba_mem_t {
    uint32_t cap;
    uint32_t ghc;
    uint32_t is;
    uint32_t pi;
    uint32_t vs;
    uint32_t ccc_ctl;
    uint32_t ccc_pts;
    uint32_t em_loc;
    uint32_t em_ctl;
    uint32_t cap2;
    uint32_t bohc;
    uint8_t rsv[0xA0 - 0x2C];
    uint8_t vendor[0x100 - 0xA0];

    struct hba_port_t ports[MAX_AHCI_DEVICES];
} __attribute__((packed));

#endif
