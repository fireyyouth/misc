KSRC ?= /lib/modules/$(shell uname -r)/build

KBUILD_EXTRA_SYMBOLS := $(KSRC)/Module.symvers

obj-m += keylogger.o

all:
	make -C $(KSRC) M=$(PWD) modules

clean:
	make -C $(KSRC) M=$(PWD) clean
