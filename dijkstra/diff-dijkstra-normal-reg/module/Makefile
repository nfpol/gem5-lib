obj-m	:= pmu_el0.o
#KDIR	:= /lib/modules/$(shell uname -r)/build
KDIR    := /lib/modules/4.14.0+/build
PWD	:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
