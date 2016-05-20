

x86-all: perft




SOURCES := main.c meminfo.c ioloading.c sysctlvm.c
OBJECTS := $(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))  

$(info LD_LIBRARY_PATH:$(LD_LIBRARY_PATH))


$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)
	mv $(TARGET) ../

%.o: %.c
	$(CC) -Wall $(CFLAGS) $(INC_PARAMS)  -c $< -o $@
