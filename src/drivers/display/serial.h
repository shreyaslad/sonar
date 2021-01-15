#ifndef __DRIVERS__SERIAL_H__
#define __DRIVERS__SERIAL_H__

#include <sys/ports.h>

void serial_write(char c);

void init_serial();

#endif
