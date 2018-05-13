#include <stdio.h>
#include <mpi.h>

#include "hitbox.h"

int load_configuration();
void log(const char* msg);

World world;
int nproc, pid;


int main(int argc, char** argv){
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	printf("Process %d running. nproc: %d\n", pid, nproc);
	/**
	 * The master process wil distribute the tasks and configuration.
	 * 
	 */
	load_configuration();
	if(pid == 0){
	}else {

	}

	MPI_Finalize();
	return 0;
}

uint32_t read_till(FILE* fd, char* buffer, char separator, uint32_t buflen){
	char ch;
	uint32_t curr = 0;
	while((ch = getc(fd)) != separator || ch != '\n' || ch != EOF && curr < buflen - 1){
		buffer[curr++] = ch;
	}
	buffer[curr] = 0;
	return curr;
}

double next_double(FILE* fd, char* buffer, char separator, uint32_t buflen){
	uint32_t count = read_till(fd, buffer, separator, buflen);
	if(count < 1){
		return 0.0;
	}
	return atof(buffer);
}
uint32_t next_int(FILE* fd, char* buffer, char separator, uint32_t buflen){
	uint32_t count = read_till(fd, buffer, separator, buflen);
	if(count < 1){
		return 0;
	}
	return atoi(buffer);
}

int load_configuration(){
	// World
	log("Loading world.");
	char buffer[256];
	char current;
	FILE* world_fd = open("world.csv");
	world.w = next_int(world_fd, buffer, ',', 256);
	world.h = next_int(world_fd, buffer, ',', 256);
	world.narea = next_int(world_fd, buffer, ',', 256);
	world.narea_width = next_int(world_fd, buffer, ',', 256);
	world.areas = (Area*)malloc(world.narea * sizeof(Area));
	close(world_fd);
	log("Loading world completed.");
	// Circle
	log("Loading circles.");
	circle_fd = open("circle.csv");

	close(circle_fd);
	log("Loading circles completed.");
	return 0;
}


void log(const char* msg) {
	printf("PID: %d - %s", pid, msg);
}