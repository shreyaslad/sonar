#include <drivers/pci.h>

struct pci_cfg_desc_t {
    uint64_t base;
    uint16_t segment;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t reserved;
} __attribute__((packed));

struct acpi_mcfg_t {
    struct sdt_t sdt;

    uint64_t reserved;
    struct pci_cfg_desc_t descs[1];
} __attribute__((packed));

static struct vector_t* devices;
static struct vector_t* handlers;

static uint16_t pci_cfg_desc_cnt;
static struct pci_cfg_desc_t* cfg_descs;

uint32_t pci_pio_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg) {
    uint32_t pci_cfg_addr = (1 << 31)                               |
                            ((uint32_t)bus << 16)                   |
                            (((uint32_t)device & 0b11111) << 11)    |
                            (((uint32_t)function & 0b111) << 8)     |
                            (reg & ~(0b11));

    outd(0xcf8, pci_cfg_addr);
    return ind(0xcfc);
}

void pci_pio_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg, uint32_t data) {
    uint32_t pci_cfg_addr = (1 << 31)                               |
                            ((uint32_t)bus << 16)                   |
                            (((uint32_t)device & 0b11111) << 11)    |
                            (((uint32_t)function & 0b111) << 8)     |
                            (reg & ~(0b11));

    outd(0xcf8, pci_cfg_addr);
    outd(0xcfc, data);
}

static char* get_dev_type(uint8_t class, uint8_t subclass, uint8_t prog_if) {
    switch (class) {
        case 0: return "Undefined";
        case 1:
            switch (subclass) {
                case 0: return "SCSI Bus Controller";
                case 1: return "IDE Controller";
                case 2: return "Floppy Disk Controller";
                case 3: return "IPI Bus Controller";
                case 4: return "RAID Controller";
                case 5: return "ATA Controller";
                case 6:
                    switch (prog_if) {
                        case 0: return "Vendor specific SATA Controller";
                        case 1: return "AHCI SATA Controller";
                        case 2: return "Serial Storage Bus SATA Controller";
                    }

                    break;
                case 7: return "Serial Attached SCSI Controller";
                case 8:
                    switch (prog_if) {
                        case 1: return "NVMHCI Controller";
                        case 2: return "NVMe Controller";
                    }

                    break;
        }

        return "Mass Storage Controller";

        case 2:
            switch (subclass) {
                case 0: return "Ethernet Controller";
                case 1: return "Token Ring Controller";
                case 2: return "FDDI Controller";
                case 3: return "ATM Controller";
                case 4: return "ISDN Controller";
                case 5: return "WorldFip Controller";
                case 6: return "PICMG 2.14 Controller";
                case 7: return "InfiniBand Controller";
                case 8: return "Fabric Controller";
            }

            return "Network Controller";
        case 3:
            switch (subclass) {
                case 0: return "VGA Compatible Controller";
                case 1: return "XGA Controller";
                case 2: return "3D Controller";
            }

            return "Display Controller";
        case 4: return "Multimedia controller";
        case 5: return "Memory Controller";
        case 6:
            switch (subclass) {
                case 0: return "Host Bridge";
                case 1: return "ISA Bridge";
                case 2: return "EISA Bridge";
                case 3: return "MCA Bridge";
                case 4: return "PCI-to-PCI Bridge";
                case 5: return "PCMCIA Bridge";
                case 6: return "NuBus Bridge";
                case 7: return "CardBus Bridge";
                case 8: return "RACEway Bridge";
                case 9: return "Semi-Transparent PCI-to-PCI Bridge";
                case 10: return "InfiniBand-to-PCI Host Bridge";
            }
            return "Bridge Device";
        case 8:
            switch (subclass) {
                case 0:
                    switch (prog_if) {
                        case 0: return "8259-Compatible PIC";
                        case 1: return "ISA-Compatible PIC";
                        case 2: return "EISA-Compatible PIC";
                        case 0x10: return "I/O APIC IRQ Controller";
                        case 0x20: return "I/O xAPIC IRQ Controller";
                    }

                    break;
                case 1:
                    switch (prog_if) {
                        case 0: return "8237-Compatible DMA Controller";
                        case 1: return "ISA-Compatible DMA Controller";
                        case 2: return "EISA-Compatible DMA Controller";
                    }

                    break;
                case 2:
                    switch (prog_if) {
                        case 0: return "8254-Compatible PIT";
                        case 1: return "ISA-Compatible PIT";
                        case 2: return "EISA-Compatible PIT";
                        case 3: return "HPET";
                    }

                    break;
                case 3: return "Real Time Clock";
                case 4: return "PCI Hot-Plug Controller";
                case 5: return "SDHCI";
                case 6: return "IOMMU";
            }

            return "Base System Peripheral";
        case 0xC:
            switch (subclass) {
                case 0:
                    switch (prog_if) {
                        case 0: return "Generic FireWire (IEEE 1394) Controller";
                        case 0x10: return "OHCI FireWire (IEEE 1394) Controller";
                    }

                    break;
                case 1: return "ACCESS Bus Controller";
                case 2: return "SSA Controller";
                case 3:
                    switch (prog_if) {
                        case 0: return "uHCI USB1 Controller";
                        case 0x10: return "oHCI USB1 Controller";
                        case 0x20: return "eHCI USB2 Controller";
                        case 0x30: return "xHCI USB3 Controller";
                        case 0xFE: return "USB Device";
                    }
                    break;
                case 4: return "Fibre Channel Controller";
                case 5: return "SMBus";
                case 6: return "InfiniBand Controller";
                case 7: return "IPMI Interface Controller";
            }

            return "Serial Bus Controller";
        default:
            return "Unknown";
            break;
    }
}

static uint32_t* pci_cfg_space(uint16_t bus, uint16_t dev, uint16_t func) {
    for (int i = 0; i < pci_cfg_desc_cnt; i++) {
        struct pci_cfg_desc_t* cfg = &cfg_descs[i];

        if (bus >= cfg->start_bus && bus < cfg->end_bus) {
            return (uint32_t*)(cfg->base + ((bus - cfg->start_bus) << 20 | dev << 15 | func << 12));
        }
    }

    return NULL;
}

static void pci_enumerate() {
    TRACE("Enumerating PCI Devices:\n");

    for (uint16_t bus = 0; bus < 256; bus++) {
        if (!pci_cfg_space(bus, 0, 0)) {
            continue;
        }

        for (uint16_t dev = 0; dev < 256; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t* cfg_space = pci_cfg_space(bus, dev, func);

                uint32_t vid_pid = cfg_space[0];

                if (vid_pid == 0xFFFFFFFF)
                    continue;
                    
                uint16_t c_sub = cfg_space[2] >> 16;

                struct pci_dev_t* device = kmalloc(sizeof(struct pci_dev_t));
                device->bus = bus;
                device->device = dev;
                device->function = func;
                device->cfg_space = cfg_space;
                vec_a(devices, device);

                TRACE("\t- %d:%d.%d: %s\n", bus, dev, func, get_dev_type(c_sub >> 8, c_sub, cfg_space[2]));
            }
        }
    }
}

void init_pci() {
    devices = kmalloc(sizeof(struct vector_t)) + HIGH_VMA;
    handlers = kmalloc(sizeof(struct vector_t)) + HIGH_VMA;
    vec_n(devices);

    struct acpi_mcfg_t* mcfg = find_sdt("MCFG", 0);
    pci_cfg_desc_cnt = (mcfg->sdt.len - sizeof(struct sdt_t)) / sizeof(struct pci_cfg_desc_t);
    cfg_descs = mcfg->descs;

    pci_enumerate();
}
