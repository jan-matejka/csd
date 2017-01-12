.DEFAULT_GOAL := build
C++ ?= g++

.PHONY: build
build: csd

csd: csd.cpp GNUmakefile

	$(C++) $(CFLAGS) \
		-Wall -Wextra $< -o $@
