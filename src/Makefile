default: all
.PHONY: all

CFLAGS = \
	-target ${ARCH}-unknown-none \
	-std=c11 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-pie \
	-fno-pic \
	-m64 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \
	-I.

LDFLAGS = \
	-O2 \
	-ffreestanding \
	-nostdlib \
	-no-pie \
	-z max-page-size=0x1000 \
	-T linker.ld

ASMFLAGS = \
	-f elf64 \
	-f dwarf \
	-g

C_SOURCES = $(shell find . -type f -name '*.c')
H_SOURCES = $(shell find . -type f -name '*.h')
ASM_SOURCES = $(shell find . -type f -name '*.asm')

OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

all: ${SONAR_KERNEL_TARGET}

${SONAR_KERNEL_TARGET}: ${OBJ}
	${LD} ${LDFLAGS} ${OBJ} -o $@

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	${AS} ${ASMFLAGS} -o $@ $<
