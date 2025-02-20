obj-m := sit95317.o

MY_CFLAGS += -g -DDEBUG 
EXTRA_CFLAGS += -I ${PWD}/sit95317.h
ccflags-y += ${MY_CFLAGS} ${EXTRA_CFLAGS}

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
