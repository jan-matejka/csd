.DEFAULT_GOAL := build
CXX ?= c++

.PHONY: build
.PHONY: clean
.PHONY: install
build clean install:

	$(MAKE) -C lib/ $@
	$(MAKE) -C src/ $@
