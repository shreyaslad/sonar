default: all

.PHONY: all \
		run \
		debug \
		limine \
		sonar-kernel \
		test-kernel \
		clean-all \
		clean-sonar \
		clean-test

ARCH = x86_64

CC = clang
LD = gcc
AS = nasm

SRC_DIR = ./src
TEST_DIR = ./test
BUILD_DIR = ./build

SONAR_BUILD_DIR = ${BUILD_DIR}/sonar
SONAR_KNL_TARGET = ${SONAR_BUILD_DIR}/ksonar.elf
SONAR_IMG_TARGET = ${SONAR_BUILD_DIR}/sonar.img
SONAR_MNT_TARGET = ${SONAR_BUILD_DIR}/sonar_image
SONAR_IMG_SIZE = 128

TEST_BUILD_DIR = ${BUILD_DIR}/test
TEST_KNL_TARGET = ${TEST_BUILD_DIR}/ktest.elf

LIMINE_DIR = ${BUILD_DIR}/limine
LIMINE_BRANCH = 0.6

LOG = ${SONAR_BUILD_DIR}/dump.log

QEMUFLAGS =	\
		-m 3G \
		-boot menu=on \
		-hda ${SONAR_IMG_TARGET} \
		-smp cpus=4	\
		-machine q35 \
		-name sonar	\

all: clean-sonar sonar run

run:
	qemu-system-${ARCH} ${QEMUFLAGS} -serial stdio --enable-kvm -cpu host,+vmx | tee "${LOG}"

debug:
	qemu-system-${ARCH} ${QEMUFLAGS} -s -S -no-shutdown -no-reboot
	gdb -ex "target remote localhost:1234" -ex "symbol-file ${SONAR_KNL_TARGET}"

limine:
	git clone https://github.com/limine-bootloader/limine ${LIMINE_DIR} --branch=v${LIMINE_BRANCH}
	make -C ${LIMINE_DIR} limine-install

sonar:
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

	find ${SRC_DIR} -type f -name '*.o' -exec mv {} ${SONAR_BUILD_DIR}/objects \;
	find ${TEST_DIR} -type f -name '*.o' -exec mv {} ${TEST_BUILD_DIR}/objects \;

sonar-kernel:
	make -C ${SRC_DIR} all

test-kernel:
	make -C ${TEST_DIR} all

clean-all:
	rm -rf ${BUILD_DIR}
	find . -type f -name "*.o" -delete

clean-sonar:
	rm -rf ${SONAR_BUILD_DIR}
	find ${SRC_DIR} -type f -name "*.o" -delete

clean-test:
	rm -rf ${TEST_BUILD_DIR}
	find ${TEST_SRC_DIR} -type f -name "*.o" -delete

clean-limine:
	rm -rf ${LIMINE_DIR}
