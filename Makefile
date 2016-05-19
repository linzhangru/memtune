ARCH := x86
PWD:=$(shell pwd)

export ARCH


INC:=$(PWD)/include
INC_PARAMS:=$(foreach d, $(INC), -I$d)
$(info $(INC_PARAMS))

CFLAGS   += $(INC_PARAMS)
CPPFLAGS += $(INC_PARAMS)

export CFLAGS
export CPPFLAGS

.PHONY: all

all:
	make -C ./src x86-all
