obj-m+=rtdm_gpio_drv.o

#KERNEL_PATH:= #some absolute path if you wish to have#
         
all: build info

info:
	modinfo rtdm_gpio_drv.ko

build:
	make -C $(KERNEL_PATH) M=$(PWD) modules
clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
	@rm -f *.o
	@rm -f *.o.*
