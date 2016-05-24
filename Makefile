ARCH ?= x86
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


$(info SYSROOT: $(SYSROOT))

ifeq ($(ARCH), x86)
CC = gcc
#select arm64 compiler in else 
else
    ifeq ($(ARCH), arm64)
        CC = /home/zhalin/bin/aarch64-linux-android-4.8/bin/aarch64-linux-android-gcc
        SYSROOT = /home/zhalin/bin/aarch64-linux-android-4.8/sysroot
        CFLAGS += --sysroot=$(SYSROOT)
    else
        $(error "unknown architecture")
    endif

endif

$(info CC:$(CC))

export CC
export LD
export AR

export CFLAGS
export CPPFLAGS
export LDFLAGS

TARGET  := mma
export TARGET

.PHONY: all test clean

all:
	make -C ./src all

test:
	make -C ./test mmc esti

clean:
	find -iname "*.o" | xargs rm -f
	find -iname "*~"  | xargs rm -f
	find -iname "cscope*" | xargs rm -rf
	find -iname "*#*" | xargs rm -rf
	rm -f $(TARGET) test/mmc test/esti test/log
