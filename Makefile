CC=gcc
CFLAGS= -Wall -g -ansi -pedantic -c $(OPTFLAGS)
LFLAGS= -Wall -g -ansi -pedantic


SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
HEADERS=$(wildcard *.h)

TARGET=stack

all: $(TARGET) $(HEADERS)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(TARGET) $(OBJS)
