CC=gcc
CFLAGS= -Wall -g -ansi -pedantic -c $(OPTFLAGS)
CFLAGS+= -MMD
LFLAGS= -Wall -g -ansi -pedantic


SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
HEADERS=$(wildcard *.h)

TARGET=rbtree

all: $(TARGET) $(HEADERS)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

%.c: %.h


clean:
	rm -f $(TARGET) *.o *.d

-include $(OBJS:%.o=%.d)
