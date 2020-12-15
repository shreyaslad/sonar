#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <str.h>
#include <trace.h>
#include <ospm/acpi/acpi.h>
#include <ospm/acpi/lai/host.h>
#include <drivers/pci.h>

void laihost_panic(const char* str) {
    panic(str);
}

void laihost_log(int level, const char* str) {
    switch (level) {
        case LAI_DEBUG_LOG:
            TRACE("%s", str);
            break;
        case LAI_WARN_LOG:
            WARN("%s", str);
            break;
        default:
            WARN("%s", str);
            break;
    }
}

void* laihost_malloc(size_t size) {
    return kmalloc(size);
}

void* laihost_realloc(void* p, size_t newsize, size_t oldsize) {
    (void)oldsize;

    return krealloc(p, newsize);
}

void laihost_free(void* p, size_t size) {
    (void)size;

    kfree(p);
}

// TODO: sleep

void* laihost_scan(const char* signature, size_t index) {
    if (!strncmp(signature, "DSDT", 4)) {
        if (index > 0) {
            #define __MODULE__ "acpi"
            ERR("Only valid DSDT index is 0\n");
            return NULL;
        }

        void* fadt = find_sdt("FACP", 0);
        void* dsdt = (void *)(*(size_t *)((size_t)fadt + sizeof(struct sdt_t) + 4));
        return dsdt;
    } else {
        return find_sdt(signature, index);
    }
}

uint8_t laihost_inb(uint16_t port) {
    return inb(port);
}

void laihost_outb(uint16_t port, uint8_t value) {
    outb(port, value);
}

uint16_t laihost_inw(uint16_t port) {
    return inw(port);
}

void laihost_outw(uint16_t port, uint16_t value) {
    outw(port, value);
}

uint32_t laihost_ind(uint16_t port) {
    return ind(port);
}

void laihost_outd(uint16_t port, uint32_t value) {
    outd(port, value);
}

uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    (void)seg;

    return pci_pio_read_dword(bus, device, function, (uint8_t)offset);
}

void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint8_t value) {
    (void)seg;

    pci_pio_write_dword(bus, device, function, (uint16_t)offset, (uint8_t)value);
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    (void)seg;

    return pci_pio_read_dword(bus, device, function, (uint8_t)offset);
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t value) {
    (void)seg;
    pci_pio_write_dword(bus, device, function, offset, (uint16_t)value);
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    (void)seg;

    return pci_pio_read_dword(bus, device, function, offset);
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t value) {
    (void)seg;

    pci_pio_write_dword(bus, device, function, offset, value);
}

void* laihost_map(size_t paddr, size_t count) {
    (void)count;

    return (void *)(paddr + HIGH_VMA);
}