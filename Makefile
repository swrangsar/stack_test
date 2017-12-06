SHELL = /bin/sh

CC = cc
RANLIB = ranlib
ARFLAGS = rcs

CFLAGS = -g
ALL_CFLAGS = -Wall -O2 $(CFLAGS)

srcdir = src
BUILDDIR = build
TARGET = lib/librbmap.a


SRCS = $(wildcard $(srcdir)/*.c)
OBJS = $(patsubst $(srcdir)/%.c,$(BUILDDIR)/%.o,$(SRCS))
AUX = $(srcdir) Makefile include test
LDFLAGS = -Llib
LDLIBS = -lrbmap
CPPFLAGS = -MMD -Iinclude


.PHONY: all
all: $(TARGET)


$(TARGET): $(OBJS)
	@if test ! -d lib; then mkdir lib; fi
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(BUILDDIR)/%.o: $(srcdir)/%.c
	@if test ! -d $(BUILDDIR); then mkdir $(BUILDDIR); fi
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -o $@ $<

.PHONY: clean
clean:
	-$(RM) $(BUILDDIR)/* $(TARGET) *.tgz

.PHONY: test
test:
	@if test ! -d bin; then mkdir bin; fi
	$(CC) $(CPPFLAGS) $(ALL_CFLAGS) $(LDFLAGS) test/map_test.c $(LDLIBS) -o bin/map_test
	$(CC) $(CPPFLAGS) $(ALL_CFLAGS) $(LDFLAGS) test/queue_test.c $(LDLIBS) -o bin/queue_test

.PHONY: dist
dist:
	tar -zcvf rbmap.tgz $(AUX)

-include $(patsubst %.o,%.d,$(OBJS))
