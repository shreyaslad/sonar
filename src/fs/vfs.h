#ifndef __FS__VFS_H__
#define __FS__VFS_H__

#include <stdint.h>
#include <stddef.h>
#include <stb_ds.h>

#undef __MODULE__
#define __MODULE__ "vfs"

#define DIRECTORY   0
#define FILE        1

struct vfs_node_t {
    char* name;
    char* path;
    int type;

    uint64_t perms;
    uint64_t size;

    uint64_t ctime;
    uint64_t itime;
    uint64_t atime;

    int (*read)(struct vfs_node_t* node, char* buffer, size_t bytes);
    int (*write)(struct vfs_node_t* node, const char* buffer, size_t bytes);
};

#endif
