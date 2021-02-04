#ifndef __USB__XHCI_H__
#define __USB__XHCI_H__

#include <stdint.h>
#include <ospm/pci.h>

#define XHCI_CLASS      0x0c
#define XHCI_SUBCLASS   0x03
#define XHCI_PROGIF     0x30

struct xhci_cap_regs {
    uint8_t cap_len;
    uint8_t rsvd;
    uint16_t hci_ver;

    uint32_t hcs_params1;
    uint32_t hcs_params2;
    uint32_t hcs_params3;

    uint32_t hcc_params1;
    uint32_t dboff;
    uint32_t rtsoff;
    uint32_t hcc_params2;
} __attribute__((packed));

struct xhci_port_regs {
    uint32_t portsc;
    uint32_t portpmsc;
    uint32_t portli;
    uint32_t rsvd;
} __attribute__((packed, aligned(4)));

struct xhci_op_regs {
    uint32_t usbcmd;
    uint32_t usbsts;
    uint32_t page_size;
    uint8_t rsvd1[0x14 - 0x0C];
    uint32_t dnctrl;
    uint64_t crcr;
    uint8_t rsvd2[0x30 - 0x20];
    uint64_t dcbaap;
    uint32_t config;
    uint8_t rsvd3[964]; /* 3C - 3FF */
    volatile struct xhci_port_regs prs[256];
} __attribute__((packed, aligned(8)));

#define XHCI_IREGS_MAX 1024

struct xhci_int_regs {
    uint32_t iman;
    uint32_t imod;
    uint32_t erstsz;
    uint32_t rsvd;
    uint64_t erstba;
    uint64_t erdp;
} __attribute__((packed, aligned(8)));

struct xhci_run_regs {
    uint32_t mfindex;
    uint8_t reserved[28];
    volatile struct xhci_int_regs irs[XHCI_IREGS_MAX];
} __attribute((packed, aligned(8)));



#endif
