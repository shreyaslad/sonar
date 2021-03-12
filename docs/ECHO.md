# echo

Echo is a specification that allows guest kernels to describe the current machine and internal state so that it may be stored and consequently restored at a later date. Echo compliant kernels are paravirtualized by some hypervisor, such as sonar, in order to be hotswappable.

## General Theory

The system and internal state of the kernel should be stored in *some* format, that allows future kernels to easy parse and restore this saved state. Upon an update, the kernel should deploy a worker thread to fetch and determine the validity of a potentially new kernel image. If the image is determined to be new, the worker thread should signal the kernel to halt all execution and to dump the current state. The kernel will then load the image into memory and pass it through some kind of exposed interface so that the hypervisor may recieve it. The kernel will then transfer control to the hypervisor, at which point the hypervisor will load in the new kernel and virtualize it.

When the hypervisor loads the new guest kernel, it will call the ctor that the kernel sets using the echo runtime services. This ctor is the one responsible for parsing saved state and loading it. Once it returns, the hypervisor will jump to the normal kernel entry point.

If no ctor is provided, the hypervisor will simply jump to the kernel entry point.

The hypervisor should expose it's existence through setting the Hypervisor Present Bit.

## Tables

Sonar does not know the data or the format of the data that the kernel wishes to save in between launches, so it only provides a general method to store such data so that it can be passed on and interpreted by the next kernel.

```c
struct echo_table_hdr {
    uint64_t utid;  // Unique table identifier
    char name[8];

    uint64_t* next; // Next table in the list

    uint64_t len;   // Length in bytes
    uint64_t* data;
} __attribute__((packed));
```

The one exception to this, however, is the Resource Table. The Resource Table describes the new kernel to be loaded, providing the underlying hypervisor with all the information it needs to know to swap the current guest kernel out. The structure of this table is described below, and **must** be present in every 

## Echo Information

Echo is backwards compatible with stivale2 kernels. For this reason, it passes information to the virtualized kernel as a stivale2 tag. The ID of this tag and its structure is described below. Note that the kernel does not request any stivale2 tags related to echo; it only parses them.

```c
#define STIVALE2_STRUCT_TAG_ECHO_INFO

struct stivale2_struct_tag_echo_info {
    struct stivale2_tag tag;

    uint64_t warn;                         // Any warnings
    uint64_t err;                          // Any errors

    struct echo_runtime* services;   // The echo runtime services
} __attribute__((packed));
```

Non-fatal warnings/errors don't prevent the kernel from loading, but may hamper some functionality or allude to problems. Echo reports non-fatal warnings/errors to the guest kernel in two ways:
1. Setting the `warn`/`err` field(s) in the echo information struct
2. Calling the error handlers that the kernel sets up

It's recommended that the guest kernel install handlers for the kernel constructor (which is responsible for loading any saved data), the kernel destructor (which is responsible for saving internal state), and error handlers using `echo_register_handle` in the hypervisor runtime service. The underlying hypervisor will call these functions on its own when it sees fit. For example, it will independently call the kernel dtor when the guest kernel wants to create a snapshot *and* when the guest kernel wants to swap out.

## Error Codes

Error and warning codes are bitwise OR'd with each other to form the `echo_err` and `echo_warn` fields.

In addition, the guest kernel can use the echo runtime services to install custom error handlers for each type of error. If installed, these handlers will be called before the ctor so that the kernel can appropriatly handle them.

It's recommended that the kernel either installs handlers or checks the `echo_err` field in the entry point.

```c
enum echo_error_code {
    TODO = (1 << 2)
};
```
| Code | Description |
| ---- | ----------- |
| `(1 << 2)` | TODO |

Notice that the first error code starts at 

## Flags

Flags allow the guest kernel to control various settings in the hypervisor's operation. A guest kernel can retrieve/modify flags by using the hypervisor runtime services. As with errors, flags are bitwise OR'd with each other, but this **is not** something that the guest kernel must worry about. The guest kernel sets/clears flags using the echo hypervisor services.

```c
enum echo_flags {
    // Retrieve internal kernel state on kernel exit
    DUMP_KSTATE_ON_EXIT = (1 << 0),

    // Automatically write all echo variables to disk on kernel exit
    PRESERVE_VAR_ON_EXIT = (1 << 1),

    // Automatically write all echo snapshots to disk on kernel exit
    PRESERVE_SNAPSHOT_ON_EXIT = (1 << 2),
};
```

## Variables

Echo allocates space for guest kernels to specifically store information or flags in-between runs; these mechanisms are called echo variables. The Echo Runtime Services expose functions to create, update, and retrieve variables. Each variable is 8 bytes in length and is atomically written to/read from by the hypervisor.

```c
struct echo_var {
    uint64_t uvid;      // Unique variable ID
    char name[16];

    uint64_t ctime;     // Creation time

    uint64_t contents;
} __attribute__((packed));
```

## Snapshots

Echo provides a method for guest kernels to save state without swapping, and then either inspect or restore to that state. For instance, upon crashing, a kernel might save its state in a snapshot, switch out to a fresh version of the same kernel, and then inspect the faulting state of the machine.

Each snapshot is created by saving the system state and then simply calling the kernel dtor. They are then assigned a unique tag and marked with a timestamp. The guest kernel can request all snapshots or a snapshot with a specific tag. 

#### Snapshot Structure

```c
struct echo_snapshot {
    uint64_t usid;  // Unique snapshot ID
    char name[16];

    uint64_t ctime; // Creation time

    struct echo_regs* regs;
    struct echo_frames* stacktrace;
    struct echo_tables* k_state;
} __attribute__((packed));
```

#### Registers Structure

```c
struct echo_regs {
    uint64_t rax, rbx, rcx, rdx, rsp, rbp, rdi, rsi, r8, r9, r10, r11, r12, r13, r14, r15, rip;
    uint64_t cs, ds, es, fs, gs, ss;
    uint64_t gdtr, idtr;
} __attribute__((packed));
```

#### Stackframe Structure

```c
struct echo_stackframe {
    uint64_t addr;
    char name[24];

    struct echo_stackframe* next;
} __attribute__((packed));
```

## Runtime Services

```c
struct echo_runtime {
    /* Hypervisor Services */
    ...


    /* Variable Services */
    ...

    /* Snapshot Services */
    ...
};
```

#### Hypervisor Services

```c
int swap_kernel();

int exit_kernel();

int install_handle(uint64_t type,
                    void (*handler)(struct echo_runtime_servs*));

int uninstall_handle(uint64_t type);

int set_flag(uint64_t flag);

int clear_flag(uint64_t flag);

int get_flag(uint64_t flag);

int set_kernel_state(struct echo_tbl_hdr* head);

int get_kernel_state(struct echo_tbl_hdr* ret);
```

##### Handler Types

Handler IDs are used when registering handlers for different tasks. The underlying hypervisor will invoke these handlers on its own when it sees fit, and their only job is to 

```c
enum echo_handler_type {
    K_CTOR = (1 << 0),  // The handle is a kernel constructor
    K_DTOR = (1 << 1)   // The handle is a kernel destructor
};
```

#### Variable Services

TODO: Provide some way to enumerate over variables and snapshots
TODO: Provide a bitmap  to apply the same operation to a number of variables/snapshots

```c
// Atomically create a new variable
int echo_create_var(char* name, uint64_t val);

// Write a variable to disk. Ideally, this should only be done in the
// kernel destructor
int echo_preserve_var(char* name);

// Atomically store a new value in a variable
int echo_update_var(char* name, uint64_t val);

// Atomically retrieve a variable
// Note that `ret` is a copy, not the actual variable
int echo_get_var(char* name, struct echo_var* ret);

// Atomically delete a variable
int echo_delete_var(char* name);

// Atomically rename a variable
int echo_rename_var(char* name, char* new_name);
```

#### Snapshot Services

```c
int echo_create_snapshot(char* name);

int echo_preserve_snapshot(char* name);

int echo_get_snapshot(char* name, struct echo_snapshot* ret);

int echo_delete_snapshot(char* name);

int echo_rename_snapshot(char* name, char* new_name);
```