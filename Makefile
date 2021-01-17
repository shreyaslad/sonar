default: all

.PHONY:	all	\
		build	\
		run	\
		debug	\
		deps	\
		sonar-kernel	\
		test-kernel	\
		clean-all	\
		clean-sonar	\
		clean-test	\

###########
# Tooling #
###########

export	\
		ARCH	\
		CC	\
		LD	\
		AS	\

ARCH	= x86_64

CC		= clang
LD		= gcc
AS		= nasm

QEMU	= qemu-system-${ARCH}
GDB		= gdb

#########################
# Top Level Directories #
#########################

export	\
		DIR	\
		SRC_DIR	\
		TEST_DIR	\
		BUILD_DIR	\

DIR			= $(shell readlink -f .)

SRC_DIR		= ${DIR}/src
TEST_DIR	= ${DIR}/test
BUILD_DIR	= ${DIR}/build

###########
# Kernels #
###########

export	\
		SONAR_BUILD_DIR	\
		SONAR_KNL_TARGET	\
		SONAR_IMG_TARGET	\
		SONAR_MNT_TARGET	\
		TEST_BUILD_DIR	\
		TEST_KNL_TARGET	\

KERNEL_DIR			= ${BUILD_DIR}/kernels

SONAR_BUILD_DIR		= ${KERNEL_DIR}/sonar
SONAR_KNL_TARGET	= ${SONAR_BUILD_DIR}/ksonar.elf
SONAR_IMG_TARGET	= ${SONAR_BUILD_DIR}/sonar.img
SONAR_MNT_TARGET	= ${SONAR_BUILD_DIR}/sonar_image
SONAR_IMG_SIZE		= 128

TEST_BUILD_DIR		= ${KERNEL_DIR}/test
TEST_KNL_TARGET		= ${TEST_BUILD_DIR}/ktest.elf

################
# Dependencies #
################

export	\
		DEPS_DIR	\
		LIMINE_DIR	\
		LIMINE_REPO	\
		LIMINE_BRANCH	\
		BDDISASM_DIR	\
		BDDISASM_REPO	\
		BDDISASM_BRANCH	\
		LAI_DIR	\
		LAI_HEADER_DIR	\
		LAI_COMPILE_DIR	\
		LAI_REPO	\
		LAI_BRANCH	\

DEPS_DIR		= ${BUILD_DIR}/deps

LIMINE_DIR		= ${DEPS_DIR}/limine
LIMINE_REPO 	= https://github.com/limine-bootloader/limine
LIMINE_BRANCH	= v1.0

BDDISASM_DIR	= ${DEPS_DIR}/bddisasm
BDDISASM_REPO	= https://github.com/bitdefender/bddisasm
BDDISASM_BRANCH	= v1.31.2

LAI_HEADER_DIR	= ${SRC_DIR}/ospm/acpi
LAI_COMPILE_DIR	= ${SRC_DIR}/thirdparty/lai
LAI_DIR			= ${DEPS_DIR}/lai
LAI_REPO		= https://github.com/managarm/lai
LAI_BRANCH		= master

#######################################
# Compilation/Linkage/Execution Flags #
#######################################

export	\
		CFLAGS	\
		O_LEVEL	\
		LDFLAGS	\

CFLAGS	=	\
		-target	${ARCH}-unknown-none	\
		-ggdb	\
		-nostdlib	\
		-fno-stack-protector	\
		-nostartfiles	\
		-nodefaultlibs	\
		-Wall	\
		-Wextra	\
		-Wpedantic	\
		-ffreestanding	\
		-std=gnu11	\
		-mcmodel=kernel	\
		-fno-pic	\
		-mno-red-zone	\
		-mno-sse	\
		-mno-sse2	\
		-fasm-blocks	\
		#-fsanitize=undefined	\

O_LEVEL	=	\
		2	\

LDFLAGS	=	\
		-no-pie	\
		-ffreestanding	\
		-O${O_LEVEL}	\
		-nostdlib	\
		-z max-page-size=0x1000	\
		-T linker.ld	\

QFLAGS	=	\
		-name 		sonar	\
		-m			3G	\
		-boot		menu=off	\
		-hda		${SONAR_IMG_TARGET}	\
		-smp		cpus=4	\
		#-numa		node,cpus=0,nodeid=0	\
		#-numa		node,cpus=1,nodeid=1	\
		#-numa		node,cpus=2,nodeid=2	\
		#-numa		node,cpus=3,nodeid=3	\
		-device		intel-iommu,aw-bits=48	\
		#-device		qemu-xhci	\
		#-device		id=foo,file=${SONAR_IMG_TARGET},format=raw,if=none	\
		#-device		usb-storage,drive=foo	\

QFLAGS_KVM =	\
				-enable-kvm	\
				-cpu		host,+vmx	\
				-machine	q35	\
				

LOG		= ${SONAR_BUILD_DIR}/qemu.log

all: clean-sonar sonar run

build: clean-sonar sonar

run:
	${QEMU} ${QFLAGS} ${QFLAGS_KVM} -serial stdio | tee "${LOG}"

monitor:
	${QEMU} ${QFLAGS} -d int -monitor stdio | tee "${LOG}"

debug:
	${QEMU} ${QFLAGS} ${QFLAGS_KVM} -s -S -no-shutdown -no-reboot
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ${SONAR_KNL_TARGET}"

sonar:
	if [ ! -d '${DEPS_DIR}' ]; then	\
	make deps;	\
	fi	\

	if [ -d '${LAI_DIR}' ]; then \
	mv ${LAI_DIR} ${LAI_COMPILE_DIR}; \
	fi \

	mkdir -p ${SONAR_BUILD_DIR}/objects
	mkdir -p ${TEST_BUILD_DIR}/objects
	mkdir ${SONAR_MNT_TARGET}
	dd if=/dev/zero bs=1M count=0 seek=${SONAR_IMG_SIZE} of=${SONAR_IMG_TARGET}
	parted -s ${SONAR_IMG_TARGET} mklabel msdos
	parted -s ${SONAR_IMG_TARGET} mkpart primary 1 100%

	make sonar-kernel
	make test-kernel

	sudo losetup -Pf --show ${SONAR_IMG_TARGET} > ${SONAR_BUILD_DIR}/loopback_dev
	sudo mkfs.ext2 `cat ${SONAR_BUILD_DIR}/loopback_dev`p1
	sudo mount `cat ${SONAR_BUILD_DIR}/loopback_dev`p1 ${SONAR_MNT_TARGET}
	sudo mkdir ${SONAR_MNT_TARGET}/boot/
	sudo cp ${SONAR_KNL_TARGET} ${SONAR_MNT_TARGET}/boot/
	sudo cp ${SRC_DIR}/boot/limine.cfg ${SONAR_MNT_TARGET}/boot/
	sudo cp ${SRC_DIR}/boot/limine_background.bmp ${SONAR_MNT_TARGET}
	sudo cp ${SONAR_KNL_TARGET} ${SONAR_MNT_TARGET}/boot/
	sync
	sudo umount ${SONAR_MNT_TARGET}
	sudo losetup -d `cat ${SONAR_BUILD_DIR}/loopback_dev`

	sudo ${LIMINE_DIR}/limine-install ${LIMINE_DIR}/limine.bin ${SONAR_IMG_TARGET}

	mv ${LAI_COMPILE_DIR} ${LAI_DIR}

	find ${SRC_DIR} -type f -name '*.o' -exec mv {} ${SONAR_BUILD_DIR}/objects \;
	find ${TEST_DIR} -type f -name '*.o' -exec mv {} ${TEST_BUILD_DIR}/objects \;

sonar-kernel:
	make -C ${SRC_DIR} all

test-kernel:
	make -C ${TEST_DIR} all

deps:
	make -C ${BUILD_DIR}

clean-all: clean-kernels clean-deps
	find . -type f -name "*.o" -delete

clean-sonar:
	rm -rf ${SONAR_BUILD_DIR}
	find ${SRC_DIR} -type f -name "*.o" -delete

clean-test:
	rm -rf ${TEST_BUILD_DIR}
	find ${TEST_SRC_DIR} -type f -name "*.o" -delete

clean-limine:
	rm -rf ${LIMINE_DIR}

clean-bddisasm:
	rm -rf ${BDDISASM_DIR}

clean-lai:
	rm -rf ${LAI_COMPILE_DIR} ${LAI_DIR}
	rm -rf ${LAI_HEADER_DIR}/acpispec ${LAI_HEADER_DIR}/lai

clean-kernels:
	rm -rf ${KERNEL_DIR}

clean-deps: clean-limine clean-bddisasm clean-lai
	make -C ${BUILD_DIR} clean
