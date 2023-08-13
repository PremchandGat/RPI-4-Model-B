obj-m := c_timer.o

target_ARCH=arm64
target_CROSS_COMPILE=aarch64-linux-gnu-
target_kernel_src_path=/home/prem/Desktop/LinuxKernel-RPI4B/linux/
host_kernel_path=/lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(target_ARCH) CROSS_COMPILE=$(target_CROSS_COMPILE) -C  $(target_kernel_src_path) M=$(PWD) modules

clean:
	make ARCH=$(target_ARCH) CROSS_COMPILE=$(target_CROSS_COMPILE) -C  $(target_kernel_src_path) M=$(PWD) clean

host:
	make -C  $(host_kernel_path) M=$(PWD) modules

cleanHost:
	make -C  $(host_kernel_path) M=$(PWD) clean
