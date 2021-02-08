#include <ospm/apic.h>

struct acpi_madt* madt;

struct lapic_t** lapics;
int lapic_cnt;

struct ioapic_t** ioapics;
int ioapic_cnt;

struct iso_t** isos;
int iso_cnt;

struct nmi_t** nmis;
int nmi_cnt;

#define APIC_ACTIVE_HIGH        (1 << 1)
#define APIC_LEVEL_TRIGGERED    (1 << 3)
#define APIC_REDIR_BAD_READ     0xFFFFFFFFFFFFFFFF

struct ioapic_redir_entry {
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

#define IA32_APIC_BASE      0x1b
#define IA32_APIC_BASE_BSP  0x100
#define IA32_APIC_ENABLE    0x800

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

static struct ioapic_t* get_ioapic(uint32_t gsi) {
    struct ioapic_t* valid = NULL;

    for (int i = 0; i <= ioapic_cnt; i++) {
        struct ioapic_t* cur = ioapics[i];
        uint32_t max_gsi = get_max_gsi(cur->ioapic_addr) + cur->gsi_base;

        if (cur->gsi_base <= gsi && max_gsi >= gsi) {
            valid = cur;
        }
    }

    return valid;
}

static uint32_t read_redir_entry(uint32_t gsi) {
    struct ioapic_t* valid = get_ioapic(gsi);

    if (!valid)
        return APIC_REDIR_BAD_READ;
    
    uint32_t reg = ((gsi - valid->gsi_base) * 2) + 16;
    uint64_t val = (uint64_t)ioapic_read(valid->ioapic_addr, reg);

    return val |= ioapic_read(valid->ioapic_addr, reg + 1) << 32;
}

static uint32_t set_redir_entry(uint64_t gsi, uint64_t val) {
    struct ioapic_t* valid = get_ioapic(gsi);
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

    // set LAPIC enable
    wrmsr(IA32_APIC_BASE, (rdmsr(IA32_APIC_BASE) | IA32_APIC_ENABLE) & ~(1 << 10));

    // enable spurious interrupts
    lapic_write(0xf0, lapic_read(0xf0) | (1 << 11));

    asm volatile("mov %0, %%cr8" :: "r"(0ULL));

    lapic_write(LAPIC_REG_TPR, 0);
    lapic_write(LAPIC_REG_SPUR_INTR, lapic_read(LAPIC_REG_SPUR_INTR) | 0x100);

    if (!(rdmsr(IA32_APIC_BASE) & (1 << 11))) {
        panic("Not properly initialized!\n");
    }

    struct cpu_t* cpu = get_cpu();
    cpu->lapic_id = (lapic_read(LAPIC_REG_ID) >> 24) & 0xff;
    cpu->lapic = lapics[cpu->lapic_id];

    uint32_t edx;

    if (!cpuid_count(1, 0, NULL, NULL, NULL, &edx)) {
        panic("Failed to retrieve BSP LAPIC ID!");
    }

    uint8_t bsp_lapic_id = edx >> 24;

    if (bsp_lapic_id == cpu->lapic_id) {
        cpu->ap_type = CPU_BSP;
    } else {
        cpu->ap_type = CPU_AP;
    }

    //uint32_t* volatile lapic_base = (uint32_t* volatile)madt->l_paddr;
}

void init_madt() {
    if ((madt = find_sdt("APIC", 0))) {
        TRACE("APIC configuration:\n");

        lapics = kmalloc(ACPI_MAX_TBL_CNT) + HIGH_VMA;
        ioapics = kmalloc(ACPI_MAX_TBL_CNT) + HIGH_VMA;
        isos = kmalloc(ACPI_MAX_TBL_CNT) + HIGH_VMA;
        nmis = kmalloc(ACPI_MAX_TBL_CNT) + HIGH_VMA;

        for (uint8_t* madt_ptr = (uint8_t *)(&madt->madt_entries_begin);
            (size_t)madt_ptr < (size_t)madt + madt->sdt.len;
            madt_ptr += *(madt_ptr + 1)) {
                switch (*(madt_ptr)) {
                    case 0:
                        TRACE("-\tLAPIC #%u\n", lapic_cnt);
                        lapics[lapic_cnt++] = (struct madt_lapic_t *)madt_ptr;
                        break;
                    case 1:
                        TRACE("-\tIOAPIC #%u\n", ioapic_cnt);
                        ioapics[ioapic_cnt++] = (struct ioapic_t *)madt_ptr;
                        break;
                    case 2:
                        TRACE("-\tISO #%u\n", iso_cnt);
                        isos[iso_cnt++] = (struct madt_iso_t *)madt_ptr;
                        break;
                    case 4:
                        TRACE("-\tNMI #%u\n", nmi_cnt);
                        nmis[nmi_cnt++] = (struct madt_nmi_t *)madt_ptr;
                        break;
                    default:
                        WARN("-\tNothing found\n");
                        break;
                }
        }
    }
}
