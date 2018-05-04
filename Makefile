CC=mpiCC
MPIRUN=mpirun
MPIOPTS=-n 2
CFLAGS=-I./include

build:bin/main

bin/main: bin main.c include/hitbox.h
	$(CC) $(CFLAGS) -o bin/main main.c

bin:
	mkdir -p bin

run: bin/main
	$(MPIRUN) $(MPIOPTS) bin/main
