CC := gcc
CFLAGS := -O2 -Wall -g -ansi -pedantic $(OPTFLAGS)
CFLAGS+= -MMD

SRCDIR := src
BUILDDIR := build
TARGET := bin/rbmap

SOURCES := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,%.c,$(SOURCES:.c=.o))
LIB := -Llib
INC := -Iinclude


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) $<


clean:
	rm -r $(BUILDDIR) $(TARGET) 

.PHONY: clean

test:
	$(CC) $(CFLAGS) test/test.c $(INC) $(LIB) -o bin/test

-include $(OBJECTS:%.o=%.d)
