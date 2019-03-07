CARD     ?= 25
ROUTER    = peri
LOGIN     = kadi-zhuang
FILE      = ./TP3/modules/test.c
USER      = zhuang

all:

key_trans:
	ssh-copy-id -i ~/.ssh/id_rsa.pub -p 50$(CARD) pi@$(ROUTER)
	ssh -p 50$(CARD) pi@peri 'mkdir -p ~/kadi-zhuang'

co: 
	ssh -p 50$(CARD) pi@peri


up: 
	scp -P50$(CARD) $(FILE) pi@$(ROUTER):$(LOGIN)/


import_linux : /dsk/l1/misc/$(USER)
	# cp /users/enseig/zhuang/linux-rpi-3.18.y.tbz2 /dsk/l1/misc/$(USER)/linux-rpi-3.18.y.tbz2
	tar xjf /users/enseig/zhuang/linux-rpi-3.18.y.tbz2 -C /dsk/l1/misc/$(USER)

/dsk/l1/misc/$(USER) : 
	mkdir /dsk/l1/misc/$(USER)
	chmod 740 /dsk/l1/misc/$(USER)