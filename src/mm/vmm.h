#ifndef VMM_H
#define VMM_H

#define RMFLAGS         0x000ffffffffff000
#define GETFLAGS        ~RMFLAGS
#define MAP_TABLEPRESENT    (1 << 0)
#define MAP_TABLEWRITE      (1 << 1)
#define MAP_TABLEUSER       (1 << 2)
#define MAP_TABLEHUGE       (1 << 7)

#define MAP_SUPERVISOR  0
#define MAP_USER        1

#endif