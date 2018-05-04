#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv){
	int nproc, pid;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	printf("Process %d running. nproc: %d\n", pid, nproc);

	MPI_Finalize();
	return 0;
}
