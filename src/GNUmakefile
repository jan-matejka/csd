.DEFAULT_GOAL := build
C++ ?= g++

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

.PHONY: build
build: csd

csd: csd.cpp GNUmakefile

	$(C++) -std=gnu++11 $(CFLAGS) \
		-L../lib -I../lib -lcsd \
		-Wl,-rpath='$$ORIGIN/../lib' \
		-Wall -Wextra $< -o $@

.PHONY: install
install:

	install -m755 csd $(DESTDIR)$(BINDIR)
