#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include "hitbox.h"

int load_configuration();
void categorize_circles();
void log(const char* msg);

World world;
Circle* circles = NULL;
uint32_t ncircles = 0;
int nproc, pid;


int main(int argc, char** argv){
	struct timespec start, finish;
	timespec_get(&start, TIME_UTC);

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	printf("Process %d running. nproc: %d\n", pid, nproc);
	if(load_configuration()){
		return 1;
	}
	categorize_circles();


	if(world.areas != NULL)
		free(world.areas);
	if(circles != NULL)
		free(circles);
	MPI_Finalize();
	timespec_get(&finish, TIME_UTC);
	printf("Process %d finished in %.4f ms\n", pid, ((double)finish.tv_nsec - start.tv_nsec)/1000000);
	return 0;
}

/* Calculations */

void categorize_circles(){
	uint32_t delta = (ncircles / nproc);
	uint32_t to = (pid + 1) * delta;
	uint32_t from = pid * delta;
	if(to > ncircles){
		to = ncircles;
	}
	for(int i = from; i < to; i++){
		circles[i].area_index = get_area_id(&world, &circles[i]);
		//printf("Circle %d belongs to the area %d.\n", circles[i].id, circles[i].area_index);
	}

	uint32_t msg[2];
	if(pid == 0){
		uint32_t collectCount = ncircles - delta;
		log("Collecting results.");

		for(int i = 0; i < collectCount; i++){
			MPI_Recv(msg, 2, MPI_UINT32_T, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < ncircles; j++){
				if(circles[j].id == msg[0]){
					circles[j].area_index = msg[1];
					break;
				}
			}
		}
		log("Collecting finished.");
	}else {
		log("Sending results.");
		for(int i = from; i < to; i++){
			msg[0] = circles[i].id;
			msg[1] = circles[i].area_index;
			MPI_Send(msg, 2, MPI_UINT32_T, 0, 10, MPI_COMM_WORLD);
		}
		log("Sending finished.");
	}
}

/* CONFIG LOADER */

uint32_t read_till(FILE* fd, char* buffer, char separator, uint32_t buflen){
	char ch;
	uint32_t curr = 0;
	while(((ch = (char)fgetc(fd)) != separator && ch != '\n' && ch != EOF) && curr < buflen){
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
		return 0.0;
	}
	return atoi(buffer);
}

int load_configuration(){
	// World
	log("Loading world.");
	world.areas = NULL;
	char buffer[256];
	char current;
	int i;
	FILE* world_fd = fopen("world.csv", "r");
	if(world_fd == NULL){
		log("Could not load world.csv.");
		return 1;
	}
	fscanf(world_fd, "%d,", &world.w);
	fscanf(world_fd, "%d,", &world.h);
	fscanf(world_fd, "%d,", &world.narea);
	fscanf(world_fd, "%d\n", &world.narea_width);
	fclose(world_fd);

	world.areas = (Area*)malloc(world.narea * sizeof(Area));
	double dwidth = world.w / world.narea_width;
	double dheight = world.h / (world.narea / world.narea_width);
	for(i = 0; i < world.narea; i++){
		world.areas[i].x = dwidth * (i % world.narea_width);
		world.areas[i].y = dheight * (int)(i / world.narea_width);
		world.areas[i].h = dheight;
		world.areas[i].w = dwidth;
	}

	log("Loading world completed.");
	// Circle
	log("Loading circles.");
	FILE* circle_fd = fopen("circle.csv", "r");
	if(circle_fd == NULL){
		log("Could not load circle.csv.");
		return 1;
	}
	ncircles = next_int(circle_fd, buffer, ',', 256);
	circles = (Circle*)malloc(ncircles * sizeof(Circle));
	for(i = 0; i < ncircles; i++){
		circles[i].id = next_int(circle_fd, buffer, ',', 256);
		circles[i].x = next_double(circle_fd, buffer, ',', 256);
		circles[i].y = next_double(circle_fd, buffer, ',', 256);
		circles[i].r = next_double(circle_fd, buffer, ',', 256);
	}

	fclose(circle_fd);
	log("Loading circles completed.");
	return 0;
}


void log(const char* msg) {
	printf("PID: %d - %s\n", pid, msg);
}