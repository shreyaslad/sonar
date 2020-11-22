#include <virt/intel/iommu/iommu.h>

void init_iommu() {
    struct dmar_t* dmar = find_sdt("DMAR", 0);

    // TODO: everything else
}
