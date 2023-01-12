#ifndef SERIAL_H
#define SERIAL_H

#include <sys/ports.h>
#include <lib/log.h>

void serial_write(char c);

void init_serial();

#endif
