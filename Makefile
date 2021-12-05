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
SONAR_IMG_TARGET	= ${SONAR_BUILD_DIR}/sonar.iso
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
LIMINE_BRANCH	= v2.66.3

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
		-smp		cpus=4	\
		-cdrom $(SONAR_IMG_TARGET) \
		-device 	ahci,id=ahci	\
		-drive		if=none,id=disk,file=$(SONAR_IMG_TARGET),format=raw	\
		#-device		intel-iommu,aw-bits=48	\

QFLAGS_KVM =	\
				-enable-kvm	\
				-cpu host,+vmx	\
				-machine q35	\
				
all: dirs $(SONAR_IMG_TARGET)

run:
	qemu-system-x86_64 $(QFLAGS) -nographic

$(SONAR_IMG_TARGET): deps kernel
	rm -rf $(SONAR_MNT_TARGET)
	mkdir -p $(SONAR_MNT_TARGET)
	
	cp $(SONAR_KNL_TARGET) \
		$(SRC_DIR)/boot/limine.cfg $(LIMINE_DIR)/limine.sys $(LIMINE_DIR)/limine-cd.bin \
		$(LIMINE_DIR)/limine-eltorito-efi.bin $(SONAR_MNT_TARGET)/
	
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(SONAR_MNT_TARGET) -o $(SONAR_IMG_TARGET)
	
	$(LIMINE_DIR)/limine-install $(SONAR_IMG_TARGET)

dirs:
	mkdir -p $(SONAR_BUILD_DIR) $(DEPS_DIR)

kernel:
	make -C $(SRC_DIR)

deps:
	make -C $(BUILD_DIR)	
