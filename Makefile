CC := gcc
CFLAGS := -O2 -Wall -g -ansi -pedantic $(OPTFLAGS)
CFLAGS+= -MMD

SRCDIR := src
BUILDDIR := build
TARGET := bin/rbmap

SOURCES := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
LIB := -Llib
INC := -Iinclude


$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -r $(BUILDDIR) $(TARGET)

tester:
	$(CC) $(CFLAGS) test/tester.c $(INC) $(LIB) -o bin/tester

.PHONY: clean

-include $(OBJECTS:%.o=%.d)
