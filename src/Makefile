

x86-all: perft

main.o: main.c
meminfo.o: meminfo.c 
ioloading.o: ioloading.c
perft: main.o meminfo.o ioloading.o



%.o: %.c
	$(CC) -Wall $(CFLAGS) $(INC_PARAMS)  -c $< -o $@
