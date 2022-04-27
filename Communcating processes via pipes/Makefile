CC=gcc
OBJS=nicecmp.o loopcmp.o
ALL=nicecmp loopcmp
DEBUG = -g
CFLAGS= -std=c99 -Wall -Werror $(DEBUG)

$(ALL):$(OBJS)
	$(CC) nicecmp.o -o nicecmp
	$(CC) loopcmp.o -o loopcmp
	
nicecmp.o: nicecmp.c

loopcmp.o: loopcmp.c

clean:
	rm -f $(OBJS) $(ALL) 

