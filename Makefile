SHELL = /bin/sh

CC = cc
RANLIB = ranlib
ARFLAGS = rcs

CFLAGS = -O2 -Wall -g -ansi -pedantic $(OPTFLAGS)
CFLAGS += -MMD

srcdir = src
BUILDDIR = build
TARGET = lib/librbmap.a


SRCS = $(shell find $(srcdir) -type f -name *.c)
OBJS = $(patsubst $(srcdir)/%.c,$(BUILDDIR)/%.o,$(SRCS))
AUX = $(srcdir) Makefile include test
LDLIBS = -Llib
LDFLAGS = -lrbmap
INC = -Iinclude

.PHONY: all
all: $(TARGET)


$(TARGET): $(OBJS)
	@if test ! -d lib; then mkdir lib; fi
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(BUILDDIR)/%.o: $(srcdir)/%.c
	@if test ! -d $(BUILDDIR); then mkdir $(BUILDDIR); fi
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	-rm -f $(BUILDDIR)/* $(TARGET) *.tgz

.PHONY: tester
tester:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) test/tester.c $(INC) $(LDLIBS) -o bin/tester

.PHONY: dist
dist:
	tar -zcvf rbmap.tgz $(AUX)

-include $(OBJS:%.o=%.d)
