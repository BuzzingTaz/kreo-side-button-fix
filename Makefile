# Path to the kernel build directory
KERNEL_DIR := /lib/modules/$(shell uname -r)

# Path to the kernel configuration generated during kernel build
AUTO_CONF := $(KERNEL_DIR)/build/include/config/auto.conf

# Default to 0 (Assume GCC)
LLVM := 0

# Detect if the kernel was built with Clang
# We check if auto.conf exists and contains CONFIG_CC_IS_CLANG=y
ifneq ($(wildcard $(AUTO_CONF)),)
    ifneq ($(shell grep "CONFIG_CC_IS_CLANG=y" $(AUTO_CONF)),)
        LLVM := 1
    endif
endif

obj-m += kreo_side_fixup.o

all: clean build install load

load:
	sudo depmod -a
	sudo modprobe kreo_side_fixup

install: build
	sudo mkdir -p $(KERNEL_DIR)/extras
	sudo cp -r kreo_side_fixup.ko $(KERNEL_DIR)/extras

build:
	make -C $(KERNEL_DIR)/build M=$(PWD) LLVM=$(LLVM) modules

clean:
	make -C $(KERNEL_DIR)/build M=$(PWD) LLVM=$(LLVM) clean
