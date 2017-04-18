obj-m+=rtdm_gpio_drv.o

KERNEL_PATH:= /home/mind/work/ostro-os-xt/ostro-os-xt/build/tmp-glibc/work/corei7-64-intel-common-ostro-linux/linux-yocto/4.4.36+gitAUTOINC+b846fc6436_5ec33015d3-r0/linux-corei7-64-intel-common-standard-build
         
all: build info

info:
	modinfo rtdm_gpio_drv.ko

build:
	make -C $(KERNEL_PATH) M=$(PWD) modules
clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
	@rm -f *.o
	@rm -f *.o.*

