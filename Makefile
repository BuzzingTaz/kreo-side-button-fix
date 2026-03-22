# Path to the kernel build directory
KERNEL_DIR := /lib/modules/$(shell uname -r)

# Path to the kernel configuration generated during kernel build
AUTO_CONF := $(KERNEL_DIR)/build/include/config/auto.conf

# Setting LLVM=0 doesn't work; only checks if the variable is defined
LLVM_FLAG :=

# Detect if the kernel was built with Clang
# We check if auto.conf exists and contains CONFIG_CC_IS_CLANG=y
# Pass LLVM=1 only if kernel was built with clang
ifneq ($(wildcard $(AUTO_CONF)),)
    ifneq ($(shell grep "CONFIG_CC_IS_CLANG=y" $(AUTO_CONF)),)
	LLVM_FLAG := LLVM=1
    endif
endif

obj-m += kreo_side_fixup.o

all: build

build:
	make -C $(KERNEL_DIR)/build M=$(CURDIR) $(LLVM_FLAG) modules

install: build
	sudo mkdir -p $(KERNEL_DIR)/extras
	sudo cp -r kreo_side_fixup.ko $(KERNEL_DIR)/extras
	sudo depmod -a

load:
	sudo modprobe kreo_side_fixup

unload:
	sudo rmmod kreo_side_fixup

clean:
	make -C $(KERNEL_DIR)/build M=$(CURDIR) $(LLVM_FLAG) clean

.PHONY: all build load unload install clean
