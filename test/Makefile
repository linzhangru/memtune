.PHONY: mmc

SOURCES = mmc_acount.c
OBJECTS := $(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))

mmc : $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)
#	mv $@ ../ 
