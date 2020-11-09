default: all

.PHONY: all \
		symlist \

ARCH = x86_64

CC = clang
LD = gcc
AS = nasm

SRC_DIR = ./
BUILD_DIR = ../build

SONAR_BUILD_DIR = ${BUILD_DIR}/sonar
SONAR_KNL_TARGET = ${SONAR_BUILD_DIR}/ksonar.elf
SONAR_IMG_TARGET = ${SONAR_BUILD_DIR}/sonar.img
SONAR_MNT_TARGET = ${SONAR_BUILD_DIR}/sonar_image

C_SOURCES = $(shell find . -type f -name '*.c')
H_SOURCES = $(shell find . -type f -name '*.h')
A_SOURCES = $(shell find . -type f -name '*.asm')

OBJ = ${C_SOURCES:.c=.o} ${A_SOURCES:.asm=.o}

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
		-I. \
        -Ilib \
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

all: ${SONAR_KNL_TARGET}

${SONAR_KNL_TARGET}: ${OBJ} symlist
	${LD} ${LDFLAGS} ${OBJ} sys/symlist.o -o $@
	sh gensyms.sh
	${CC} -x c ${CFLAGS} -c sys/symlist.gen -o sys/symlist.o
	${LD} ${LDFLAGS} ${OBJ} sys/symlist.o -o $@

symlist:
	echo '#include <sys/symlist.h>' > sys/symlist.gen
	echo 'struct symlist_t symlist[] = {{0xffffffffffffffff,""}};' >> sys/symlist.gen
	${CC} -x c ${CFLAGS} -c sys/symlist.gen -o sys/symlist.o

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm -f elf64 -F dwarf -g -o $@ $<
