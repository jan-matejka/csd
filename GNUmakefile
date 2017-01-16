.DEFAULT_GOAL := build
CXX ?= c++

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib

.PHONY: build
build: libcsd.so csd

.PHONY: install
install:

	install -m755 lib/libcsd.so $(DESTDIR)$(LIBDIR)
	install -m755 src/csd $(DESTDIR)$(BINDIR)

.PHONY: clean
clean:

	$(RM) libcsd.so
	$(RM) csd

libcsd.so: lib/csd.cpp GNUmakefile

	$(CXX) -std=gnu++11 $(CFLAGS) \
		-fPIC \
		-lcurl \
		-lpthread \
		$(shell pkg-config --cflags --libs libcrypto++) \
		$(shell pkg-config --cflags --libs gumbo) \
		$(shell pkg-config --cflags --libs liburiparser) \
		$(shell pkg-config --cflags --libs curlpp) \
		-Wall -Wextra -shared $< -o $@

csd: src/csd.cpp GNUmakefile

	$(CXX) -std=gnu++11 $(CFLAGS) \
		-L. -I./lib -lcsd \
		-Wl,-rpath='$$ORIGIN/../lib' \
		-Wall -Wextra $< -o $@
