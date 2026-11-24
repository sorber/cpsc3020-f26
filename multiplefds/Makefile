CC ?= gcc
CFLAGS ?= -std=gnu2x -g -Og -ggdb3
LDFLAGS ?=

DEPFLAGS:= -MMD -MP
OBJDIR:= objs
SRCS:=$(wildcard *.c)
APPS:=$(SRCS:.c=)

all: $(APPS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

%: $(OBJDIR)/%.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

examples.tar.xz: $(SRCS) Makefile examples.h
	tar --numeric-owner -cvJf $@ $^
clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(APPS) examples.tar.xz
.PHONY: all clean

-include $(wildcard $(OBJDIR)/*.d)
.SUFFIXES: