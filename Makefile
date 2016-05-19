
ARCH := x86
PWD:=$(shell pwd)


INC:=$(PWD)/include
INC_PARAMS:=$(foreach d, $(INC), -I$d)
#$(info $(INC_PARAMS))

CFLAGS   += $(INC_PARAMS)
CPPFLAGS += $(INC_PARAMS)

ifeq ($(ARCH), x86)
CC = gcc
AR = ar
#select arm64 compiler in else 
else
CC =
AR = 
endif

LD=$(CC)


.PHONY: all x86-all

all:


x86-all: perft

main.o: main.c
meminfo.o: meminfo.c meminfo.h
ioloading.o: ioloading.c ioloading.h
perft: main.o meminfo.o ioloading.o



%.o: %.c
	$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $(INC) $< -o $@
