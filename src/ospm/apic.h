#ifndef __SYS__INT__APIC_H__
#define __SYS__INT__APIC_H__

#include <stdint.h>
#include <log.h>
#include <ospm/acpi/acpi.h>
#include <sys/ports.h>
#include <sys/msrs.h>

#define __MODULE__ "apic"

#define LAPIC_REG_ID                    0x20
#define LAPIC_REG_VER                   0x30
#define LAPIC_REG_TPR                   0x80
#define LAPIC_REG_APR                   0x90
#define LAPIC_REG_PPR                   0xA0
#define LAPIC_REG_EOI                   0xB0
#define LAPIC_REG_RRD                   0xC0
#define LAPIC_REG_LOGICAL_DEST          0xD0
#define LAPIC_REG_DEST_FMT              0xE0
#define LAPIC_REG_SPUR_INTR             0xF0

#define LAPIC_REG_ISR0                  0x100
#define LAPIC_REG_ISR1                  0x110
#define LAPIC_REG_ISR2                  0x120
#define LAPIC_REG_ISR3                  0x130
#define LAPIC_REG_ISR4                  0x140
#define LAPIC_REG_ISR5                  0x150
#define LAPIC_REG_ISR6                  0x160
#define LAPIC_REG_ISR7                  0x170

#define LAPIC_REG_TMR0                  0x180
#define LAPIC_REG_TMR1                  0x190
#define LAPIC_REG_TMR2                  0x1A0
#define LAPIC_REG_TMR3                  0x1B0
#define LAPIC_REG_TMR4                  0x1C0
#define LAPIC_REG_TMR5                  0x1D0
#define LAPIC_REG_TMR6                  0x1E0
#define LAPIC_REG_TMR7                  0x1F0

#define LAPIC_REG_IRR0                  0x200
#define LAPIC_REG_IRR1                  0x210
#define LAPIC_REG_IRR2                  0x220
#define LAPIC_REG_IRR3                  0x230
#define LAPIC_REG_IRR4                  0x240
#define LAPIC_REG_IRR5                  0x250
#define LAPIC_REG_IRR6                  0x260
#define LAPIC_REG_IRR7                  0x270

#define LAPIC_REG_ERR_ST                0x280
#define LAPIC_REG_LVT_CMCI              0x2F0
#define LAPIC_REG_ICR0                  0x300
#define LAPIC_REG_ICR1                  0x310
#define LAPIC_REG_LVT_TIMER             0x320
#define LAPIC_REG_LVT_THERM             0x330
#define LAPIC_REG_LVT_PERFMON           0x340
#define LAPIC_REG_LVT_LINT0             0x350
#define LAPIC_REG_LVT_LINT1             0x360
#define LAPIC_REG_LVT_ERR               0x370
#define LAPIC_REG_TIMER_INITCNT         0x380
#define LAPIC_REG_TIMER_CURCNT          0x390
#define LAPIC_REG_TIMER_DIVCONF         0x3E0

extern struct acpi_madt* madt;

extern struct madt_lapic** lapics;
extern int lapic_cnt;

extern struct madt_ioapic** ioapics;
extern int ioapic_cnt;

extern struct madt_iso** isos;
extern int iso_cnt;

extern struct madt_nmi** nmis;
extern int nmi_cnt;

uint32_t lapic_read(uint16_t offset);
void lapic_write(uint16_t offset, uint32_t val);

uint32_t ioapic_read(uint64_t ioapic_base, uint32_t reg);
void ioapic_write(uint64_t ioapic_base, uint32_t reg, uint32_t val);

uint32_t redirect_gsi(uint32_t gsi, uint64_t ap, uint8_t irq, uint64_t flags);

void init_lapic_timer();
void init_apic();

void init_madt();

#endif
