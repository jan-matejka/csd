.DEFAULT_GOAL := build
CXX ?= c++

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

.PHONY: build
build: csd

.PHONY: clean
clean:
	$(RM) csd

csd: csd.cpp GNUmakefile

	$(CXX) -std=gnu++11 $(CFLAGS) \
		-L../lib -I../lib -lcsd \
		-Wl,-rpath='$$ORIGIN/../lib' \
		-Wall -Wextra $< -o $@

.PHONY: install
install:

	install -m755 csd $(DESTDIR)$(BINDIR)
