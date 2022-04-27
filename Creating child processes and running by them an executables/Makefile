CC=gcc
OBJS=lexcmp.o lencmp.o digcmp.o loopcmp.o
ALL=lexcmp lencmp digcmp loopcmp
DEBUG = -g
CFLAGS= -std=c99 -Wall -Werror $(DEBUG)

$(ALL):$(OBJS)
	$(CC) lexcmp.o -o lexcmp
	$(CC) lencmp.o -o lencmp
	$(CC) digcmp.o -o digcmp
	$(CC) loopcmp.o -o loopcmp
	
lexcmp.o: lexcmp.c

lencmp.o: lencmp.c 

digcmp.o: digcmp.c

loopcmp.o: loopcmp.c

clean:
	rm -f $(OBJS) $(ALL) 

