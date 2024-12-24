.PHONY: all clean run

include config.cfg

ifeq ($(DEBUG), n)
.SILENT:
endif

all: main

main: main.o
	$(LD) $(LDFLAGS) -o main main.o

main.o: main.c
	$(CC) $(CFLAGS) -o main.o main.c

clean:
	$(RM) main.o main log.txt

run: main
	stdbuf -o0 ./main > log.txt

run-2-core: main
	taskset --cpu-list 2 stdbuf -o0 ./main > log.txt

run-18-core: main
	taskset --cpu-list 18 stdbuf -o0 ./main > log.txt

run-19-core: main
	taskset --cpu-list 19 stdbuf -o0 ./main > log.txt
