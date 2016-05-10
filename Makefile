SHELL = /bin/sh

CC = cc
RANLIB = ranlib

CFLAGS = -O2 -Wall -g -ansi -pedantic $(OPTFLAGS)
CFLAGS += -MMD

srcdir = src
BUILDDIR = build
TARGET = lib/librbmap.a

SOURCES = $(shell find $(srcdir) -type f -name *.c)
OBJECTS = $(patsubst $(srcdir)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
LDLIBS = -Llib
LDFLAGS = -lrbmap
INC = -Iinclude

.PHONY: all
all: $(TARGET)


$(TARGET): $(OBJECTS)
	@if test ! -d lib; then mkdir lib; fi
	$(AR) rcs $@ $(OBJECTS)
	$(RANLIB) $@

$(BUILDDIR)/%.o: $(srcdir)/%.c
	@if test ! -d $(BUILDDIR); then mkdir $(BUILDDIR); fi
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	-rm -r $(BUILDDIR) $(TARGET)

.PHONY: tester
tester:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) test/tester.c $(INC) $(LDLIBS) -o bin/tester


-include $(OBJECTS:%.o=%.d)
