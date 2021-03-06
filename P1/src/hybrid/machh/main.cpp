#include "machh.h"
#include <iostream>
#include <mpi.h>
#include <cmath>

using namespace std;

int main(int argc, char* argv[])
{
	int numberOfProcesses, rank;

	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if ( (numberOfProcesses & (numberOfProcesses-1) ) != 0 && numberOfProcesses != 0) {
		if (rank = 0){
			cout << fixed << "Number of processes need to be power of two"<< endl;
		}
		
		MPI_Finalize();
		return -1;
	}

	auto plot = false;

	if (argc > 1){
		string arg = argv[1];
		if (arg =="-v"){
			plot = true;
		}
	} 

	if (plot) {
		auto maxK = 7;
		for (int k = 1; k <= maxK ; k++){
			if (rank == 0){
				int n = pow(2, k);
				master_task(n, numberOfProcesses);
					
			} else {
				slave_task(rank, numberOfProcesses);
			}
		}
	} else {
		if (rank == 0){
			int n = 1000;
			master_task(n, numberOfProcesses);
				
		} else {
			slave_task(rank, numberOfProcesses);
		}
	}

	MPI_Finalize();
	return 0;
}