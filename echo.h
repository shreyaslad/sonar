#ifndef __ECHO_H__
#define __ECHO_H__

#include <stdint.h>

struct echo_stivale2_tag {
    uint64_t identifier;
    uint64_t next;
} __attribute__((packed));

enum echo_flags {
    // Retrieve internal kernel state on kernel exit
    DUMP_KSTATE_ON_EXIT = (1 << 0),

    // Automatically write all echo variables to disk on kernel exit
    PRESERVE_VAR_ON_EXIT = (1 << 1),

    // Automatically write all echo snapshots to disk on kernel exit
    PRESERVE_SNAPSHOT_ON_EXIT = (1 << 2),
};

enum echo_error_code {
    TODO = (1 << 2)
};

struct stivale2_struct_tag_echo_info {
    struct echo_stivale2_tag tag;

    uint64_t warn;                   // Any warnings
    uint64_t err;                    // Any errors

    struct echo_runtime* services;   // The echo runtime services
} __attribute__((packed));

struct echo_regs {
    uint64_t rax, rbx, rcx, rdx, rsp, rbp, rdi, rsi, r8, r9, r10, r11, r12, r13, r14, r15, rip;
    uint64_t cs, ds, es, fs, gs, ss;
    uint64_t gdtr, idtr;
} __attribute__((packed));

struct echo_stackframe {
    uint64_t addr;

#define ECHO_STACKFRAME_NAME_LEN 24
    char name[ECHO_STACKFRAME_NAME_LEN];

    struct echo_stackframe* next;
} __attribute__((packed));

struct echo_table_hdr {
    uint64_t utid;

#define ECHO_TABLE_HDR_NAME_LEN 8
    char name[ECHO_TABLE_HDR_NAME_LEN];

    uint64_t len;
} __attribute__((packed));

#define ECHO_VAR_NAME_LEN 16

struct echo_var {
    uint64_t uvid;      // Unique variable ID
    char name[ECHO_VAR_NAME_LEN];

    uint64_t ctime;     // Creation time

    uint64_t contents;
} __attribute__((packed));

#define ECHO_SNAPSHOT_NAME_LEN 16

struct echo_snapshot {
    uint64_t usid;  // Unique snapshot ID

    char name[ECHO_SNAPSHOT_NAME_LEN];

    uint64_t ctime; // Creation time

    struct echo_regs* regs;
    struct echo_frames* stacktrace;
    struct echo_tables* k_state;
} __attribute__((packed));

struct echo_runtime {
    /* Hypervisor Services */

    int swap_kernel();

    int exit_kernel();

    int install_handle(uint64_t type,
                        void (*handler)(struct echo_runtime*));

    int uninstall_handle(uint64_t type);

    int set_flag(uint64_t flag);

    int clear_flag(uint64_t flag);

    int get_flag(uint64_t flag);

    int set_kernel_state(struct echo_tbl_hdr* head);

    int get_kernel_state(struct echo_tbl_hdr* ret);

    /* Variable Services */

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

    /* Snapshot Services */

    int echo_create_snapshot(char* name);

    int echo_preserve_snapshot(char* name);

    int echo_get_snapshot(char* name, struct echo_snapshot* ret);

    int echo_delete_snapshot(char* name);

    int echo_rename_snapshot(char* name, char* new_name);
} __attribute__((packed));

#endif
