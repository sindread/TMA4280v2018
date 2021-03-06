﻿#include "mach1r.h"
#include <cmath>
#include <iostream>
#include <mpi.h>

using namespace std;

void master_task(const int &n, const int &numberOfProcesses){
	double vi5[n], vi239[n];
	double start, end;

	start = MPI_Wtime();

	vi_parts(n, vi5, vi239);

	int lengthForRank[numberOfProcesses];
	length_of_work(lengthForRank, n, numberOfProcesses);
	
	MPI_Bcast(&lengthForRank, numberOfProcesses-1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&vi5, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&vi239, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	//Reduction with broadcast
	double piPart239=0, piPart5=0, sum5=0, sum239=0;
	MPI_Allreduce(&piPart5, &sum5, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&piPart239, &sum239, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	double pi = 4 * (4 * sum5 - sum239);

	//Manual global reduction
	double sum2=0;
	globalReduce(numberOfProcesses, sum2, lengthForRank);
	MPI_Bcast(&sum2, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	end = MPI_Wtime();

	cout << "Pi is with mach1r, with " << n << " iterations: Pi_" << n <<" = "<< pi <<  endl;
	cout << "Error(PI-pi_" << n << "): E  = " << abs(M_PI-pi) <<  endl;
	cout << "Runtime: Time = " <<  (end-start)*1000 << "ms" << endl;
}

void slave_task(int &rank, int &numberOfProcesses){
	int n=0;
	int slaves = numberOfProcesses-1;
	int WorkerRank = rank -1;
	int lengthForRank[WorkerRank];
	
	MPI_Bcast(&lengthForRank, slaves, MPI_INT, 0, MPI_COMM_WORLD);
	sumVector(lengthForRank, slaves, n);

	double vi5[n], vi239[n];
	MPI_Bcast(&vi5, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&vi239, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	int length = lengthForRank[WorkerRank];
	int index = 0;
	sumVector(lengthForRank, WorkerRank, index);

	double piPart5=0, piPart239=0;
	sumVector(&vi5[index], length, piPart5);
	sumVector(&vi239[index], length, piPart239);

	//Reduction and broadcast with MPI
	double sum5=0, sum239=0;
	MPI_Allreduce(&piPart5, &sum5, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&piPart239, &sum239, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	double partSum = 4 * (4 * sum5 - sum239);
	cout << "Sum in slave number " << rank << " after global reduction (MPI): Pi_" << n <<" = "<< partSum << endl;

	//Manual global reduction
	MPI_Send(&vi5[index], length, MPI_DOUBLE, 0, TAG_PARTSUM1, MPI_COMM_WORLD);
	MPI_Send(&vi239[index], length, MPI_DOUBLE, 0, TAG_PARTSUM2, MPI_COMM_WORLD);
	double sum2 = 0.0;
	MPI_Bcast(&sum2, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	cout << "Sum in slave number " << rank << " after global reduction (manual): Pi_" << n <<" = "<< sum2 << endl;
}

void length_of_work(int* lengthForRank, const int &n, const int &numberOfProcesses){
	int length = n / (numberOfProcesses-1);
	int rest = n % (numberOfProcesses-1);
	
	for(int i = 0; i < numberOfProcesses-1; i++){
		lengthForRank[i] = length;

		if(i < rest){
			lengthForRank[i]++;
		}
	}
}

void vi_parts(const int& n, double* vi5, double* vi239)
{
	const auto x1 = double(1)/5;
	const auto x2 = double(1)/239;

	for (int i = 1; i <= n; i++){
		vi5[i-1] = arctan_part(i, x1);
		vi239[i-1] = arctan_part(i, x2);
	}
}

double arctan_part(const int& i, const double& x)
{
	const auto part1 = pow(-1, i - 1);
	const auto part3 = (2 * i) - 1;
	const auto part2 = pow(x, part3); 

	return part1 * (part2 / part3);;
}

void sumVector(const double* vector, const int& length, double& sum){
	for(int i = 0; i < length; i++){
		sum += vector[i];	
	}
}

void sumVector(const int* vector, const int& length, int& sum){
	for(int i = 0; i < length; i++){
		sum += vector[i];	
	}
}

void globalReduce(const int &numberOfProcesses, double &sum, const int *lengthForProcesses){
	//MPI_Allgather(&myArray, length, MPI_DOUBLE, array&, )
	double sum1 = 0, sum2 = 0;
	for (int i = 1; i < numberOfProcesses; i++){
		int length = lengthForProcesses[i-1];
		double array5[length];
		double array239[length];
		MPI_Recv(&array5, length, MPI_DOUBLE, i, TAG_PARTSUM1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&array239, length, MPI_DOUBLE, i, TAG_PARTSUM2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		sumVector(array5, length, sum1);
		sumVector(array239, length, sum2);
	}
	sum = 4*(4*sum1 - sum2);
}