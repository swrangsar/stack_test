SHELL = /bin/sh

CC = cc
RANLIB = ranlib

CFLAGS := -O2 -Wall -g -ansi -pedantic $(OPTFLAGS)
CFLAGS+= -MMD

srcdir = src
BUILDDIR := build
TARGET := lib/librbmap.a

SOURCES := $(shell find $(srcdir) -type f -name *.c)
OBJECTS := $(patsubst $(srcdir)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
LIB := -Llib -lrbmap
INC := -Iinclude

.PHONY: all
all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)
	$(RANLIB) $@

$(BUILDDIR)/%.o: $(srcdir)/%.c
	@if test ! -d $(BUILDDIR); then mkdir $(BUILDDIR); fi
	$(CC) -c $(CFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -r $(BUILDDIR) $(TARGET)

.PHONY: tester
tester:
	$(CC) $(CFLAGS) test/tester.c $(INC) $(LIB) -o bin/tester


-include $(OBJECTS:%.o=%.d)
