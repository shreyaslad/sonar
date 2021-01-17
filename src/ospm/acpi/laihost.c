#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <str.h>
#include <trace.h>
#include <ospm/acpi/acpi.h>
#include <ospm/acpi/lai/host.h>
#include <ospm/pci.h>

void laihost_panic(const char* str) {
    panic(str);
}

void laihost_log(int level, const char* str) {
    switch (level) {
        case LAI_DEBUG_LOG:
            TRACE("%s\n", str);
            break;
        case LAI_WARN_LOG:
            WARN("%s\n", str);
            break;
        default:
            WARN("%s\n", str);
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
    return find_sdt(signature, index);
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

    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    return pci_read_device_byte(&pci_dev, offset);
}

void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint8_t value) {
    (void)seg;

    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    pci_write_device_byte(&pci_dev, offset, value);
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    (void)seg;

    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    return pci_read_device_word(&pci_dev, offset);
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t value) {
    (void)seg;
    
    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    pci_write_device_word(&pci_dev, offset, value);
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    (void)seg;

    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    return pci_read_device_dword(&pci_dev, offset);
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t value) {
    (void)seg;

    struct pci_device_t pci_dev = {
        .bus = bus,
        .device = device,
        .func = function
    };

    pci_write_device_dword(&pci_dev, offset, value);
}

void* laihost_map(size_t paddr, size_t count) {
    (void)count;

    return (void *)(paddr + HIGH_VMA);
}
