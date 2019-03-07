CARD 		    ?= 25
ROUTER          = peri
LOGIN           = kadi-zhuang
LAB             = lab3
USER    		= zhuang

FILE           ?= ledKZ2.ko
MODULE 			= ledKZ2


CROSSDIR        = /users/enseig/franck/peri
KERNELDIR       = /dsk/l1/misc/$(USER)/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean

upload:
	scp -P50$(CARD) $(FILE) pi@$(ROUTER):$(LOGIN)/

up_ins : insdev
	scp -P50$(CARD) < pi@$(ROUTER):$(LOGIN)/$<

up_rm : rmdev
	scp -P50$(CARD) < pi@$(ROUTER):$(LOGIN)/$<
