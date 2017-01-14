.DEFAULT_GOAL := build
CXX ?= c++

.PHONY: build
build:

	$(MAKE) -C lib/ $@
	$(MAKE) -C src/ $@

.PHONY: install
install:

	$(MAKE) -C lib/ $@
	$(MAKE) -C src/ $@
