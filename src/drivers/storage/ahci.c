#include <drivers/storage/ahci.h>

struct cached_block_t {
    uint8_t* cache;
    uint64_t block;
    int status
};

struct ahci_device_t {
    volatile struct hba_port_t* port;
    int exists;
    uint64_t sector_count;
    struct cached_block_t* cache;
    int overwritten_slot;
};

struct ahci_device_t* ahci_drivers;

static int check_type(volatile struct hba_port_t *port) {
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0f;
    uint8_t det = ssts & 0x0f;

    /* Check for correct interface and active state */
    if ((ipm != 0x01) || (det != 0x03))
        return AHCI_DEV_NULL;

    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }

    return AHCI_DEV_NULL;
}

static int probe_port(struct hba_mem_t* mem, size_t port) {
    uint32_t pi = mem->pi;

    if (pi & (1 << port)) {
        return check_type(&mem->ports[port]);
    }

    return AHCI_DEV_NULL;
}

void init_ahci() {
    struct pci_device_t device;
    
    if (!pci_get_device(&device, AHCI_CLASS, AHCI_SUBCLASS, AHCI_PROG_IF)) {
        ERR("Failed to find AHCI controller! SATA support unavailable\n");
        return;
    }

    pci_enable_busmastering(&device);
}
