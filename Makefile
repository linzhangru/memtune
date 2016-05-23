ARCH := x86
PWD:=$(shell pwd)

export ARCH


INC:=$(PWD)/include
INC_PARAMS:=$(foreach d, $(INC), -I$d)
#$(info $(INC_PARAMS))

LIBS := -L.
export LIBS

CFLAGS   += $(INC_PARAMS)
CPPFLAGS += $(INC_PARAMS)
LDFLAGS  += -lc -lm

export CFLAGS
export CPPFLAGS
export LDFLAGS


ifeq ($(ARCH), x86)
CC = gcc
LD = ld
AR = ar
#select arm64 compiler in else 
else
CC =
LD =
AR = 
endif


export CC
export LD
export AR


TARGET  := mma
export TARGET

.PHONY: all test clean

all:
	make -C ./src x86-all

test:
	make -C ./test mmc

clean:
	find -iname "*.o" | xargs rm -f
	find -iname "*~"  | xargs rm -f
	find -iname "cscope*" | xargs rm -rf
	find -iname "*#*" | xargs rm -rf
	rm -f $(TARGET) test/mmc
