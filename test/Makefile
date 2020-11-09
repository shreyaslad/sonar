default: all

.PHONY: all

ARCH = x86_64

CC = clang
LD = gcc
AS = nasm

BUILD_DIR = ../build
TEST_DIR = ./

TEST_BUILD_DIR = ${BUILD_DIR}/test
TEST_KNL_TARGET = ${TEST_BUILD_DIR}/test.elf

CFLAGS = \
		-target ${ARCH}-unknown-none \
		-ggdb \
		-nostdlib \
		-fno-stack-protector \
		-nostartfiles \
		-nodefaultlibs \
		-Wall \
		-Wextra	\
		-Wpedantic \
		-ffreestanding \
		-std=gnu11 \
		-mcmodel=kernel	\
		-fno-pic \
		-mno-red-zone \
		-mno-sse \
		-mno-sse2 \
		-fasm-blocks \
		#-fsanitize=undefined \

O_LEVEL = \
		2 \

LDFLAGS = \
		-no-pie	\
		-ffreestanding \
		-O${O_LEVEL} \
		-nostdlib \
		-z max-page-size=0x1000 \
		-T linker.ld \

C_SOURCES = $(shell find . -type f -name '*.c')
H_SOURCES = $(shell find . -type f -name '*.h')

OBJ = ${C_SOURCES:.c=.o}

all: ${TEST_KNL_TARGET}

${TEST_KNL_TARGET}: ${OBJ}
	${LD} ${LDFLAGS} ${OBJ} -o $@

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
