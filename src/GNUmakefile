.DEFAULT_GOAL := build
C++ ?= g++

.PHONY: build
build: csd

csd: csd.cpp GNUmakefile

	$(C++) $(CFLAGS) \
		-lcurl \
		-lpthread \
		$(shell pkg-config --cflags --libs libcrypto++) \
		$(shell pkg-config --cflags --libs gumbo) \
		$(shell pkg-config --cflags --libs liburiparser) \
		$(shell pkg-config --cflags --libs curlpp) \
		-Wall -Wextra $< -o $@
