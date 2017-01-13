.DEFAULT_GOAL := build
C++ ?= g++

.PHONY: build
build: csd

csd: csd.cpp GNUmakefile

	$(C++) $(CFLAGS) \
		-lcurl \
		$(shell pkg-config --cflags --libs gumbo) \
		$(shell pkg-config --cflags --libs liburiparser) \
		$(shell pkg-config --cflags --libs curlpp) \
		-Wall -Wextra $< -o $@
