obj-m	:= pmu_el0_cycle_counter.o
KDIR	:= /lib/modules/4.14.0+/build
PWD	:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
