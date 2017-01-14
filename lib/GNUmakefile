.DEFAULT_GOAL := build
CXX ?= c++

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib

.PHONY: build
build: libcsd.so

libcsd.so: csd.cpp GNUmakefile

	$(CXX) -std=gnu++11 $(CFLAGS) \
		-fPIC \
		-lcurl \
		-lpthread \
		$(shell pkg-config --cflags --libs libcrypto++) \
		$(shell pkg-config --cflags --libs gumbo) \
		$(shell pkg-config --cflags --libs liburiparser) \
		$(shell pkg-config --cflags --libs curlpp) \
		-Wall -Wextra -shared $< -o $@

.PHONY: install
install:

	install -m755 libcsd.so $(DESTDIR)$(LIBDIR)
