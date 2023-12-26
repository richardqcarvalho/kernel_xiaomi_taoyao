# Android Kernel compilation/common definitions

ifeq ($(KERNEL_DEFCONFIG),)
ifneq ($(TARGET_BOARD_AUTO),true)
     KERNEL_DEFCONFIG := vendor/taoyao-qgki-debug_defconfig
else
     KERNEL_DEFCONFIG := vendor/gen3auto-qgki-debug_defconfig
endif
endif

TARGET_KERNEL := $(TARGET_KERNEL_VERSION)
ifeq ($(TARGET_KERNEL_SOURCE),)
     TARGET_KERNEL_SOURCE := kernel/$(TARGET_KERNEL)
endif

SOURCE_ROOT := $(shell pwd)
DTC := $(HOST_OUT_EXECUTABLES)/dtc$(HOST_EXECUTABLE_SUFFIX)
UFDT_APPLY_OVERLAY := $(HOST_OUT_EXECUTABLES)/ufdt_apply_overlay$(HOST_EXECUTABLE_SUFFIX)
TARGET_KERNEL_MAKE_ENV := DTC_EXT=$(SOURCE_ROOT)/$(DTC)
TARGET_KERNEL_MAKE_ENV += DTC_OVERLAY_TEST_EXT=$(SOURCE_ROOT)/$(UFDT_APPLY_OVERLAY)
TARGET_KERNEL_MAKE_ENV += CONFIG_BUILD_ARM64_DT_OVERLAY=y
TARGET_KERNEL_MAKE_ENV += HOSTCC=$(SOURCE_ROOT)/prebuilts/clang/host/linux-x86/clang-r487747c/bin/clang
TARGET_KERNEL_MAKE_ENV += HOSTCXX=$(SOURCE_ROOT)/prebuilts/clang/host/linux-x86/clang-r487747c/bin/clang++
TARGET_KERNEL_MAKE_ENV += HOSTAR=$(SOURCE_ROOT)/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-ar
TARGET_KERNEL_MAKE_ENV += HOSTLD=$(SOURCE_ROOT)/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-ld
TARGET_KERNEL_MAKE_ENV += M4=$(SOURCE_ROOT)/prebuilts/build-tools/linux-x86/bin/m4
TARGET_KERNEL_MAKE_ENV += LEX=$(SOURCE_ROOT)/prebuilts/build-tools/linux-x86/bin/flex
TARGET_KERNEL_MAKE_ENV += YACC=$(SOURCE_ROOT)/prebuilts/build-tools/linux-x86/bin/bison
TARGET_KERNEL_MAKE_CFLAGS = "-I$(SOURCE_ROOT)/$(TARGET_KERNEL_SOURCE)/include/uapi -I/usr/include -I/usr/include/x86_64-linux-gnu -I$(SOURCE_ROOT)/$(TARGET_KERNEL_SOURCE)/include -L/usr/lib -L/usr/lib/x86_64-linux-gnu -fuse-ld=lld -D__ANDROID_COMMON_KERNEL__ -fPIC"
TARGET_KERNEL_MAKE_LDFLAGS = "-L/usr/lib -L/usr/lib/x86_64-linux-gnu -fuse-ld=lld"

ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_cuttlefish
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_db845c
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_exynos
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_exynosauto
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_fcnt
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_galaxy
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_goldfish
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_hikey960
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_imx
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_microsoft
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_oneplus
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_oplus
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_qcom
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_sony
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_sonywalkman
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_sunxi
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_unisoc
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_vivo
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_xiaomi
ADDITIONAL_KMI_SYMBOL_LISTS += android/abi_gki_aarch64_zebra

FILES += arch/arm64/boot/Image
FILES += arch/arm64/boot/Image.gz
FILES += Module.symvers
FILES += System.map
FILES += vmlinux

BUILD_CONFIG_VARS += ABI_DEFINITION=android/abi_gki_aarch64.xml
BUILD_CONFIG_VARS += ADDITIONAL_KMI_SYMBOL_LISTS=$(ADDITIONAL_KMI_SYMBOL_LISTS)
BUILD_CONFIG_VARS += BRANCH=5.4
BUILD_CONFIG_VARS += DO_NOT_STRIP_MODULES=1
BUILD_CONFIG_VARS += FILES=$(FILES)
BUILD_CONFIG_VARS += IN_KERNEL_MODULES=1
BUILD_CONFIG_VARS += KMI_ENFORCED=1
BUILD_CONFIG_VARS += KMI_GENERATION=2
BUILD_CONFIG_VARS += KMI_SYMBOL_LIST_ADD_ONLY=1
BUILD_CONFIG_VARS += KMI_SYMBOL_LIST_STRICT_MODE=1
BUILD_CONFIG_VARS += KMI_SYMBOL_LIST=android/abi_gki_aarch64
BUILD_CONFIG_VARS += STOP_SHIP_TRACEPRINTK=1
BUILD_CONFIG_VARS += TRIM_NONLISTED_KMI=1

KERNEL_LLVM_BIN := $(SOURCE_ROOT)/prebuilts/clang/host/linux-x86/clang-r487747c/bin/clang
KERNEL_AOSP_LLVM_BIN := $(SOURCE_ROOT)/prebuilts/clang/host/linux-x86/clang-r487747c/bin
KERNEL_AOSP_LLVM_CLANG := $(KERNEL_AOSP_LLVM_BIN)/clang
USE_KERNEL_AOSP_LLVM := $(shell test -f "$(KERNEL_AOSP_LLVM_CLANG)" && echo "true" || echo "false")

KERNEL_TARGET := $(strip $(INSTALLED_KERNEL_TARGET))
ifeq ($(KERNEL_TARGET),)
INSTALLED_KERNEL_TARGET := $(PRODUCT_OUT)/kernel
endif

ifneq ($(TARGET_KERNEL_APPEND_DTB), true)
$(info Using DTB Image)
INSTALLED_DTBIMAGE_TARGET := $(PRODUCT_OUT)/dtb.img
endif

TARGET_KERNEL_ARCH := $(strip $(TARGET_KERNEL_ARCH))
ifeq ($(TARGET_KERNEL_ARCH),)
KERNEL_ARCH := arm
else
KERNEL_ARCH := $(TARGET_KERNEL_ARCH)
endif

ifeq ($(shell echo $(KERNEL_DEFCONFIG) | grep vendor),)
KERNEL_DEFCONFIG := vendor/$(KERNEL_DEFCONFIG)
endif

# Force 32-bit binder IPC for 64bit kernel with 32bit userspace
ifeq ($(KERNEL_ARCH),arm64)
ifeq ($(TARGET_ARCH),arm)
KERNEL_CONFIG_OVERRIDE := CONFIG_ANDROID_BINDER_IPC_32BIT=y
endif
endif

KERNEL_CROSS_COMPILE := $(SOURCE_ROOT)/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-

ifeq ($(TARGET_PREBUILT_KERNEL),)

KERNEL_GCC_NOANDROID_CHK := $(shell (echo "int main() {return 0;}" | $(KERNEL_CROSS_COMPILE)gcc -E -mno-android - > /dev/null 2>&1 ; echo $$?))

CLANG_ARCH := aarch64-linux-gnu-

real_cc :=
ifeq ($(KERNEL_LLVM_SUPPORT),true)
  ifeq ($(KERNEL_SD_LLVM_SUPPORT), true)  #Using sd-llvm compiler
    ifeq ($(shell echo $(SDCLANG_PATH) | head -c 1),/)
       KERNEL_LLVM_BIN := $(SDCLANG_PATH)/clang
    else
       KERNEL_LLVM_BIN := $(shell pwd)/$(SDCLANG_PATH)/clang
    endif
    $(warning "Using sdllvm" $(KERNEL_LLVM_BIN))
  real_cc := REAL_CC=$(KERNEL_LLVM_BIN) CLANG_TRIPLE=aarch64-linux-gnu-
  else
    ifeq ($(USE_KERNEL_AOSP_LLVM), true)  #Using kernel aosp-llvm compiler
       KERNEL_LLVM_BIN := $(KERNEL_AOSP_LLVM_CLANG)
       $(warning "Using latest kernel aosp llvm" $(KERNEL_LLVM_BIN))
    else #Using platform aosp-llvm binaries
       KERNEL_LLVM_BIN := $(shell pwd)/$(CLANG)
       KERNEL_AOSP_LLVM_BIN := $(shell pwd)/$(shell (dirname $(CLANG)))
       $(warning "Not using latest aosp-llvm" $(KERNEL_LLVM_BIN))
    endif
  real_cc := REAL_CC=$(KERNEL_LLVM_BIN) CLANG_TRIPLE=$(CLANG_ARCH) AR=$(KERNEL_AOSP_LLVM_BIN)/llvm-ar LLVM_NM=$(KERNEL_AOSP_LLVM_BIN)/llvm-nm LD=$(KERNEL_AOSP_LLVM_BIN)/ld.lld NM=$(KERNEL_AOSP_LLVM_BIN)/llvm-nm OBJCOPY=$(KERNEL_AOSP_LLVM_BIN)/llvm-objcopy
  endif
else
ifeq ($(strip $(KERNEL_GCC_NOANDROID_CHK)),0)
KERNEL_CFLAGS := KCFLAGS=-mno-android
endif
endif

ifneq (,$(findstring gki,$(KERNEL_DEFCONFIG)))
$(info ###### GKI based platform ######)
ifneq "gki_defconfig" "$(KERNEL_DEFCONFIG)"
GKI_KERNEL ?= 1
endif
endif

BUILD_ROOT_LOC := ../../..
KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/kernel/$(TARGET_KERNEL)
KERNEL_SYMLINK := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
KERNEL_USR := $(KERNEL_SYMLINK)/usr
KERNEL_USR_TS := $(TARGET_OUT_INTERMEDIATES)/kernelusr.time

KERNEL_CONFIG := $(KERNEL_OUT)/.config

# Move MAKE_PATH here (cut from below), so that it's defined before first use.
# Without this the build fails due to android build system path tool
# restrictions.
MAKE_PATH := $(SOURCE_ROOT)/prebuilts/build-tools/linux-x86/bin/

ifeq ($(KERNEL_DEFCONFIG)$(wildcard $(KERNEL_CONFIG)),)
$(error Kernel configuration not defined, cannot build kernel)
else

ifeq ($(GKI_KERNEL),1)
GKI_PLATFORM_NAME := $(shell echo $(KERNEL_DEFCONFIG) | sed -r "s/(-gki_defconfig|-qgki_defconfig|-qgki-consolidate_defconfig|-qgki-debug_defconfig)$///")
GKI_PLATFORM_NAME := $(shell echo $(GKI_PLATFORM_NAME) | sed "s/vendor\///g")
TARGET_USES_UNCOMPRESSED_KERNEL := $(shell grep "CONFIG_BUILD_ARM64_UNCOMPRESSED_KERNEL=y" $(TARGET_KERNEL_SOURCE)/arch/arm64/configs/vendor/$(GKI_PLATFORM_NAME)_GKI.config)

else
TARGET_USES_UNCOMPRESSED_KERNEL ?= $(shell grep "CONFIG_BUILD_ARM64_UNCOMPRESSED_KERNEL=y" $(TARGET_KERNEL_SOURCE)/arch/$(KERNEL_ARCH)/configs/$(KERNEL_DEFCONFIG))
endif

# Generate the defconfig file from the fragments
_x := $(shell ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) $(real_cc) KERN_OUT=$(KERNEL_OUT) $(TARGET_KERNEL_MAKE_ENV) MAKE_PATH=$(MAKE_PATH) TARGET_BUILD_VARIANT=${TARGET_BUILD_VARIANT} $(TARGET_KERNEL_SOURCE)/scripts/gki/generate_defconfig.sh $(KERNEL_DEFCONFIG))

ifeq ($(TARGET_USES_UNCOMPRESSED_KERNEL),)
ifeq ($(KERNEL_ARCH),arm64)
TARGET_PREBUILT_INT_KERNEL := $(KERNEL_OUT)/arch/$(KERNEL_ARCH)/boot/Image.gz
else
TARGET_PREBUILT_INT_KERNEL := $(KERNEL_OUT)/arch/$(KERNEL_ARCH)/boot/zImage
endif
else
$(info Using uncompressed kernel)
TARGET_PREBUILT_INT_KERNEL := $(KERNEL_OUT)/arch/$(KERNEL_ARCH)/boot/Image
endif

ifeq ($(TARGET_KERNEL_APPEND_DTB), true)
$(info Using appended DTB)
TARGET_PREBUILT_INT_KERNEL := $(TARGET_PREBUILT_INT_KERNEL)-dtb
endif

KERNEL_HEADERS_INSTALL := $(KERNEL_OUT)/usr
KERNEL_MODULES_INSTALL ?= system
KERNEL_MODULES_OUT ?= $(PRODUCT_OUT)/$(KERNEL_MODULES_INSTALL)/lib/modules

TARGET_PREBUILT_KERNEL := $(TARGET_PREBUILT_INT_KERNEL)

endif
endif

# If the configuration is QGKI, build the GKI kernel as well
# The build system overrides INSTALLED_KERNEL_TARGET if BOARD_KERNEL_BINARIES is defined
ifeq ($(GKI_KERNEL),1)
  ifeq "$(KERNEL_DEFCONFIG)" "vendor/taoyao-qgki_defconfig"
    $(info Additional GKI images will be built)
    INSTALLED_KERNEL_TARGET := $(foreach k,$(BOARD_KERNEL_BINARIES), $(PRODUCT_OUT)/$(k))

    # Create new definitions for building an additional GKI kernel on the side
    GKI_INSTALLED_KERNEL_TARGET := $(PRODUCT_OUT)/kernel-gki
    GKI_KERNEL_DEFCONFIG := vendor/taoyao-gki_defconfig
    GKI_KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/kernel-gki/$(TARGET_KERNEL)
    GKI_KERNEL_MODULES_OUT := $(PRODUCT_OUT)/$(KERNEL_MODULES_INSTALL)/lib/modules/gki
    GKI_KERNEL_HEADERS_INSTALL := $(GKI_KERNEL_OUT)/usr
    GKI_TARGET_PREBUILT_INT_KERNEL := $(subst kernel,kernel-gki,$(TARGET_PREBUILT_INT_KERNEL))
    GKI_TARGET_PREBUILT_KERNEL := $(GKI_TARGET_PREBUILT_INT_KERNEL)
    GKI_TARGET_MODULES_DIR := $(TARGET_KERNEL_VERSION)-gki

    BOARD_KERNEL_MODULE_DIRS := $(GKI_TARGET_MODULES_DIR)
    BOARD_KERNEL-GKI_BOOTIMAGE_PARTITION_SIZE := 0x06000000

    # Generate the GKI defconfig
    _x := $(shell ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) $(real_cc) KERN_OUT=$(KERNEL_OUT) $(TARGET_KERNEL_MAKE_ENV) MAKE_PATH=$(MAKE_PATH) TARGET_BUILD_VARIANT=${TARGET_BUILD_VARIANT} $(TARGET_KERNEL_SOURCE)/scripts/gki/generate_defconfig.sh $(GKI_KERNEL_DEFCONFIG))
  endif
endif

# Archieve the DLKMs that goes into vendor.img and vendor-ramdisk.
# Also, make them dependent on the kernel compilation.
VENDOR_KERNEL_MODULES_ARCHIVE := vendor_modules.zip
BOARD_VENDOR_KERNEL_MODULES_ARCHIVE := $(KERNEL_MODULES_OUT)/$(VENDOR_KERNEL_MODULES_ARCHIVE)
$(BOARD_VENDOR_KERNEL_MODULES_ARCHIVE): $(TARGET_PREBUILT_KERNEL)

ifneq ($(GKI_INSTALLED_KERNEL_TARGET),)
BOARD_VENDOR_KERNEL_MODULES_ARCHIVE_$(GKI_TARGET_MODULES_DIR) := $(GKI_KERNEL_MODULES_OUT)/$(VENDOR_KERNEL_MODULES_ARCHIVE)
$(BOARD_VENDOR_KERNEL_MODULES_ARCHIVE_$(GKI_TARGET_MODULES_DIR)): $(GKI_TARGET_PREBUILT_KERNEL)
endif

BOARD_VENDOR_KERNEL_MODULES_$(GKI_TARGET_MODULES_DIR) = \
              $(foreach mod, $(BOARD_VENDOR_KERNEL_MODULES), \
                $(subst $(KERNEL_MODULES_OUT), $(GKI_KERNEL_MODULES_OUT), $(mod)))

$(warning VENDOR_RAMDISK_KERNEL_MODULES = $(VENDOR_RAMDISK_KERNEL_MODULES))

ifneq ($(VENDOR_RAMDISK_KERNEL_MODULES),)
VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE := vendor_ramdisk_modules.zip

ifeq ($(TARGET_BOARD_AUTO),true)
ifneq (, $(findstring -gki_defconfig, $(KERNEL_DEFCONFIG)))
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE := $(KERNEL_MODULES_OUT)/$(VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE)
$(BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE): $(TARGET_PREBUILT_KERNEL)
endif
else
ifeq "$(KERNEL_DEFCONFIG)" "vendor/taoyao-gki_defconfig"
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE := $(KERNEL_MODULES_OUT)/$(VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE)
$(BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE): $(TARGET_PREBUILT_KERNEL)
endif
endif

ifneq ($(GKI_INSTALLED_KERNEL_TARGET),)
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE_$(GKI_TARGET_MODULES_DIR) := $(GKI_KERNEL_MODULES_OUT)/$(VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE)
$(BOARD_VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE_$(GKI_TARGET_MODULES_DIR)): $(GKI_TARGET_PREBUILT_KERNEL)
endif
endif

$(BOARD_VENDOR_RAMDISK_KERNEL_MODULES): $(TARGET_PREBUILT_KERNEL)

# Add RTIC DTB to dtb.img if RTIC MPGen is enabled.
# Note: unfortunately we can't define RTIC DTS + DTB rule here as the
# following variable/ tools (needed for DTS generation)
# are missing - DTB_OBJS, OBJDUMP, KCONFIG_CONFIG, CC, DTC_FLAGS (the only available is DTC).
# The existing RTIC kernel integration in scripts/link-vmlinux.sh generates RTIC MP DTS
# that will be compiled with optional rule below.
# To be safe, we check for MPGen enable.
ifdef RTIC_MPGEN
RTIC_DTB := $(KERNEL_SYMLINK)/rtic_mp.dtb
endif

# Helper functions

# Build the kernel
# $(1): KERNEL_DEFCONFIG to build for
# $(2): KERNEL_OUT directory
# $(3): KERNEL_MODULES_OUT directory
# $(4): KERNEL_HEADERS_INSTALL directory
# $(5): HEADERS_INSTALL; If 1, the call would just generate the headers and quit
# $(6): TARGET_PREBUILT_INT_KERNEL: The location to the kernel's binary format (Image, zImage, and so on)
define build-kernel
	KERNEL_DIR=$(TARGET_KERNEL_SOURCE) \
	DEFCONFIG=$(1) \
	OUT_DIR=$(2) \
	MAKE_PATH=$(MAKE_PATH) \
	ARCH=$(KERNEL_ARCH) \
	CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) \
	KERNEL_MODULES_OUT=$(3) \
	KERNEL_HEADERS_INSTALL=$(4) \
	HEADERS_INSTALL=$(5) \
	TARGET_PREBUILT_INT_KERNEL=$(6) \
	TARGET_INCLUDES=$(TARGET_KERNEL_MAKE_CFLAGS) \
	TARGET_LINCLUDES=$(TARGET_KERNEL_MAKE_LDFLAGS) \
	VENDOR_KERNEL_MODULES_ARCHIVE=$(VENDOR_KERNEL_MODULES_ARCHIVE) \
	VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE=$(VENDOR_RAMDISK_KERNEL_MODULES_ARCHIVE) \
	VENDOR_RAMDISK_KERNEL_MODULES="$(VENDOR_RAMDISK_KERNEL_MODULES)" \
	TARGET_PRODUCT=taoyao \
	kernel/xiaomi/taoyao/build/buildkernel.sh \
	$(real_cc) \
	$(TARGET_KERNEL_MAKE_ENV) \
	$(BUILD_CONFIG_VARS)
endef

# Android Kernel make rules
# Create kernelusr.time file and use its timestamp later to modify the TS of $(KERNEL_USR). \
# this will ensure in subsequent builds, i.e. no-op incremental builds, modules depends on $(KERNEL_USR) \
# will not get recompiled.

$(KERNEL_HEADERS_INSTALL): $(DTC) $(UFDT_APPLY_OVERLAY) | $(KERNEL_OUT)
	$(call build-kernel,$(KERNEL_DEFCONFIG),$(KERNEL_OUT),$(KERNEL_MODULES_OUT),$(KERNEL_HEADERS_INSTALL),1,$(TARGET_PREBUILT_INT_KERNEL))
	touch $(KERNEL_USR_TS)

$(KERNEL_OUT):
	mkdir -p $(KERNEL_OUT)

$(GKI_KERNEL_OUT):
	mkdir -p $(GKI_KERNEL_OUT)

$(KERNEL_USR): | $(KERNEL_HEADERS_INSTALL)
	if [ -d "$(KERNEL_SYMLINK)" ] && [ ! -L "$(KERNEL_SYMLINK)" ]; then \
	rm -rf $(KERNEL_SYMLINK); \
	ln -s kernel/$(TARGET_KERNEL) $(KERNEL_SYMLINK); \
	fi

$(TARGET_PREBUILT_KERNEL): $(KERNEL_OUT) $(DTC) $(KERNEL_USR)
	echo "Building the requested kernel.."; \
	$(call build-kernel,$(KERNEL_DEFCONFIG),$(KERNEL_OUT),$(KERNEL_MODULES_OUT),$(KERNEL_HEADERS_INSTALL),0,$(TARGET_PREBUILT_INT_KERNEL))

$(GKI_TARGET_PREBUILT_KERNEL): $(DTC) $(UFDT_APPLY_OVERLAY) $(GKI_KERNEL_OUT)
	echo "Building GKI kernel.."; \
	$(call build-kernel,$(GKI_KERNEL_DEFCONFIG),$(GKI_KERNEL_OUT),$(GKI_KERNEL_MODULES_OUT),$(GKI_KERNEL_HEADERS_INSTALL),0,$(GKI_TARGET_PREBUILT_INT_KERNEL))

$(INSTALLED_KERNEL_TARGET): $(TARGET_PREBUILT_KERNEL) $(GKI_TARGET_PREBUILT_KERNEL)
	cp $(TARGET_PREBUILT_KERNEL) $(PRODUCT_OUT)/kernel
	if [ ! -z "$(GKI_TARGET_PREBUILT_KERNEL)" ]; then \
		cp $(GKI_TARGET_PREBUILT_KERNEL) $(PRODUCT_OUT)/kernel-gki; \
	fi
	touch $(KERNEL_USR) -r $(KERNEL_USR_TS)

# RTIC DTS to DTB (if MPGen enabled;
# and make sure we don't break the build if rtic_mp.dts missing)
$(RTIC_DTB): $(INSTALLED_KERNEL_TARGET)
	stat $(KERNEL_SYMLINK)/rtic_mp.dts 2>/dev/null >&2 && \
	$(DTC) -O dtb -o $(RTIC_DTB) -b 1 $(DTC_FLAGS) $(KERNEL_SYMLINK)/rtic_mp.dts || \
	touch $(RTIC_DTB)
P_NAME="$(TARGET_BOARD_SUFFIX)"
VENDOR_DTB_OBJECTS ?= arch/$(KERNEL_ARCH)/boot/dts/vendor/qcom/*.dtb
ifeq ($(P_NAME),"_gvmgh")
# Creating a dtb.img once the kernel is compiled if TARGET_KERNEL_APPEND_DTB is set to be false
$(INSTALLED_DTBIMAGE_TARGET): $(INSTALLED_KERNEL_TARGET) $(RTIC_DTB)
	cat $(KERNEL_OUT)/$(VENDOR_DTB_OBJECTS) $(RTIC_DTB) > $@ \
	$(shell device/qcom/msmnile_gvmgh/generate_linux_image.sh $(KERNEL_OUT)/arch/arm64/boot/Image) \

else
# Creating a dtb.img once the kernel is compiled if TARGET_KERNEL_APPEND_DTB is set to be false
$(INSTALLED_DTBIMAGE_TARGET): $(INSTALLED_KERNEL_TARGET) $(RTIC_DTB)
	cat $(KERNEL_OUT)/$(VENDOR_DTB_OBJECTS) $(RTIC_DTB) > $@
endif

ifeq ($(TARGET_DUMMY_VENDOR_BOOT), true)
	$(shell dd if=/dev/zero of=$(PRODUCT_OUT)/vendor_boot.img bs=1M count=96)
endif
