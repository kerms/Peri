CC=bcm2708hardfp-gcc
CFLAGS=-O2 -static -Wall
CARD= 25 #numero de port
NAME=KADI_ZHUANG
EXEC=lab1_ex1

all : $(EXEC)


lab1_ex1 : lab1_ex1.c
	$(CC) $(CFLAGS) $< -o $@


transfert : 
	scp -P 50$(CARD) lab1_ex1 pi@peri:$(NAME)

clean : 
	rm -f $(EXEC) *.o *.out *~ *#