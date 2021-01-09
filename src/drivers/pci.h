#ifndef __DRIVERS__PCI_H__
#define __DRIVERS__PCI_H__

#include <stdint.h>
#include <alloc.h>
#include <locks.h>
#include <mem.h>
#include <trace.h>
#include <vec.h>
#include <sys/ports.h>
#include <ospm/acpi/acpi.h>

#define __MODULE__ "pci" 

struct pci_id_t {
    uint8_t class;
    uint8_t subclass;
    uint8_t prog_if;
};

struct pci_dev_t {
    uint32_t* cfg_space;

    uint8_t bus;
    uint8_t device;
    uint8_t function;
};

uint32_t pci_pio_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg);
void pci_pio_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg, uint32_t data);

void init_pci();

#endif
