obj-m := siT9531x.o 
siT9531x-objs += siT95317.o sysfs_sit9531x.o

#MY_CFLAGS += -g -DDEBUG 
#EXTRA_CFLAGS += -I ${PWD}/siT95317.h
#ccflags-y += ${MY_CFLAGS} ${EXTRA_CFLAGS}

#SRC := $(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

#modules_install:
#	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c *.mod
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
