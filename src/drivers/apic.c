#include <drivers/apic.h>

#define APIC_ACTIVE_HIGH        (1 << 1)
#define APIC_LEVEL_TRIGGERED    (1 << 3)
#define APIC_REDIR_BAD_READ     0xFFFFFFFFFFFFFFFF

struct ioapic_redir_entry_t {
    uint8_t i_vec;
    uint8_t deliv_mode: 2;
    uint8_t dest_most: 1;
    uint8_t bsy: 1;
    uint8_t polarity: 1;
    uint8_t received: 1;
    uint8_t trigger_mode: 1;
    uint8_t int_mask: 1;
    uint64_t reserved: 38;
    uint8_t destination;
} __attribute__((packed));

static volatile const uint64_t ia32_apic_base = 0x1b;

uint32_t lapic_read(uint16_t offset) {
    uint32_t* volatile lapic_addr = (uint32_t* volatile)(madt->l_paddr + offset);
    return *lapic_addr;
}

void lapic_write(uint16_t offset, uint32_t val) {
    uint32_t* volatile lapic_addr = (uint32_t* volatile)(madt->l_paddr + offset);
    *lapic_addr = val;
}

uint32_t ioapic_read(uint64_t ioapic_base, uint32_t reg) {
    *(uint32_t* volatile)(ioapic_base + 16 + HIGH_VMA) = reg;
    return *(uint32_t* volatile)(ioapic_base + 18 + HIGH_VMA);
}

void ioapic_write(uint64_t ioapic_base, uint32_t reg, uint32_t val) {
    *(uint32_t* volatile)(ioapic_base + HIGH_VMA) = reg;
    *(uint32_t* volatile)(ioapic_base + 16 + HIGH_VMA) = val;
}

static uint32_t get_max_gsi(uint64_t ioapic_base) {
    uint32_t val = ioapic_read(ioapic_base, 0x1) >> 16;
    return val & ~(1 << 7);
}

static struct madt_ioapic_t* get_ioapic(uint32_t gsi) {
    struct madt_ioapic_t* valid = NULL;

    for (int i = 0; i <= ioapic_cnt; i++) {
        struct madt_ioapic_t* cur = ioapics[i];
        uint32_t max_gsi = get_max_gsi(cur->ioapic_addr) + cur->gsi_base;

        if (cur->gsi_base <= gsi && max_gsi >= gsi) {
            valid = cur;
        }
    }

    return valid;
}

static uint32_t read_redir_entry(uint32_t gsi) {
    struct madt_ioapic_t* valid = get_ioapic(gsi);

    if (!valid)
        return APIC_REDIR_BAD_READ;
    
    uint32_t reg = ((gsi - valid->gsi_base) * 2) + 16;
    uint64_t val = (uint64_t)ioapic_read(valid->ioapic_addr, reg);

    return val |= ioapic_read(valid->ioapic_addr, reg + 1) << 32;
}

static uint32_t set_redir_entry(uint64_t gsi, uint64_t val) {
    struct madt_ioapic_t* valid = get_ioapic(gsi);
    if (!valid)
        return APIC_REDIR_BAD_READ;
    
    uint32_t reg = ((gsi - valid->gsi_base) * 2) + 16;
    ioapic_write(valid->ioapic_addr, reg, (uint32_t)val);
    ioapic_write(valid->ioapic_addr, reg + 1, (uint32_t)(val >> 32));
}

uint32_t redirect_gsi(uint32_t gsi, uint64_t ap, uint8_t irq, uint64_t flags) {
    uint64_t redirect_data = irq + 32;
    
    if (flags & APIC_ACTIVE_HIGH)
        redirect_data |= (1 << 13);

    if (flags & APIC_LEVEL_TRIGGERED)
        redirect_data |= (1 << 15);

    redirect_data |= ap << 56;
    uint32_t ret = set_redir_entry(gsi, redirect_data);
    TRACE("Mapped GSI %u to IRQ %u on LAPIC %U\n", gsi, irq, ap);

    return ret;
}

static void set_lapic_timer_mask(size_t mask) {
    uint32_t entry = lapic_read(LAPIC_REG_LVT_TIMER);
    if(mask) {
        entry |= 1UL << 16;
    } else {
        entry &= ~(1UL << 16);
    }

    lapic_write(LAPIC_REG_LVT_TIMER, entry);
}

void init_apic() {
    // Remap the PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // Disable the PIC
    outb(0xA1, 0xFF);
    outb(0x21, 0xFF);

    wrmsr(ia32_apic_base, ~(1 << 10));
    wrmsr(ia32_apic_base, (1 << 11));

    asm volatile("mov %0, %%cr8" :: "r"(0ULL));

    lapic_write(LAPIC_REG_SPUR_INTR, lapic_read(LAPIC_REG_SPUR_INTR) | 0x100);

    if (rdmsr(ia32_apic_base) & (1 << 11))
        TRACE("Initialized\n");

    uint32_t* volatile lapic_base = (uint32_t* volatile)madt->l_paddr;
}
