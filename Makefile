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

generate_circles: bin generate_circles.c
	gcc -o bin/generate_circles generate_circles.c
	./bin/generate_circles
	
draw_circles: bin draw_circles.c
	gcc -o bin/draw_circles draw_circles.c
	./bin/draw_circles

