#include <ospm/pci.h>

#define MAX_FUNCTION    8
#define MAX_DEVICE      32
#define MAX_BUS         256

struct pci_device_t* pci_drivers;

size_t available_drivers = 0;

#define BYTE    0
#define WORD    1
#define DWORD   2

static void pci_check_bus(uint8_t, int64_t);
static char* get_dev_type(uint8_t class, uint8_t subclass, uint8_t prog_if);

static void get_address(struct pci_device_t* device, uint32_t offset) {
    uint32_t address = (device->bus << 16) | (device->device << 11) | (device->func << 8)
        | (offset & ~((uint32_t)(3))) | 0x80000000;
    outd(0xcf8, address);
}

uint8_t pci_read_device_byte(struct pci_device_t* device, uint32_t offset) {
    get_address(device, offset);
    return ind(0xcfc + (offset & 3));
}

void pci_write_device_byte(struct pci_device_t* device, uint32_t offset, uint8_t value) {
    get_address(device, offset);
    outb(0xcfc + (offset & 3), value);
}

uint16_t pci_read_device_word(struct pci_device_t* device, uint32_t offset) {
    get_address(device, offset);
    return inw(0xcfc + (offset & 3));
}

void pci_write_device_word(struct pci_device_t* device, uint32_t offset, uint16_t value) {
    get_address(device, offset);
    outw(0xcfc + (offset & 3), value);
}

uint32_t pci_read_device_dword(struct pci_device_t* device, uint32_t offset) {
    get_address(device, offset);
    return ind(0xcfc + (offset & 3));
}

void pci_write_device_dword(struct pci_device_t* device, uint32_t offset, uint32_t value) {
    get_address(device, offset);
    outd(0xcfc + (offset & 3), value);
}

int pci_read_bar(struct pci_device_t* device, int bar, struct pci_bar_t* out) {
    if (bar > 5) {
        return -1;
    }

    size_t reg_index = 0x10 + bar * 4;
    uint64_t bar_low = pci_read_device_dword(device, reg_index), bar_size_low;
    uint64_t bar_high = 0, bar_size_high = 0;

    if (!bar_low) {
        return -1;
    }

    uintptr_t base;
    size_t size;

    int is_mmio = !(bar_low & 1);
    int is_prefetchable = is_mmio && bar_low & (1 << 3);
    int is_64bit = is_mmio && ((bar_low >> 1) & 0b11) == 0b10;

    if (is_64bit) {
        bar_high = pci_read_device_dword(device, reg_index + 4);
    }

    base = ((bar_high << 32) | bar_low) & ~(is_mmio ? (0b1111) : (0b11));

    pci_write_device_dword(device, reg_index, 0xFFFFFFFF);
    bar_size_low = pci_read_device_dword(device, reg_index);
    pci_write_device_dword(device, reg_index, bar_low);

    if (is_64bit) {
        pci_write_device_dword(device, reg_index + 4, 0xFFFFFFFF);
        bar_size_high = pci_read_device_dword(device, reg_index + 4);
        pci_write_device_dword(device, reg_index + 4, bar_high);
    }

    size = ((bar_size_high << 32) | bar_size_low) & ~(is_mmio ? (0b1111) : (0b11));
    size = ~size + 1;

    if (out) {
        *out = (struct pci_bar_t){base, size, is_mmio, is_prefetchable};
    }

    return 0;
}


void pci_enable_busmastering(struct pci_device_t* device) {
    if (!(pci_read_device_dword(device, 0x4) & (1 << 2))) {
        pci_write_device_dword(device, 0x4, pci_read_device_dword(device, 0x4) | (1 << 2));
    }
}

int pci_get_device(struct pci_device_t* ret, uint8_t class, uint8_t subclass, uint8_t prog_if) {
    for (size_t i = 0; i < stbds_arrlen(pci_drivers); i++) {
        struct pci_device_t dev = pci_drivers[i];

        if (dev.device_class == class && dev.subclass == subclass && dev.prog_if == prog_if) {
            memcpy(ret, &dev, sizeof(struct pci_device_t));
            return 1;
        }
    }

    return 0;
}

int pci_get_device_by_vendor(struct pci_device_t* ret, uint16_t vendor, uint16_t id, size_t index) {
    for (size_t i = 0; i < stbds_arrlen(pci_drivers); i++) {
        struct pci_device_t dev = pci_drivers[i];
        
        if (dev.vendor_id == vendor && dev.device_id == id) {
            memcpy(ret, &dev, sizeof(struct pci_device_t));
            return 1;
        }
    }

    return 0;
}

static void pci_check_function(uint8_t bus, uint8_t slot, uint8_t func, int64_t parent) {
    struct pci_device_t device = {0};
    device.bus = bus;
    device.func = func;
    device.device = slot;

    uint32_t config_0 = pci_read_device_dword(&device, 0);

    if (config_0 == 0xffffffff) {
        return;
    }

    uint32_t config_8 = pci_read_device_dword(&device, 0x8);
    uint32_t config_c = pci_read_device_dword(&device, 0xc);
    uint32_t config_3c = pci_read_device_dword(&device, 0x3c);

    device.parent = parent;
    device.device_id = (uint16_t)(config_0 >> 16);
    device.vendor_id = (uint16_t)config_0;
    device.rev_id = (uint8_t)config_8;
    device.subclass = (uint8_t)(config_8 >> 16);
    device.device_class = (uint8_t)(config_8 >> 24);
    device.prog_if = (uint8_t)(config_8 >> 8);
    device.irq_pin = (uint8_t)(config_3c >> 8);

    if (config_c & 0x800000) {
        device.multifunction = 1;
    } else {
        device.multifunction = 0;
    }

    available_drivers++;
    stbds_arrput(pci_drivers, device);
    LOG("-\t%d:%d.%d: %s\n", bus, slot, func,
            get_dev_type(device.device_class, device.subclass, device.prog_if));

    size_t id = available_drivers;

    if (device.device_class == 0x06 && device.subclass == 0x04) {
        // pci to pci bridge
        struct pci_device_t device = pci_drivers[id];

        // find drivers attached to this bridge
        uint32_t config_18 = pci_read_device_dword(&device, 0x18);
        pci_check_bus((config_18 >> 8) & 0xFF, id);
    }
}

static void pci_check_bus(uint8_t bus, int64_t parent) {
    for (size_t dev = 0; dev < MAX_DEVICE; dev++) {
        for (size_t func = 0; func < MAX_FUNCTION; func++) {
            pci_check_function(bus, dev, func, parent);
        }
    }
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

static void pci_init_root_bus() {
    struct pci_device_t device = {0};
    uint32_t config_c = pci_read_device_dword(&device, 0xc);
    uint32_t config_0;

    if (!(config_c & 0x800000)) {
        pci_check_bus(0, -1);
    } else {
        for (size_t func = 0; func < MAX_FUNCTION; func++) {
            device.func = func;
            config_0 = pci_read_device_dword(&device, 0);
            if (config_0 == 0xffffffff) {
                continue;
            }

            pci_check_bus(func, -1);
        }
    }
}

void init_pci() {
    LOG("Enumerated devices:\n");
    pci_init_root_bus();
}
