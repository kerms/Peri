# changer les valeurs par défaut de RPI et DIR
RPI?=26
DIR?=kadi-zhuang
APP=lcd_user
USER?= ${USER}
MODULE ?= lcd_driverkz

#CFLAGS=-Wall -Wfatal-errors -O2
#LDFLAGS=-static -L$(LIB) -lgpio

CROSSDIR        = /users/enseig/franck/peri
KERNELDIR       = /dsk/l1/misc/$(USER)/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

        
obj-m           += $(MODULE).o
all:  #$(APP).x #upload
	@echo $(PS1)

default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

$(APP).x: $(APP).c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static



upload: 
	scp -P50$(RPI) $(APP).x pi@peri:$(DIR)

clean:
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -f *.o *.x *~
