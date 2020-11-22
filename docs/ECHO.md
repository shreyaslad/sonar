# echo

Echo is a specification that allows guest kernels to describe the current machine and internal state so that it may be stored and consequently restored at a later date. Echo compliant kernels are paravirtualized by some hypervisor, such as sonar, in order to be hotswappable.

## General Theory

The system and internal state of the kernel should be stored in *some* format, that allows future kernels to easy parse and restore this saved state. Upon an update, the kernel should deploy a worker thread to fetch and determine the validity of a potentially new kernel image. If the image is determined to be new, the worker thread should signal the kernel to halt all execution and to dump the current state. The kernel will then load the image into memory and pass it through some kind of exposed interface so that the hypervisor may recieve it. The kernel will then transfer control to the hypervisor via a hypercall, at which point the hypervisor will load in the new kernel and virtualize it yet again. 

The hypervisor should expose it's existence through setting the CPU name in CPUID, as well as expose a PCI device to allow the paravirtualized kernel and hypervisor to communicate if need be.

## Tables