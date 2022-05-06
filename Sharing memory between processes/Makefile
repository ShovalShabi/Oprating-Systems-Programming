CC=gcc
OBJS=primecount.o
ALL=primecount
DEBUG = -g
CFLAGS= -std=c99 -Wall -Werror $(DEBUG)

$(ALL):$(OBJS)
	$(CC) primecount.o -lm -o primecount
	
primecount.o: primecount.c

clean:
	rm -f $(OBJS) $(ALL) 
