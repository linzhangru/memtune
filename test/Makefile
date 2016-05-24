.PHONY: mmc esti

SOURCES = mmc_acount.c
OBJECTS := $(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))

mmc : $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)


SOURCES = esti.c
OBJECTS := $(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))
esti : esti.o
	$(CC) $^ -o $@ $(LDFLAGS)
