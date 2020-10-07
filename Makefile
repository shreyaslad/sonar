default: all

C_SOURCES = $(shell find . -type f -name '*.c' | grep -v "build/slate")
H_SOURCES = $(shell find . -type f -name '*.h' | grep -v "build/slate")
A_SOURCES = $(shell find . -type f -name '*.asm' | grep -v "build/slate")

OBJ = ${C_SOURCES:.c=.o} ${A_SOURCES:.asm=.o}

ARCH = x86_64

CC = clang
LD = gcc
AS = nasm

SRC_DIR = ./src
BUILD_DIR = ./build

SLATE_DIR = ${BUILD_DIR}/slate
SLATE_KNL_TARGET = ${SLATE_DIR}/build/kslate.elf

KNL_TARGET = ${BUILD_DIR}/ksonar.elf

SONAR_IMG_TARGET = ${BUILD_DIR}/sonar.img
SONAR_MNT_TARGET = ${BUILD_DIR}/sonar_image

LOG = ${BUILD_DIR}/dump.log

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
		-I${SRC_DIR} \
		-I${SRC_DIR}/lib \
		-fno-pic \
		-mno-red-zone \
		-mno-sse \
		-mno-sse2 \
		-fasm-blocks \
		#-fsanitize=undefined \

QEMUFLAGS =	\
		-m 3G \
		-boot menu=on \
		-hda ${SONAR_IMG_TARGET} \
		-smp cpus=4	\
		-machine q35 \
		-name slate	\

O_LEVEL = \
		2

LDFLAGS = \
		-no-pie	\
		-ffreestanding \
		-O${O_LEVEL} \
		-nostdlib \
		-z max-page-size=0x1000 \
		-T ${SRC_DIR}/boot/linker.ld \

all: ci run

ci: clean SONAR_IMG_TARGET

run:
	qemu-system-x86_64 ${QEMUFLAGS} -serial stdio -cpu host -enable-kvm | tee "${LOG}"

debug: ci
	qemu-system-x86_64 ${QEMUFLAGS} -monitor stdio -d int -no-shutdown -no-reboot | tee "${LOG}"

SONAR_IMG_TARGET: SLATE_KNL_TARGET
	mkdir ${BUILD_DIR}/objects
	mkdir ${SONAR_MNT_TARGET}
	dd if=/dev/zero bs=1M count=0 seek=128 of=${SONAR_IMG_TARGET}
	parted -s ${SONAR_IMG_TARGET} mklabel msdos
	parted -s ${SONAR_IMG_TARGET} mkpart primary 1 100%
	make $(FS)
	sudo ${SRC_DIR}/boot/limine-install ${SRC_DIR}/boot/limine.bin ${SONAR_IMG_TARGET}
	find . -type f -name '*.o' -exec mv {} ${BUILD_DIR}/objects \;

SLATE_KNL_TARGET:
	git clone https://github.com/0xqoob/slate ${BUILD_DIR}/slate
	make -C ${BUILD_DIR}/slate all

ifndef $(FS)
FS := ext2
endif

ext2: ${KNL_TARGET}
	sudo losetup -Pf --show ${SONAR_IMG_TARGET} > ${BUILD_DIR}/loopback_dev
	sudo mkfs.ext2 `cat ${BUILD_DIR}/loopback_dev`p1
	sudo mount `cat ${BUILD_DIR}/loopback_dev`p1 ${SONAR_MNT_TARGET}
	sudo mkdir ${SONAR_MNT_TARGET}/boot/
	sudo cp ${KNL_TARGET} ${SONAR_MNT_TARGET}/boot/
	sudo cp ${SRC_DIR}/boot/limine.cfg ${SONAR_MNT_TARGET}/boot/
	sudo cp ${SLATE_KNL_TARGET} ${SONAR_MNT_TARGET}/boot/
	sync
	sudo umount ${SONAR_MNT_TARGET}
	sudo losetup -d `cat ${BUILD_DIR}/loopback_dev`
	
${KNL_TARGET}: ${OBJ} symlist
	${LD} ${LDFLAGS} ${OBJ} ${SRC_DIR}/sys/symlist.o -o $@
	sh gensyms.sh
	${CC} -x c ${CFLAGS} -c ${SRC_DIR}/sys/symlist.gen -o ${SRC_DIR}/sys/symlist.o
	${LD} ${LDFLAGS} ${OBJ} ${SRC_DIR}/sys/symlist.o -o $@

symlist:
	echo '#include <sys/symlist.h>' > ${SRC_DIR}/sys/symlist.gen
	echo 'struct symlist_t symlist[] = {{0xffffffffffffffff,""}};' >> ${SRC_DIR}/sys/symlist.gen
	${CC} -x c ${CFLAGS} -c ${SRC_DIR}/sys/symlist.gen -o ${SRC_DIR}/sys/symlist.o

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm -f elf64 -F dwarf -g -o $@ $<

clean:
	rm -rf ${BUILD_DIR}
	find . -type f -name "*.o" -delete
