#ifndef __OSPM_H__
#define __OSPM_H__

#include <stdint.h>
#include <protos/stivale2.h>
#include <ospm/acpi/acpi.h>
#include <ospm/apic.h>
#include <ospm/smp.h>

#define __MODULE__ "ospm"

void init_ospm(struct stivale2_struct_tag_rsdp* rsdp_tag,
               struct stivale2_struct_tag_smp* smp_tag);

#endif
