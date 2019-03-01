CARD_NUMB       = 25
ROUTER          = peri
LOGIN           = KADI-ZHUANG
LAB             = lab3

MODULE          = ledKZ2

CROSSDIR        = /users/enseig/franck/peri
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean

upload:;        scp -P50$(CARD_NUMB) $(MODULE).ko pi@$(ROUTER):$(LOGIN)/$(MODULE).ko

up_ins : insdev
	scp -P50$(CARD_NUMB) $< pi@$(ROUTER):$(LOGIN)/$<

up_rm : rmdev
	scp -P50$(CARD_NUMB) $< pi@$(ROUTER):$(LOGIN)/$<
