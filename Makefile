default: all

.PHONY: \
	all \
	build \
	run \
	debug \
	sonar \
	kernel \
	clean \
	limine

###########
# Tooling #
###########

export \
	ARCH \
	CC \
	LD \
	AS

ARCH = x86_64

CC = clang
LD = gcc
AS = nasm

QFLAGS = \
	-name sonar \
	-m 2G \
	-boot menu=off \
	-smp cpus=4 \
	-cdrom ${SONAR_ISO_TARGET} \
	-device ahci,id=ahci \
	-drive if=none,id=disk,file=${SONAR_ISO_TARGET},format=raw \
	-machine q35

#########################
# Top Level Directories #
#########################

export \
	BOOT_DIR \
	SRC_DIR \
	BUILD_DIR

DIR = $(shell readlink -f .)

SRC_DIR = ${DIR}/src
BOOT_DIR = ${SRC_DIR}/boot
BUILD_DIR = ${DIR}/build

################
# Build Output #
################

export \
	SONAR_KERNEL_TARGET \
	SONAR_IMG_TARGET

SONAR_KERNEL_TARGET = ${BUILD_DIR}/sonar.elf
SONAR_MOUNT_TARGET = ${BUILD_DIR}/iso_root
SONAR_ISO_TARGET = ${BUILD_DIR}/sonar.iso

################
# Dependencies #
################

LIMINE_DIR = ${BUILD_DIR}/limine
LIMINE_REPO = https://github.com/limine-bootloader/limine
LIMINE_BRANCH = v4.x-branch-binary


all: build run

build:
	docker run \
		--rm \
		-v ${BUILD_DIR}:/sonar/build \
		$(shell docker build -q . || echo "Build failed")

run:
	qemu-system-${ARCH} ${QFLAGS} -serial stdio

debug:
	qemu-system-${ARCH} ${QFLAGS} -no-reboot -d int

#################
# Raw ISO Build #
#################

sonar: kernel limine
	mkdir -p ${SONAR_MOUNT_TARGET}

	cp -v \
		${SONAR_KERNEL_TARGET} \
		${BOOT_DIR}/limine.cfg \
		${LIMINE_DIR}/limine.sys \
		${LIMINE_DIR}/limine-cd.bin \
		${LIMINE_DIR}/limine-cd-efi.bin \
		${SONAR_MOUNT_TARGET}
	
	xorriso -as mkisofs \
		-b limine-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		${SONAR_MOUNT_TARGET} \
		-o ${SONAR_ISO_TARGET}

	${LIMINE_DIR}/limine-deploy ${SONAR_ISO_TARGET}

kernel:
	make -C ${SRC_DIR}

clean:
	rm -rf ${BUILD_DIR}/*


limine:
	cd ${BUILD_DIR}

	git clone ${LIMINE_REPO} --branch=${LIMINE_BRANCH} --depth=1 ${LIMINE_DIR} \
		|| echo "Limine already exists"
	make -C ${LIMINE_DIR}

	cd -
