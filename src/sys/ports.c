#include <sys/ports.h>

uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__("inb %%dx, %%al" : "=a"(ret) : "d"(port));
  return ret;
}

void outb(uint16_t port, uint8_t data) {
  __asm__("outb %%al, %%dx" : : "a"(data), "d"(port));
}

uint16_t inw(uint16_t port) {
  uint16_t ret;
  __asm__("inw %%dx, %%ax" : "=a"(ret) : "d"(port));
  return ret;
}

void outw(uint16_t port, uint16_t data) {
  __asm__("outw %%ax, %%dx" : : "a"(data), "d"(port));
}

uint32_t ind(uint16_t port) {
  uint32_t ret;
  __asm__("inl %%dx, %%eax" : "=a"(ret) : "d"(port));
  return ret;
}

void outd(uint16_t port, uint32_t data) {
  __asm__("outl %%eax, %%dx" : : "a"(data), "d"(port));
}